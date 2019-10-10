#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "usart.h"
#include "stm32f4xx_hal.h"
#include "dm8606c_stp.h"
#include "timeouts.h"
#include "pbuf.h"
#include "netif/ethernet.h"
#include "ethernetif.h"

#define PORT_FX_1		  0
#define PORT_FX_2		  1
#define PORT_RJ45     2 
#define PORT_CPU		  3

#define BPDU_HDR_OFFSET  (SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR + SIZEOF_LLC_FIELD)

extern ETH_HandleTypeDef heth;
extern uint8_t MAC[6];
extern struct netif gnetif;
struct STP_BRIDGE* bridge;

uint32_t stp_timestamp;
static struct pbuf *bpdu_pbuf;
const struct eth_addr ethbridge =
  {
    { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 } };

#ifdef STRONGLY_SPEC_802_1W
static void StpCallback_EnableLearning (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, unsigned int enable, unsigned int timestamp)
  {
//	ENET_MIIWriteRegister (20, 6, i);
    uint32_t reg = 0;
    uint32_t PortPhy = PortIndexMapToPhy(portIndex);

    heth.Init.PhyAddress = 0x08 + PortPhy;
//  HAL_ETH_ReadPHYRegister(&heth, 0x19,&reg);
    if(enable)
      {
	reg = (reg & ~3u) | 2u;
      }
    else
      {
	reg = (reg & ~3u) | 1u;
      }
    HAL_ETH_WritePHYRegister(&heth, 0x19,reg);
  }

static void StpCallback_EnableForwarding (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, unsigned int enable, unsigned int timestamp)
  {
    uint32_t reg = 0;
    uint32_t PortPhy = PortIndexMapToPhy(portIndex);

    heth.Init.PhyAddress = 0x08 + PortPhy;
    HAL_ETH_ReadPHYRegister(&heth, 0x19,&reg);
    if(enable)
      {
	reg = (reg & ~3u);
      }
    else
      {
	reg = (reg & ~3u) | 1u;
      }
    HAL_ETH_WritePHYRegister(&heth, 0x19,reg);

  }
#else
static void
StpCallback_SetPortState (const struct STP_BRIDGE* bridge,
			  unsigned int portIndex, unsigned int treeIndex,
			  unsigned int state, unsigned int timestamp)
{
  uint32_t reg = 0;
  uint32_t PortPhy = PortIndexMapToPhy (portIndex);

  heth.Init.PhyAddress = 0x08 + PortPhy;
  HAL_ETH_ReadPHYRegister (&heth, 0x19, &reg);

  switch (state)
    {
    case 1:   //DISCARDING
      reg = (reg & ~3u) | 1u;
//      printf ("XXXXXXXXXXXXXXXXset port %d to state discarding\r\n", portIndex);
      break;
    case 2:   //LEARNING
      reg = (reg & ~3u) | 2u;
//      printf ("XXXXXXXXXXXXXXXXset port %d to state learning\r\n", portIndex);
      break;
    case 3:   //FORWARDING
      reg = (reg & ~3u);
//      printf ("XXXXXXXXXXXXXXXXset port %d to state forwarding\r\n", portIndex);
      break;
    default:
      break;
    }
  HAL_ETH_WritePHYRegister (&heth, 0x19, reg);
}
#endif

static void*
StpCallback_TransmitGetBuffer (const struct STP_BRIDGE* bridge,
			       unsigned int portIndex, unsigned int bpduSize,
			       unsigned int timestamp)
{
  if (portIndex == PORT_CPU)
    {
      // The library is trying to send a BPDU to the RMII port, which we know is us.
      // Let's not allow it cause it would be a waste of bandwidth.
      return NULL;
    }

  /* allocate a pbuf for the outgoing request packet */
  bpdu_pbuf = pbuf_alloc (PBUF_RAW_TX, BPDU_HDR_OFFSET + bpduSize, PBUF_RAM);
  /* could allocate a pbuf for an ARP request? */
  if (bpdu_pbuf == NULL)
    {
      return NULL;
    }

  struct eth_hdr* ethhdr = (struct eth_hdr *) bpdu_pbuf->payload;
  ETHADDR32_COPY(&ethhdr->dest, &ethbridge);
  ETHADDR16_COPY(&ethhdr->src, STP_GetBridgeAddress (bridge)->bytes);
  ethhdr->type = PP_HTONS(ETHTYPE_VLAN);

  struct eth_vlan_hdr *vlan = (struct eth_vlan_hdr*) (((char*) ethhdr)
      + SIZEOF_ETH_HDR);
  vlan->tpid = lwip_htons (bpdu_pbuf->len - (SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR));
  vlan->prio_vid = lwip_htons (
      (u16_t) ((1 << (PortIndexMapToPhy (portIndex) + 8)) | 0x4021)); //speical tag

  char* llc = ((char*) ethhdr) + SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR;
  *llc++ = 0x42;
  *llc++ = 0x42;
  *llc++ = 0x03;

  return (((u8_t*) bpdu_pbuf->payload) + BPDU_HDR_OFFSET);
}

static void
StpCallback_TransmitReleaseBuffer (const struct STP_BRIDGE* bridge,
				   void* bufferReturnedByGetBuffer)
{
  gnetif.linkoutput (&gnetif, bpdu_pbuf);
  pbuf_free (bpdu_pbuf);
  bpdu_pbuf = NULL;
}

static void
StpCallback_FlushFdb (const struct STP_BRIDGE* bridge, unsigned int portIndex,
		      unsigned int treeIndex, enum STP_FLUSH_FDB_TYPE flushType)
{
  uint32_t reg = 0;
  heth.Init.PhyAddress = 0x15;
  HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);

  while (reg >> 15)
    {
      HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);
    }

  reg = 0x10;
  HAL_ETH_WritePHYRegister (&heth, 0x10, reg);
  osDelay (5);

  HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);
  while (reg >> 15)
    {
      HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);
    }
}

static void
StpCallback_DebugStrOut (const struct STP_BRIDGE* bridge, int portIndex,
			 int treeIndex, const char* nullTerminatedString,
			 unsigned int stringLength, unsigned int flush)
{
  HAL_UART_Transmit (&huart5, (uint8_t *) nullTerminatedString, stringLength,
		     0xffff);
}

// See long comment at the end of 802_1Q_2011_procedures.cpp.
static void
StpCallback_OnTopologyChange (const struct STP_BRIDGE* bridge)
{
  // do nothing in this demo app
  //printf ("TC\r\n");
}

// See long comment at the end of 802_1Q_2011_procedures.cpp.
static void
StpCallback_OnNotifiedTC (const struct STP_BRIDGE* bridge,
			  unsigned int portIndex, unsigned int treeIndex,
			  unsigned int timestamp)
{
  uint32_t reg = 0;
  heth.Init.PhyAddress = 0x15;
  HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);

  while (reg >> 15)
    {
      HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);
    }

  reg = 0x10;
  HAL_ETH_WritePHYRegister (&heth, 0x10, reg);
  osDelay (5);

  HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);
  while (reg >> 15)
    {
      HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);
    }
}

void
StpCallback_OnPortRoleChanged (const struct STP_BRIDGE* bridge,
			       unsigned int portIndex, unsigned int treeIndex,
			       enum STP_PORT_ROLE role, unsigned int timestamp)
{
}

void
StpCallback_OnConfigChanged (const struct STP_BRIDGE* bridge,
			     unsigned int timestamp)
{
}

static void*
StpCallback_AllocAndZeroMemory (unsigned int size)
{
  void* result = malloc (size);
  assert(result != NULL);
  memset (result, 0, size);
  return result;
}

static void
StpCallback_FreeMemory (void* p)
{
  free (p);
}

static struct STP_CALLBACKS const Callbacks =
  {
#ifdef STRONGLY_SPEC_802_1W
      StpCallback_EnableLearning,
      StpCallback_EnableForwarding,
#else
      StpCallback_SetPortState,
#endif
      StpCallback_TransmitGetBuffer, StpCallback_TransmitReleaseBuffer,
      StpCallback_FlushFdb, StpCallback_DebugStrOut,
      StpCallback_OnTopologyChange, StpCallback_OnNotifiedTC,
      StpCallback_OnPortRoleChanged, StpCallback_OnConfigChanged,
      StpCallback_AllocAndZeroMemory, StpCallback_FreeMemory };

uint32_t
PortIndexMapToPhy (uint32_t PortIndex)
{
  uint32_t PortPhy = 0;
  if (PortIndex == PORT_FX_1)
    PortPhy = 1;
  if (PortIndex == PORT_FX_2)
    PortPhy = 2;
  if (PortIndex == PORT_RJ45)
    PortPhy = 3;
  if (PortIndex == PORT_CPU)
    PortPhy = 5;
  return PortPhy;
}
uint32_t
PortPhyMapToIndex (uint32_t PortPhy)
{
  uint32_t PortIndex = 0;
  if (PortPhy == 1)
    PortIndex = PORT_FX_1;
  if (PortPhy == 2)
    PortIndex = PORT_FX_2;
  if (PortPhy == 3)
    PortIndex = PORT_RJ45;
  if (PortPhy == 5)
    PortIndex = PORT_CPU;
  return PortIndex;
}

void
stp_scan_tmr (void)
{
  stp_timestamp = HAL_GetTick ();
  uint32_t PortIndex, reg = 0;
  for (PortIndex = 0; PortIndex < 3; PortIndex++)
    {
      // See the switch per port status data Register, page 28 of the DM8606C datasheet.
      heth.Init.PhyAddress = 0x08 + PortIndexMapToPhy (PortIndex);
      HAL_ETH_ReadPHYRegister (&heth, 0x10, &reg);
      if ((reg & 1) && !STP_GetPortEnabled (bridge, PortIndex))
	{
	  // link is now good
	  STP_OnPortEnabled (bridge, PortIndex, 100, true, stp_timestamp);
	}
      else if (!(reg & (1)) && STP_GetPortEnabled (bridge, PortIndex))
	{
	  // link is now down
	  STP_OnPortDisabled (bridge, PortIndex, stp_timestamp);
	}
    }
//	sys_timeout(STP_SCAN_TMR_INTERVAL, stp_scan_tmr, NULL);
}

void
stp_stack_tmr (void)
{
  stp_timestamp = HAL_GetTick ();
  STP_OnOneSecondTick (bridge, stp_timestamp);
//	sys_timeout(STP_STACK_TMR_INTERVAL, stp_stack_tmr, NULL);
}

// Initialize and start the STP library.
void
stp_init (void)
{
  stp_timestamp = HAL_GetTick ();
  bridge = STP_CreateBridge (4, 0, 0, &Callbacks, MAC, 2);
  STP_EnableLogging (bridge, 0);

  STP_SetBridgePriority (bridge, 0, 0x9000, stp_timestamp);

  // Port CPU of the switch chip, which on my board is the MII connection to the microcontroller's Ethernet module, is an Edge port.
  STP_SetPortAdminEdge (bridge, PORT_CPU, true, stp_timestamp);

//  STP_SetPortAutoEdge (bridge, PORT_RJ45, true, stp_timestamp);
  STP_SetPortAdminEdge (bridge, PORT_RJ45, true, stp_timestamp);

  // Port CPU (MII) is always enabled.
  STP_OnPortEnabled (bridge, PORT_CPU, 100, true, stp_timestamp);

  // Force all ports as P2P, since my connections are all P2P (no multidrop coaxial or something).
  STP_SetPortAdminPointToPointMAC (bridge, PORT_FX_1, STP_ADMIN_P2P_FORCE_TRUE,
				   stp_timestamp);
  STP_SetPortAdminPointToPointMAC (bridge, PORT_FX_2, STP_ADMIN_P2P_FORCE_TRUE,
				   stp_timestamp);
  STP_SetPortAdminPointToPointMAC (bridge, PORT_RJ45, STP_ADMIN_P2P_FORCE_TRUE,
				   stp_timestamp);
  STP_SetPortAdminPointToPointMAC (bridge, PORT_CPU, STP_ADMIN_P2P_FORCE_TRUE,
				   stp_timestamp);

  STP_StartBridge (bridge, stp_timestamp);

//  sys_timeout(STP_STACK_TMR_INTERVAL, stp_stack_tmr, NULL);
//  sys_timeout(STP_SCAN_TMR_INTERVAL, stp_scan_tmr, NULL);

}

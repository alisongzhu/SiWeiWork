
// This file is part of the mstp-lib library, available at https://github.com/adigostin/mstp-lib
// Copyright (c) 2011-2017 Adi Gostin, distributed under Apache License v2.0.
//
// Find documentation for all STP functions in the _help directory in the source code tree.
//
// Improvements and bug fixes may have been made in the source code, but not put up yet as a zip download;
// Have a look at the latest source code on the Sourceforge website to learn about these.
//
// Also check the open tickets to learn about known issues: http://sourceforge.net/p/mstp-lib/tickets

#ifndef MSTP_LIB_H
#define MSTP_LIB_H

struct STP_BRIDGE;

enum STP_FLUSH_FDB_TYPE
{
	STP_FLUSH_FDB_TYPE_IMMEDIATE,
	STP_FLUSH_FDB_TYPE_RAPID_AGEING,
};

enum STP_PORT_ROLE
{
	STP_PORT_ROLE_UNKNOWN	= 0,
	STP_PORT_ROLE_DISABLED	= 5,
	STP_PORT_ROLE_ROOT		= 6,
	STP_PORT_ROLE_DESIGNATED= 7,
	STP_PORT_ROLE_ALTERNATE = 8,
	STP_PORT_ROLE_BACKUP    = 9,
	STP_PORT_ROLE_MASTER	= 10,
};

#ifdef STRONGLY_SPEC_802_1W
typedef void  (*STP_CALLBACK_ENABLE_LEARNING)				(const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, unsigned int enable, unsigned int timestamp);
typedef void  (*STP_CALLBACK_ENABLE_FORWARDING)				(const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, unsigned int enable, unsigned int timestamp);
#else
typedef void  (*STP_CALLBACK_SET_PORT_STATE)(const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, unsigned int state, unsigned int timestamp)        ;
#endif
typedef void* (*STP_CALLBACK_TRANSMIT_GET_BUFFER)			(const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int bpduSize, unsigned int timestamp);
typedef void  (*STP_CALLBACK_TRANSMIT_RELEASE_BUFFER)		(const struct STP_BRIDGE* bridge, void* bufferReturnedByGetBuffer);
typedef void  (*STP_CALLBACK_FLUSH_FDB)						(const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, enum STP_FLUSH_FDB_TYPE flushType);
typedef void  (*STP_CALLBACK_DEBUG_STR_OUT)					(const struct STP_BRIDGE* bridge, int portIndex, int treeIndex, const char* nullTerminatedString, unsigned int stringLength, unsigned int flush);
typedef void  (*STP_CALLBACK_ON_TOPOLOGY_CHANGE)			(const struct STP_BRIDGE* bridge);
typedef void  (*STP_CALLBACK_ON_NOTIFIED_TOPOLOGY_CHANGE)	(const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, unsigned int timestamp);
typedef void  (*STP_CALLBACK_PORT_ROLE_CHANGED)             (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, enum STP_PORT_ROLE role, unsigned int timestamp);
typedef void  (*STP_CALLBACK_CONFIG_CHANGED)                (const struct STP_BRIDGE* bridge, unsigned int timestamp);
typedef void* (*STP_CALLBACK_ALLOC_AND_ZERO_MEMORY) (unsigned int size);
typedef void  (*STP_CALLBACK_FREE_MEMORY) (void* p);

struct STP_CALLBACKS
{
#ifdef STRONGLY_SPEC_802_1W
	STP_CALLBACK_ENABLE_LEARNING			 enableLearning;
	STP_CALLBACK_ENABLE_FORWARDING			 enableForwarding;
#else
	STP_CALLBACK_SET_PORT_STATE        setPortState;
#endif
	STP_CALLBACK_TRANSMIT_GET_BUFFER		 transmitGetBuffer;
	STP_CALLBACK_TRANSMIT_RELEASE_BUFFER	 transmitReleaseBuffer;
	STP_CALLBACK_FLUSH_FDB					 flushFdb;
	STP_CALLBACK_DEBUG_STR_OUT				 debugStrOut;
	STP_CALLBACK_ON_TOPOLOGY_CHANGE			 onTopologyChange;
	STP_CALLBACK_ON_NOTIFIED_TOPOLOGY_CHANGE onNotifiedTopologyChange;
	STP_CALLBACK_PORT_ROLE_CHANGED           onPortRoleChanged;
	STP_CALLBACK_CONFIG_CHANGED              onConfigChanged;
	STP_CALLBACK_ALLOC_AND_ZERO_MEMORY		 allocAndZeroMemory;
	STP_CALLBACK_FREE_MEMORY				 freeMemory;
};

// 6.6.3 Point-to-point parameters
enum STP_ADMIN_P2P
{
	// There's logic in the STP library that counts on AUTO being equal to 0, so don't move it from the top of the enumeration.
	STP_ADMIN_P2P_AUTO,
	STP_ADMIN_P2P_FORCE_TRUE,
	STP_ADMIN_P2P_FORCE_FALSE,
};

enum STP_VERSION
{
	STP_VERSION_LEGACY_STP = 0,
	STP_VERSION_RSTP = 2,
	STP_VERSION_MSTP = 3,
};

// 13.7
struct STP_MST_CONFIG_ID
{
	unsigned char ConfigurationIdentifierFormatSelector;// 1)
	char ConfigurationName [32];            // 2)
	unsigned char RevisionLevelHigh;        // 3)
	unsigned char RevisionLevelLow;
	unsigned char ConfigurationDigest [16]; // 4)

	#ifdef __cplusplus
	bool operator== (const STP_MST_CONFIG_ID& rhs) const;
	bool operator< (const STP_MST_CONFIG_ID& rhs) const;
	void Dump (STP_BRIDGE* bridge, int port, int tree) const;
	#endif
};

// Six-byte MAC address, not aligned in memory.
struct STP_BRIDGE_ADDRESS
{
	unsigned char bytes[6];
	#ifdef __cplusplus
	bool operator== (const STP_BRIDGE_ADDRESS& rhs) const;
	bool operator!= (const STP_BRIDGE_ADDRESS& rhs) const;
	#endif
};

#ifdef __cplusplus
extern "C" {
#endif

struct STP_BRIDGE* STP_CreateBridge (unsigned int portCount,
									 unsigned int mstiCount,
									 unsigned int maxVlanNumber,
									 const struct STP_CALLBACKS* callbacks,
									 const unsigned char bridgeAddress[6],
									 unsigned int debugLogBufferSize);
void STP_DestroyBridge (struct STP_BRIDGE* bridge);

void STP_StartBridge (struct STP_BRIDGE* bridge, unsigned int timestamp);
void STP_StopBridge (struct STP_BRIDGE* bridge, unsigned int timestamp);
unsigned int STP_IsBridgeStarted (const struct STP_BRIDGE* bridge);

void STP_EnableLogging (struct STP_BRIDGE* bridge, unsigned int enable);
unsigned int STP_IsLoggingEnabled (const struct STP_BRIDGE* bridge);

unsigned int STP_GetPortCount (const struct STP_BRIDGE* bridge);
unsigned int STP_GetMstiCount (const struct STP_BRIDGE* bridge);

enum STP_VERSION STP_GetStpVersion (const struct STP_BRIDGE* bridge);
void STP_SetStpVersion (struct STP_BRIDGE* bridge, enum STP_VERSION version, unsigned int timestamp);

// Call this when you receive a BPDU.
void STP_OnBpduReceived (struct STP_BRIDGE* bridge, unsigned int portIndex, const unsigned char* bpdu, unsigned int bpduSize, unsigned int timestamp);

// Call this every time the bridge's MAC address changes while STP is running.
void STP_SetBridgeAddress (struct STP_BRIDGE* bridge, const unsigned char* address, unsigned int timestamp);
const struct STP_BRIDGE_ADDRESS* STP_GetBridgeAddress (const struct STP_BRIDGE* bridge);

// Call these whenever one of the ports changes state. See 13.25.31 portEnabled in 802.1Q-2011 for details.
void STP_OnPortEnabled (struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int speedMegabitsPerSecond, unsigned int detectedPointToPointMAC, unsigned int timestamp);
void STP_OnPortDisabled (struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int timestamp);

void STP_OnOneSecondTick (struct STP_BRIDGE* bridge, unsigned int timestamp);

// 0-61440 in steps of 4096
void           STP_SetBridgePriority (struct STP_BRIDGE* bridge, unsigned int treeIndex, unsigned short bridgePriority, unsigned int timestamp);
unsigned short STP_GetBridgePriority (const struct STP_BRIDGE* bridge, unsigned int treeIndex);

// 0-240 in steps of 16
void          STP_SetPortPriority (struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex, unsigned char portPriority, unsigned int timestamp);
unsigned char STP_GetPortPriority (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex);
unsigned short STP_GetPortIdentifier (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex);

void STP_SetPortAdminEdge (struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int adminEdge, unsigned int timestamp);
unsigned int STP_GetPortAdminEdge (const struct STP_BRIDGE* bridge, unsigned int portIndex);

void STP_SetPortAutoEdge (struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int autoEdge, unsigned int timestamp);
unsigned int STP_GetPortAutoEdge (const struct STP_BRIDGE* bridge, unsigned int portIndex);

void          STP_SetPortAdminPointToPointMAC (struct STP_BRIDGE* bridge, unsigned int portIndex, enum STP_ADMIN_P2P adminPointToPointMAC, unsigned int timestamp);
enum STP_ADMIN_P2P STP_GetPortAdminPointToPointMAC (const struct STP_BRIDGE* bridge, unsigned int portIndex);

unsigned int STP_GetPortEnabled             (const struct STP_BRIDGE* bridge, unsigned int portIndex);
enum STP_PORT_ROLE STP_GetPortRole          (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex);
unsigned int STP_GetPortLearning            (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex);
unsigned int STP_GetPortForwarding          (const struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int treeIndex);
unsigned int STP_GetPortOperEdge            (const struct STP_BRIDGE* bridge, unsigned int portIndex);
unsigned int STP_GetPortOperPointToPointMAC (const struct STP_BRIDGE* bridge, unsigned int portIndex);

void STP_GetDefaultMstConfigName (const unsigned char bridgeAddress[6], char nameOut[18]);
void STP_SetMstConfigName (struct STP_BRIDGE* bridge, const char* name, unsigned int timestamp);
void STP_SetMstConfigRevisionLevel (struct STP_BRIDGE* bridge, unsigned short revisionLevel, unsigned int debugTimestamp);

struct STP_CONFIG_TABLE_ENTRY
{
	unsigned char unused;
	unsigned char treeIndex; // 0=CIST, 1=MSTI1, 2=MSTI2...
};

void STP_SetMstConfigTable (struct STP_BRIDGE* bridge, const struct STP_CONFIG_TABLE_ENTRY* entries, unsigned int entryCount, unsigned int timestamp);
const struct STP_CONFIG_TABLE_ENTRY* STP_GetMstConfigTable (struct STP_BRIDGE* bridge, unsigned int* entryCountOut);
unsigned int STP_GetMaxVlanNumber (const struct STP_BRIDGE* bridge);
unsigned int STP_GetTreeIndexFromVlanNumber (const struct STP_BRIDGE* bridge, unsigned int vlanNumber);
const struct STP_MST_CONFIG_ID* STP_GetMstConfigId (const struct STP_BRIDGE* bridge);

const char* STP_GetPortRoleString (enum STP_PORT_ROLE portRole);
const char* STP_GetVersionString (enum STP_VERSION version);
enum STP_VERSION STP_GetVersionFromString (const char* str);
const char* STP_GetAdminP2PString (enum STP_ADMIN_P2P adminP2P);

void STP_GetRootPriorityVector (const struct STP_BRIDGE* bridge, unsigned int treeIndex, unsigned char priorityVectorOut[36]);
void STP_GetRootTimes (const struct STP_BRIDGE* bridge,
					   unsigned int treeIndex,
					   unsigned short* forwardDelayOutOrNull,
					   unsigned short* helloTimeOutOrNull,
					   unsigned short* maxAgeOutOrNull,
					   unsigned short* messageAgeOutOrNull,
					   unsigned char* remainingHopsOutOrNull);

unsigned int STP_IsCistRoot (const struct STP_BRIDGE* bridge);
unsigned int STP_IsRegionalRoot (const struct STP_BRIDGE* bridge, unsigned int treeIndex);

// dot1dStpPortAdminPathCost
// ieee8021SpanningTreeRstpPortAdminPathCost
// ieee8021MstpCistPortAdminPathCost
void STP_SetAdminPortPathCost (struct STP_BRIDGE* bridge, unsigned int portIndex, unsigned int adminPathCost, unsigned int debugTimestamp);
unsigned int STP_GetAdminPortPathCost (const struct STP_BRIDGE* bridge, unsigned int portIndex);

// dot1dStpPortPathCost
// ieee8021SpanningTreePortPathCost
// ieee8021MstpCistPortCistPathCost
unsigned int STP_GetPortPathCost (const struct STP_BRIDGE* bridge, unsigned int portIndex);

void  STP_SetApplicationContext (struct STP_BRIDGE* bridge, void* applicationContext);
void* STP_GetApplicationContext (const struct STP_BRIDGE* bridge);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

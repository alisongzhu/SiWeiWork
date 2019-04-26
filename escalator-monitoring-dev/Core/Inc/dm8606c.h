#ifndef __DM8606C_H
#define __DM8606C_H

#define DM8606C_SUPERVISE         0

/* Initialization and de-initialization functions  ****************************/

void DM8606C_Init(void);

void DM8606C_Set_PHY_Down(void);
void DM8606C_Set_PHY_Up(void);

void DM8606C_Watchdog(void);
#if DM8606C_SUPERVISE
void DM8606C_Supervisory(void);
#endif


#endif /* __DM8606C_H */

/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#include <Arduino.h>

#define RW_MAX_NDEF_FILE_SIZE 500

extern unsigned char NdefBuffer[RW_MAX_NDEF_FILE_SIZE];

typedef void RW_NDEF_PushCallback_t (unsigned char*, unsigned short);
typedef void RW_NDEF_PullCallback_t (unsigned char*, unsigned short, unsigned int);

#define RW_NDEF_TYPE_T1T    0x1
#define RW_NDEF_TYPE_T2T    0x2
#define RW_NDEF_TYPE_T3T    0x3
#define RW_NDEF_TYPE_T4T    0x4
#define RW_NDEF_TYPE_MIFARE 0x80

extern unsigned char *pRW_NdefMessage;
extern unsigned short RW_NdefMessage_size;

extern RW_NDEF_PullCallback_t *pRW_NDEF_PullCb;
extern RW_NDEF_PushCallback_t *pRW_NDEF_PushCb;

void RW_NDEF_RegisterPullCallback(RW_NDEF_PullCallback_t *pCb);
bool RW_NDEF_SetMessage(unsigned char *pMessage, unsigned short Message_size, RW_NDEF_PushCallback_t *pCb);
void RW_NDEF_Reset(unsigned char type);
void RW_NDEF_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
void RW_NDEF_Write_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

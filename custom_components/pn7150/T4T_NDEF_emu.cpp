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

//#ifdef CARDEMU_SUPPORT
//#ifndef NO_NDEF_SUPPORT
#include "tool.h"
#include "T4T_NDEF_emu.h"

const unsigned char T4T_NDEF_EMU_APP_Select[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 0x00};
const unsigned char T4T_NDEF_EMU_CC[] = {0x00, 0x0F, 0x20, 0x00, 0xFF, 0x00, 0xFF, 0x04, 0x06, 0xE1, 0x04, 0x00, 0xFF, 0x00, 0x00};
const unsigned char T4T_NDEF_EMU_CC_Select[] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xE1, 0x03};
const unsigned char T4T_NDEF_EMU_NDEF_Select[] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xE1, 0x04};
const unsigned char T4T_NDEF_EMU_Read[] = {0x00, 0xB0};
const unsigned char T4T_NDEF_EMU_Write[] = {0x00, 0xD6};
const unsigned char T4T_NDEF_EMU_OK[] = {0x90, 0x00};
const unsigned char T4T_NDEF_EMU_NOK[] = {0x6A, 0x82};

unsigned char *pT4T_NdefMessage;
unsigned short T4T_NdefMessage_size = 0;

unsigned char T4T_NdefMessageWritten[256];

typedef enum
{
    Ready,
    NDEF_Application_Selected,
    CC_Selected,
    NDEF_Selected,
    DESFire_prod
} T4T_NDEF_EMU_state_t;



static T4T_NDEF_EMU_state_t eT4T_NDEF_EMU_State = Ready;

static T4T_NDEF_EMU_PushCallback_t *pT4T_NDEF_EMU_PushCb = NULL;
static T4T_NDEF_EMU_PullCallback_t *pT4T_NDEF_EMU_PullCb = NULL;

static void T4T_NDEF_EMU_FillRsp(unsigned char *pRsp, unsigned short offset, unsigned char length)
{
    if (offset == 0)
    {
        pRsp[0] = (T4T_NdefMessage_size & 0xFF00) >> 8;
        pRsp[1] = (T4T_NdefMessage_size & 0x00FF);
        if (length > 2)
            memcpy(&pRsp[2], &pT4T_NdefMessage[0], length - 2);
    }
    else if (offset == 1)
    {
        pRsp[0] = (T4T_NdefMessage_size & 0x00FF);
        if (length > 1)
            memcpy(&pRsp[1], &pT4T_NdefMessage[0], length - 1);
    }
    else
    {
        memcpy(pRsp, &pT4T_NdefMessage[offset - 2], length);
    }

    /* Did we reached the end of NDEF message ?*/
    if ((offset + length) >= (T4T_NdefMessage_size + 2))
    {
        /* Notify application of the NDEF send */
        if (pT4T_NDEF_EMU_PushCb != NULL)
            pT4T_NDEF_EMU_PushCb(pT4T_NdefMessage, T4T_NdefMessage_size);
    }
}

void PrintBuf1(const byte * data, const uint32_t numBytes)
{ //Print hex data buffer in format
  uint32_t szPos;
  String print_string;
  for (szPos=0; szPos < numBytes; szPos++)
  {
    Serial.print(F("0x"));
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      Serial.print(F("0"));
      Serial.print(data[szPos]&0xff, HEX);
      print_string += (char)data[szPos];
    if ((numBytes > 1) && (szPos != numBytes - 1))
    {
      Serial.print(F(" "));
    }
  }
  Serial.println();
  Serial.println(print_string);
}

bool T4T_NDEF_EMU_SetMessage(unsigned char *pMessage, unsigned short Message_size, T4T_NDEF_EMU_PushCallback_t *pCb)
{
    pT4T_NdefMessage = pMessage;
    T4T_NdefMessage_size = Message_size;
    pT4T_NDEF_EMU_PushCb = (T4T_NDEF_EMU_PushCallback_t *)pCb;

    return true;
}

bool T4T_NDEF_EMU_PullCallback(T4T_NDEF_EMU_PullCallback_t *pCb)
{
    pT4T_NDEF_EMU_PullCb = (T4T_NDEF_EMU_PullCallback_t *)pCb;
    return true;
}

void T4T_NDEF_EMU_Reset(void)
{
    eT4T_NDEF_EMU_State = Ready;
}

void T4T_NDEF_EMU_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *pRsp, unsigned short *pRsp_size)
{
    Serial.println("in Emu Next");
    bool eStatus = false;

    if (!memcmp(pCmd, T4T_NDEF_EMU_APP_Select, sizeof(T4T_NDEF_EMU_APP_Select)))
    {
        Serial.println("in Emu APP");
        *pRsp_size = 0;
        eStatus = true;
        eT4T_NDEF_EMU_State = NDEF_Application_Selected;
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_CC_Select, sizeof(T4T_NDEF_EMU_CC_Select)))
    {
        Serial.println("in Emu CC");
        if (eT4T_NDEF_EMU_State == NDEF_Application_Selected)
        {
            *pRsp_size = 0;
            eStatus = true;
            eT4T_NDEF_EMU_State = CC_Selected;
        }
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_NDEF_Select, sizeof(T4T_NDEF_EMU_NDEF_Select)))
    {
        Serial.println("in Emu NDEF Sel.");
        *pRsp_size = 0;
        eStatus = true;
        eT4T_NDEF_EMU_State = NDEF_Selected;
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_Read, sizeof(T4T_NDEF_EMU_Read)))
    {
        Serial.println("in Emu Read to read data from card");
        if (eT4T_NDEF_EMU_State == CC_Selected)
        {
            unsigned short offset = (pCmd[2] << 8) + pCmd[3];
            unsigned char length = pCmd[4];
            Serial.println("CC_Selected");
            if (length <= (sizeof(T4T_NDEF_EMU_CC) + offset + 2))
            {
                memcpy(pRsp, &T4T_NDEF_EMU_CC[offset], length);
                *pRsp_size = length;
                eStatus = true;
            }
        }
        else if (eT4T_NDEF_EMU_State == NDEF_Selected)
        {
            unsigned short offset = (pCmd[2] << 8) + pCmd[3];
            unsigned char length = pCmd[4];
            Serial.println("NDEF_Selected");
            if (length <= (T4T_NdefMessage_size + offset + 2))
            {
                T4T_NDEF_EMU_FillRsp(pRsp, offset, length);
                *pRsp_size = length;
                eStatus = true;
            }
        }else{
            Serial.println("in EMU Read Else");
        }
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_Write, sizeof(T4T_NDEF_EMU_Write)))
    {
        Serial.println("in Emu Write to write data to card"); 
        if (eT4T_NDEF_EMU_State == NDEF_Selected)
        {
            Serial.println("NDEF_Selected"); 
            unsigned short offset = (pCmd[2] << 8) + pCmd[3];
            unsigned char length = pCmd[4];
            if (offset + length <= sizeof(T4T_NdefMessageWritten))
            {
                Serial.println("NDEF_Selected in if"); 
                memcpy(&T4T_NdefMessageWritten[offset - 2], &pCmd[5], length);
                pT4T_NdefMessage = T4T_NdefMessageWritten;
                T4T_NdefMessage_size = (pCmd[5] << 8) + pCmd[6];
                if( length > 0x0A )
                {
                    pT4T_NDEF_EMU_PullCb(pT4T_NdefMessage, T4T_NdefMessage_size, T4T_NdefMessage_size);
                }
                //
                *pRsp_size = 0;
                eStatus = true;
            }else{
                Serial.println("NDEF_Selected in else"); 
            }
        }
    }

    if (eStatus == true)
    {
        Serial.println("in Emu OK");
        memcpy(&pRsp[*pRsp_size], T4T_NDEF_EMU_OK, sizeof(T4T_NDEF_EMU_OK));
        *pRsp_size += sizeof(T4T_NDEF_EMU_OK);
    }
    else
    {
        Serial.println("in Emu NOK");
        memcpy(pRsp, T4T_NDEF_EMU_NOK, sizeof(T4T_NDEF_EMU_NOK));
        *pRsp_size = sizeof(T4T_NDEF_EMU_NOK);
        T4T_NDEF_EMU_Reset();
    }
}
//#endif
//#endif

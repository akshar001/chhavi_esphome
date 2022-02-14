#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include <memory>
#include "PN7150.h"

#define PN7150_IRQ   (23)
#define PN7150_VEN   (19)
#define PN7150_ADDR  (0x28)

Electroniccats_PN7150 mynfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR);
RfIntf_t RfInterface;

int mode = 1; 

namespace esphome {
namespace pn7150 {

static const char *TAG = "pn7150.component";

void PN7150::setup() 
{
	ESP_LOGI(TAG,"Detect nfc Tag with pn7150");
	ESP_LOGCONFIG(TAG, "Setting up PN7150...");

	if(mynfc.connectNCI())
	{
		ESP_LOGI(TAG,"Error while setting up the mode, check connections!");
		while(1);
	}

	if(mynfc.ConfigureSettings())
	{
		ESP_LOGI(TAG,"The Configure Settings is failed!");
		while(1);
	}

	if(mynfc.ConfigMode(mode))
	{
		ESP_LOGI(TAG,"The Configure Mode is failed");
	}

	mynfc.StartDiscovery(mode);
	ESP_LOGI(TAG,"Waiting for Card .....................");
}
  
void PN7150::loop() 
{

  while(!mynfc.WaitForDiscoveryNotification(&RfInterface,1))
  { // Waiting to detect cards
    displayCardInfo(RfInterface);
    switch(RfInterface.Protocol) {
      case PROT_T1T:
      case PROT_T2T:
      case PROT_T3T:
      case PROT_ISODEP:
          mynfc.ProcessReaderMode(RfInterface, READ_NDEF);
          break;
      
      case PROT_ISO15693:
          break;
      
      case PROT_MIFARE:
          mynfc.ProcessReaderMode(RfInterface, READ_NDEF);
          break;
      
      default:
          break;
    }

    mynfc.StopDiscovery();
    mynfc.StartDiscovery(mode);
  }

  delay(500);
}


void PN7150::PrintBuf(const byte * data, const uint32_t numBytes)
{ //Print hex data buffer in format
  uint32_t szPos;
  for (szPos=0; szPos < numBytes; szPos++)
  {
    Serial.print(F("0x"));
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      Serial.print(F("0"));
    Serial.print(data[szPos]&0xff, HEX);
    if ((numBytes > 1) && (szPos != numBytes - 1))
    {
      Serial.print(F(" "));
    }
  }
  Serial.println();
}


void PN7150:: displayCardInfo(RfIntf_t RfIntf)
{
    ESP_LOGI(TAG,"displayCardInfo");
	char tmp[16];
	while (1)
    {
        switch(RfIntf.Protocol)
        {  //Indetify card protocol
            case PROT_T1T:
            case PROT_T2T:
            case PROT_T3T:
            case PROT_ISODEP:
                Serial.print(" - POLL MODE: Remote activated tag type: ");
                Serial.println(RfIntf.Protocol);
                break;
            case PROT_ISO15693:
                Serial.println(" - POLL MODE: Remote ISO15693 card activated");
                break;
            case PROT_MIFARE:
                Serial.println(" - POLL MODE: Remote MIFARE card activated");
                break;
            default:
                Serial.println(" - POLL MODE: Undetermined target");
                    return;
        }

        switch(RfIntf.ModeTech) 
        { //Indetify card technology
            case (MODE_POLL | TECH_PASSIVE_NFCA):
                ESP_LOGI(TAG,"TECH_PASSIVE_NFCA");
                Serial.print("\tSENS_RES = ");
                sprintf(tmp, "0x%.2X",RfIntf.Info.NFC_APP.SensRes[0]);
                Serial.print(tmp); Serial.print(" ");
                sprintf(tmp, "0x%.2X",RfIntf.Info.NFC_APP.SensRes[1]);
                Serial.print(tmp); Serial.println(" ");
                    
                Serial.print("\tNFCID = ");
                PrintBuf(RfIntf.Info.NFC_APP.NfcId, RfIntf.Info.NFC_APP.NfcIdLen);
                copy(&RfIntf.Info.NFC_APP.NfcId[0],&RfIntf.Info.NFC_APP.NfcId[RfIntf.Info.NFC_APP.NfcIdLen],back_inserter(uid));
                memcpy(&id,RfIntf.Info.NFC_APP.NfcId,RfIntf.Info.NFC_APP.NfcIdLen);
                    
                if(RfIntf.Info.NFC_APP.SelResLen != 0) 
                {
                    Serial.print("\tSEL_RES = ");
                    sprintf(tmp, "0x%.2X",RfIntf.Info.NFC_APP.SelRes[0]);
                    Serial.print(tmp); Serial.println(" ");
            
                }
                break;
            
            case (MODE_POLL | TECH_PASSIVE_NFCB):
                if(RfIntf.Info.NFC_BPP.SensResLen != 0) 
                {
                    Serial.print("\tSENS_RES = ");
                    PrintBuf(RfIntf.Info.NFC_BPP.SensRes,RfIntf.Info.NFC_BPP.SensResLen);
                }
                break;
            
            case (MODE_POLL | TECH_PASSIVE_NFCF):
                Serial.print("\tBitrate = ");
                Serial.println((RfIntf.Info.NFC_FPP.BitRate == 1) ? "212" : "424");
                    
                if(RfIntf.Info.NFC_FPP.SensResLen != 0) 
                {
                    Serial.print("\tSENS_RES = ");
                    PrintBuf(RfIntf.Info.NFC_FPP.SensRes,RfIntf.Info.NFC_FPP.SensResLen);
                }
                break;
            
            case (MODE_POLL | TECH_PASSIVE_15693):
                Serial.print("\tID = ");
                PrintBuf(RfIntf.Info.NFC_VPP.ID,sizeof(RfIntf.Info.NFC_VPP.ID));
                    
                Serial.print("\ntAFI = ");
                Serial.println(RfIntf.Info.NFC_VPP.AFI);
                    
                Serial.print("\tDSFID = ");
                Serial.println(RfIntf.Info.NFC_VPP.DSFID,HEX);
                break;
            
                default:
                    break;
        }

        for (auto *bin_sens : this->binary_sensors_) 
        {
            if (bin_sens->process(uid)) 
            {
                report = false;
            }
            uid.clear();
        }    

        if(report == false)
        {
            report = true;
            for(auto *trig : this->triggers_ontag_)
                trig->process(id);
        }
        else
        {
            ESP_LOGI(TAG,"New tag found");
            PrintBuf((const byte *)&id,RfIntf.Info.NFC_APP.NfcIdLen);
        }

        
        if(RfIntf.MoreTags) 
        { // It will try to identify more NFC cards if they are the same technology
            if(mynfc.ReaderActivateNext(&RfIntf) == NFC_ERROR) break;
        }
        else break;
	}

}

void PN7150::dump_config() 
{

    ESP_LOGCONFIG(TAG, "Custom binary sensor");
}

float PN7150::get_setup_priority() const { return setup_priority::DATA; }

bool PN7150Binary :: process(std::vector<uint32_t> &data)
{
    //ESP_LOGI(TAG,"size data %d",data.size());
    //ESP_LOGI(TAG,"size uid_ %d",uid_.size());
    if (data.size() != this->uid_.size())
        return false;

    for (size_t i = 0; i < data.size(); i++) 
    {
       // ESP_LOGI(TAG,"%x",data[i]);
        //ESP_LOGI(TAG,"%x",uid_[i]);
        if (data[i] != this->uid_[i])
          return false;
    }

    this->publish_state(true);
    this->found_ = true;
    return true;
}

} //namespace empty_binary_sensor
} //namespace esphome
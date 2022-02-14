#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "Electroniccats_PN7150.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/nfc/nfc_tag.h"
#include "esphome/components/nfc/nfc.h"

namespace esphome 
{
    namespace pn7150 
    {

class PN7150Binary;
class PN7150OnTagTrigger;

        class PN7150 : public Component
        {
        public:
            //EmptyComponent() : PollingComponent(5000) {}

            void setup() override;
            void dump_config() override;
            void loop() override;
            void PrintBuf(const byte * data, const uint32_t numBytes);
            float get_setup_priority() const override;
            void displayCardInfo(RfIntf_t RfIntf);
            void register_tag(PN7150Binary *tag) { this->binary_sensors_.push_back(tag); }
            void register_ontag_trigger(PN7150OnTagTrigger *trig) { this->triggers_ontag_.push_back(trig); }
            void register_ontagremoved_trigger(PN7150OnTagTrigger *trig) { this->triggers_ontagremoved_.push_back(trig); }

            std::vector<PN7150Binary *> binary_sensors_;
            std::vector<PN7150OnTagTrigger *> triggers_ontag_;
            std::vector<PN7150OnTagTrigger *> triggers_ontagremoved_;
            bool report = true;
            std::vector<uint32_t> uid;
            uint32_t id;
        };

        class PN7150Binary : public binary_sensor::BinarySensor 
        {
            
            public:
                void set_uid(const std::vector<uint8_t> &uid) { uid_ = uid; }
                bool process(std::vector<uint32_t> &data);

            protected:
                std::vector<uint8_t> uid_;
                bool found_{false};
        };


        class PN7150OnTagTrigger : public Trigger<uint32_t> 
        {
         
            public:
                //void process(uint32_t id);
                void process(uint32_t id) { this->trigger(id);}
        };

    } //namespace empty_binary_sensor
} //namespace esphome
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import nfc, binary_sensor
from esphome.const import CONF_ID, CONF_ON_TAG, CONF_TRIGGER_ID,CONF_ON_TAG_REMOVED

AUTO_LOAD = ["binary_sensor", "nfc"]

pn7150_ns = cg.esphome_ns.namespace("pn7150")
PN7150 = pn7150_ns.class_("PN7150", cg.Component)

PN7150OnTagTrigger = pn7150_ns.class_(
    "PN7150OnTagTrigger", automation.Trigger.template(cg.uint32)
)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PN7150),
        cv.Optional(CONF_ON_TAG): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PN7150OnTagTrigger),
            }
        ),
        cv.Optional(CONF_ON_TAG_REMOVED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PN7150OnTagTrigger),
            }
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_ON_TAG, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
        cg.add(var.register_ontag_trigger(trigger))
        await automation.build_automation(
            trigger, [(cg.uint32, "x")], conf
        )

    for conf in config.get(CONF_ON_TAG_REMOVED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
        cg.add(var.register_ontagremoved_trigger(trigger))
        await automation.build_automation(
            trigger, [(cg.uint32, "x")], conf
        )
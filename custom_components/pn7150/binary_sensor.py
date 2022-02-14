import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, pn7150
from esphome.const import CONF_ID, CONF_UID
from esphome.core import HexInt
from . import pn7150_ns

DEPENDENCIES = ["pn7150"]

CONF_PN7150_ID = "pn7150_id"

def validate_uid(value):
    value = cv.string_strict(value)
    for x in value.split("-"):
        if len(x) != 2:
            raise cv.Invalid(
                "Each part (separated by '-') of the UID must be two characters "
                "long."
            )
        try:
            x = int(x, 16)
        except ValueError as err:
            raise cv.Invalid(
                "Valid characters for parts of a UID are 0123456789ABCDEF."
            ) from err
        if x < 0 or x > 255:
            raise cv.Invalid(
                "Valid values for UID parts (separated by '-') are 00 to FF"
            )
    return value



PN7150Binary = pn7150_ns.class_('PN7150Binary',binary_sensor.BinarySensor)

CONFIG_SCHEMA = binary_sensor.BINARY_SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(PN7150Binary),
        cv.GenerateID(CONF_PN7150_ID): cv.use_id(pn7150.PN7150),
        cv.Required(CONF_UID): validate_uid,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)

    hub = await cg.get_variable(config[CONF_PN7150_ID])
    cg.add(hub.register_tag(var))
    addr = [HexInt(int(x, 16)) for x in config[CONF_UID].split("-")]
    cg.add(var.set_uid(addr))
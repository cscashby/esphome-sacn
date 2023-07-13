import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ID
from esphome.core import coroutine_with_priority

sacn_component_ns = cg.esphome_ns.namespace('sacn_component')
sACNComponent = sacn_component_ns.class_('sACNComponent', cg.Component)

CONF_UNIVERSE = 'universe'
CONF_START_ADDRESS = 'start_address'
CONF_RGB_ID = 'rgb_id'
CONF_RGB_NUM_LEDS = 'rgb_num_leds'
CONF_MOTOR_ID = 'motor_id'
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(sACNComponent),
    cv.Required(CONF_UNIVERSE): cv.int_range(min=1,max=63999),
    cv.Required(CONF_START_ADDRESS): cv.int_range(min=1,max=512),
    cv.Required(CONF_RGB_ID): cv.use_id(light.AddressableLightState),
    cv.Required(CONF_RGB_NUM_LEDS): cv.int_range(min=1,max=512),
    cv.Required(CONF_MOTOR_ID): cv.use_id(light.LightState),
}).extend(cv.COMPONENT_SCHEMA)

@coroutine_with_priority(100.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_universe(config[CONF_UNIVERSE]))
    cg.add(var.set_start_address(config[CONF_START_ADDRESS]))
    var_ = await cg.get_variable(config[CONF_RGB_ID])
    cg.add(var.set_rgb_light(var_))
    cg.add(var.set_rgb_num_leds(config[CONF_RGB_NUM_LEDS]))
    var_ = await cg.get_variable(config[CONF_MOTOR_ID])
    cg.add(var.set_motor(var_))

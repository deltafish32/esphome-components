import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan
from esphome.const import (
    CONF_ID, 
    CONF_OUTPUT_ID, 
)
from esphome import pins
from .. import vornado_660_ns


AUTO_LOAD = ["fan"]


Vornado_660 = vornado_660_ns.class_('Vornado_660', cg.Component, fan.Fan)

CONF_POWER_PIN = 'power_pin'
CONF_LL_PIN = 'll_pin'
CONF_L_PIN = 'l_pin'
CONF_H_PIN = 'h_pin'
CONF_HH_PIN = 'hh_pin'
CONF_LL_FB_PIN = 'll_fb_pin'
CONF_L_FB_PIN = 'l_fb_pin'
CONF_H_FB_PIN = 'h_fb_pin'
CONF_HH_FB_PIN = 'hh_fb_pin'
# CONF_RELAY_PIN = 'relay_pin'


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(Vornado_660),
            cv.Required(CONF_POWER_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_LL_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_L_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_H_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_HH_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_LL_FB_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_L_FB_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_H_FB_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_HH_FB_PIN): pins.gpio_output_pin_schema,
            # cv.Optional(CONF_RELAY_PIN): pins.gpio_output_pin_schema,
        }

    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(fan.FAN_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await cg.register_component(var, config)
    await fan.register_fan(var, config)

    power_pin = await cg.gpio_pin_expression(config[CONF_POWER_PIN])
    cg.add(var.set_power_pin(power_pin))
    
    ll_pin = await cg.gpio_pin_expression(config[CONF_LL_PIN])
    cg.add(var.set_ll_pin(ll_pin))
    l_pin = await cg.gpio_pin_expression(config[CONF_L_PIN])
    cg.add(var.set_l_pin(l_pin))
    h_pin = await cg.gpio_pin_expression(config[CONF_H_PIN])
    cg.add(var.set_h_pin(h_pin))
    hh_pin = await cg.gpio_pin_expression(config[CONF_HH_PIN])
    cg.add(var.set_hh_pin(hh_pin))

    ll_fb_pin = await cg.gpio_pin_expression(config[CONF_LL_FB_PIN])
    cg.add(var.set_ll_fb_pin(ll_fb_pin))
    l_fb_pin = await cg.gpio_pin_expression(config[CONF_L_FB_PIN])
    cg.add(var.set_l_fb_pin(l_fb_pin))
    h_fb_pin = await cg.gpio_pin_expression(config[CONF_H_FB_PIN])
    cg.add(var.set_h_fb_pin(h_fb_pin))
    hh_fb_pin = await cg.gpio_pin_expression(config[CONF_HH_FB_PIN])
    cg.add(var.set_hh_fb_pin(hh_fb_pin))

    # if CONF_RELAY_PIN in config:
    #     relay_pin = await cg.gpio_pin_expression(config[CONF_RELAY_PIN])
    #     cg.add(var.set_relay_pin(relay_pin))

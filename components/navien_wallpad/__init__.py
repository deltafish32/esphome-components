import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, button, climate, light, sensor, switch, uart
from esphome.const import (
    CONF_ID, 
    CONF_ICON, 
    CONF_UART_ID, 
    CONF_OUTPUT_ID, 
    CONF_DEVICE_CLASS, 
    CONF_UPDATE_INTERVAL, 
    DEVICE_CLASS_SAFETY, 
    DEVICE_CLASS_GAS, 
    DEVICE_CLASS_POWER, 
    DEVICE_CLASS_ENERGY, 
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING, 
    UNIT_WATT, 
    UNIT_KILOWATT_HOURS, 
)


DEPENDENCIES = ["uart"]
AUTO_LOAD = ["button", "climate", "sensor", "binary_sensor", "light", "switch"]

validate_device_class = cv.one_of(*binary_sensor.DEVICE_CLASSES, lower=True, space="_")

CONF_VALVE_SENSOR = 'valve_sensor'
CONF_VALVE_CLOSE_BUTTON = 'valve_close_button'
CONF_LIGHTS = 'lights'
CONF_CLIMATES = 'climates'
CONF_SUB_ID = 'sub_id'
CONF_MEASURES = 'measures'
CONF_WATER_CURRENT = 'water_current'
CONF_WATER_TOTAL   = 'water_total'
CONF_GAS_CURRENT   = 'gas_current'
CONF_GAS_TOTAL     = 'gas_total'
CONF_POWER_CURRENT = 'power_current'
CONF_POWER_TOTAL   = 'power_total'
CONF_BREAKER = 'breaker'
CONF_DETAILED_LOG = 'detailed_log'
CONF_WALLPAD_TEST = 'wallpad_test'
CONF_SUPPORT_TEMPERATURE_0_5 = 'support_temperature_0_5'

UNIT_M3_H = 'm³/h'
UNIT_M3 = 'm³'

ICON_FIRE = 'mdi:fire'
ICON_WATER = 'mdi:water'
ICON_VALVE_CLOSED = 'mdi:valve-closed'
ICON_ELECTRIC_SWITCH = 'mdi:electric-switch'
ICON_NOTE_EDIT = 'mdi:note-edit'
ICON_CODE_BRACES = 'mdi:code-braces'

navien_wallpad_ns = cg.esphome_ns.namespace('navien_wallpad')
Navien_Wallpad = navien_wallpad_ns.class_('Navien_Wallpad', uart.UARTDevice, cg.Component)

WallpadLightOutput = navien_wallpad_ns.class_('WallpadLightOutput', light.LightOutput, cg.Component)
WallpadClimate = navien_wallpad_ns.class_('WallpadClimate', climate.Climate, cg.Component)
WallpadValveCloseButton = navien_wallpad_ns.class_('WallpadValveCloseButton', button.Button, cg.Component)
WallpadMeasure = navien_wallpad_ns.class_('WallpadMeasure', sensor.Sensor, cg.PollingComponent)
WallpadWaterCurrent = navien_wallpad_ns.class_('WallpadWaterCurrent', WallpadMeasure)
WallpadWaterTotal   = navien_wallpad_ns.class_('WallpadWaterTotal',   WallpadMeasure)
WallpadGasCurrent   = navien_wallpad_ns.class_('WallpadGasCurrent',   WallpadMeasure)
WallpadGasTotal     = navien_wallpad_ns.class_('WallpadGasTotal',     WallpadMeasure)
WallpadPowerCurrent = navien_wallpad_ns.class_('WallpadPowerCurrent', WallpadMeasure)
WallpadPowerTotal   = navien_wallpad_ns.class_('WallpadPowerTotal',   WallpadMeasure)
WallpadBreakLightsSwitch = navien_wallpad_ns.class_('WallpadBreakLightsSwitch', switch.Switch, cg.Component)
WallpadLogSwitch = navien_wallpad_ns.class_('WallpadLogSwitch', switch.Switch, cg.Component)
WallpadTestButton = navien_wallpad_ns.class_('WallpadTestButton', button.Button, cg.Component)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Navien_Wallpad),
            cv.Optional(CONF_VALVE_SENSOR): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
                {
                    cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_GAS): validate_device_class,
                }
            ),
            cv.Optional(CONF_VALVE_CLOSE_BUTTON): button.button_schema(
                icon=ICON_VALVE_CLOSED,
            ).extend(
                {
                    cv.GenerateID(): cv.declare_id(WallpadValveCloseButton),
                }
            ),
            cv.Optional(CONF_LIGHTS): cv.ensure_list(light.BINARY_LIGHT_SCHEMA.extend(cv.Schema(
                {
                    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(WallpadLightOutput),
                    cv.Required(CONF_SUB_ID): cv.int_range(min=0x11),
                }
            ))),
            cv.Optional(CONF_SUPPORT_TEMPERATURE_0_5, default=False): cv.boolean,
            cv.Optional(CONF_CLIMATES): cv.ensure_list(climate.CLIMATE_SCHEMA.extend(cv.Schema(
                {
                    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(WallpadClimate),
                    cv.Required(CONF_SUB_ID): cv.int_range(min=0x11),
                }
            ))),
            cv.Optional(CONF_MEASURES): cv.Schema(
                {
                    cv.Optional(CONF_WATER_CURRENT): sensor.sensor_schema(
                        unit_of_measurement=UNIT_M3_H,
                        accuracy_decimals=3,
                        state_class=STATE_CLASS_MEASUREMENT,
                        icon=ICON_WATER,
                    )
                    .extend(cv.polling_component_schema("60s"))
                    .extend(
                        {
                            cv.GenerateID(): cv.declare_id(WallpadWaterCurrent),
                        }
                    ),
                    cv.Optional(CONF_WATER_TOTAL): sensor.sensor_schema(
                        unit_of_measurement=UNIT_M3,
                        accuracy_decimals=2,
                        state_class=STATE_CLASS_TOTAL_INCREASING,
                        icon=ICON_WATER,
                    )
                    .extend(cv.polling_component_schema("60s"))
                    .extend(
                        {
                            cv.GenerateID(): cv.declare_id(WallpadWaterTotal),
                        }
                    ),
                    cv.Optional(CONF_GAS_CURRENT): sensor.sensor_schema(
                        unit_of_measurement=UNIT_M3_H,
                        accuracy_decimals=3,
                        device_class=DEVICE_CLASS_GAS,
                        state_class=STATE_CLASS_MEASUREMENT,
                        icon=ICON_FIRE,
                    )
                    .extend(cv.polling_component_schema("60s"))
                    .extend(
                        {
                            cv.GenerateID(): cv.declare_id(WallpadGasCurrent),
                        }
                    ),
                    cv.Optional(CONF_GAS_TOTAL): sensor.sensor_schema(
                        unit_of_measurement=UNIT_M3,
                        accuracy_decimals=2,
                        device_class=DEVICE_CLASS_GAS,
                        state_class=STATE_CLASS_TOTAL_INCREASING,
                        icon=ICON_FIRE,
                    )
                    .extend(cv.polling_component_schema("60s"))
                    .extend(
                        {
                            cv.GenerateID(): cv.declare_id(WallpadGasTotal),
                        }
                    ),
                    cv.Optional(CONF_POWER_CURRENT): sensor.sensor_schema(
                        unit_of_measurement=UNIT_WATT,
                        accuracy_decimals=0,
                        device_class=DEVICE_CLASS_POWER,
                        state_class=STATE_CLASS_MEASUREMENT,
                    )
                    .extend(cv.polling_component_schema("60s"))
                    .extend(
                        {
                            cv.GenerateID(): cv.declare_id(WallpadPowerCurrent),
                        }
                    ),
                    cv.Optional(CONF_POWER_TOTAL): sensor.sensor_schema(
                        unit_of_measurement=UNIT_KILOWATT_HOURS,
                        accuracy_decimals=1,
                        device_class=DEVICE_CLASS_ENERGY,
                        state_class=STATE_CLASS_TOTAL_INCREASING,
                    )
                    .extend(cv.polling_component_schema("60s"))
                    .extend(
                        {
                            cv.GenerateID(): cv.declare_id(WallpadPowerTotal),
                        }
                    ),
                }
            ),
            cv.Optional(CONF_BREAKER): switch.SWITCH_SCHEMA.extend(
                {
                    cv.GenerateID(): cv.declare_id(WallpadBreakLightsSwitch),
                    cv.Optional(CONF_ICON, default=ICON_ELECTRIC_SWITCH): cv.icon,
                }
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_DETAILED_LOG): switch.SWITCH_SCHEMA.extend(
                {
                    cv.GenerateID(): cv.declare_id(WallpadLogSwitch),
                    cv.Optional(CONF_ICON, default=ICON_NOTE_EDIT): cv.icon,
                }
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_WALLPAD_TEST): button.button_schema(
                icon=ICON_CODE_BRACES,
            ).extend(
                {
                    cv.GenerateID(): cv.declare_id(WallpadTestButton),
                }
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    uart_component = await cg.get_variable(config[CONF_UART_ID])

    wp = cg.new_Pvariable(config[CONF_ID], uart_component)
    await cg.register_component(wp, config)

    if CONF_VALVE_SENSOR in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_VALVE_SENSOR])
        await binary_sensor.register_binary_sensor(sens, config[CONF_VALVE_SENSOR])
        cg.add(wp.register_valve_close_sensor(sens))

    if CONF_VALVE_CLOSE_BUTTON in config:
        conf = config[CONF_VALVE_CLOSE_BUTTON]
        var = cg.new_Pvariable(conf[CONF_ID])
        await button.register_button(var, conf)
        await cg.register_component(var, conf)

    if CONF_LIGHTS in config:
        for i, conf in enumerate(config[CONF_LIGHTS]):
            rhs = cg.new_Pvariable(conf[CONF_OUTPUT_ID])
            await light.register_light(rhs, conf)
            cg.add(rhs.set_sub_id(conf[CONF_SUB_ID]))
            cg.add(wp.register_light(rhs))

    cg.add(wp.set_support_temperature_0_5(config[CONF_SUPPORT_TEMPERATURE_0_5]))

    if CONF_CLIMATES in config:
        for i, conf in enumerate(config[CONF_CLIMATES]):
            rhs = cg.new_Pvariable(conf[CONF_OUTPUT_ID])
            await climate.register_climate(rhs, conf)
            cg.add(rhs.set_sub_id(conf[CONF_SUB_ID]))
            cg.add(wp.register_climate(rhs))

    if CONF_MEASURES in config:
        if CONF_WATER_CURRENT in config[CONF_MEASURES]:
            conf = config[CONF_MEASURES][CONF_WATER_CURRENT]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await sensor.register_sensor(var, conf)
        if CONF_WATER_TOTAL in config[CONF_MEASURES]:
            conf = config[CONF_MEASURES][CONF_WATER_TOTAL]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await sensor.register_sensor(var, conf)
        if CONF_GAS_CURRENT in config[CONF_MEASURES]:
            conf = config[CONF_MEASURES][CONF_GAS_CURRENT]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await sensor.register_sensor(var, conf)
        if CONF_GAS_TOTAL in config[CONF_MEASURES]:
            conf = config[CONF_MEASURES][CONF_GAS_TOTAL]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await sensor.register_sensor(var, conf)
        if CONF_POWER_CURRENT in config[CONF_MEASURES]:
            conf = config[CONF_MEASURES][CONF_POWER_CURRENT]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await sensor.register_sensor(var, conf)
        if CONF_POWER_TOTAL in config[CONF_MEASURES]:
            conf = config[CONF_MEASURES][CONF_POWER_TOTAL]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await sensor.register_sensor(var, conf)

    if CONF_BREAKER in config:
        conf = config[CONF_BREAKER]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await switch.register_switch(var, conf)
        cg.add(wp.register_breaker(var))

    if CONF_DETAILED_LOG in config:
        conf = config[CONF_DETAILED_LOG]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await switch.register_switch(var, conf)

    if CONF_WALLPAD_TEST in config:
        conf = config[CONF_WALLPAD_TEST]
        var = cg.new_Pvariable(conf[CONF_ID])
        await button.register_button(var, conf)
        await cg.register_component(var, conf)

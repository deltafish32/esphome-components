import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, button, climate, light, sensor, switch, uart
from esphome.const import (
    CONF_ID,
    CONF_UART_ID,
    CONF_OUTPUT_ID,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_GAS,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLUME_FLOW_RATE,
    DEVICE_CLASS_WATER,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CUBIC_METER,
    UNIT_CUBIC_METER_PER_HOUR,
    UNIT_KILOWATT_HOURS,
    UNIT_WATT,
)


DEPENDENCIES = ["uart"]
AUTO_LOAD = ["button", "climate", "sensor", "binary_sensor", "light", "switch"]

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
CONF_WALLPAD_SCAN = 'wallpad_scan'
CONF_SUPPORT_TEMPERATURE_0_5 = 'support_temperature_0_5'
CONF_WRITE_RETRY = 'write_retry'

ICON_FIRE = 'mdi:fire'
ICON_WATER = 'mdi:water'
ICON_VALVE_CLOSED = 'mdi:valve-closed'
ICON_ELECTRIC_SWITCH = 'mdi:electric-switch'
ICON_NOTE_EDIT = 'mdi:note-edit'
ICON_CODE_BRACES = 'mdi:code-braces'

MAX_GROUPS = 14
MAX_LIGHTS = 14
MAX_CLIMATES = 8

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
WallpadScanButton = navien_wallpad_ns.class_('WallpadScanButton', button.Button, cg.Component)


def validate_sub_id(max_each):
    def validator(value):
        value = cv.hex_uint8_t(value)
        group = (value & 0xF0) >> 4
        each = value & 0x0F
        if not 1 <= group <= MAX_GROUPS:
            raise cv.Invalid(f"sub_id 0x{value:02x}: group must be 1~{MAX_GROUPS} (0x1_ ~ 0x{MAX_GROUPS:x}_)")
        if not 1 <= each <= max_each:
            raise cv.Invalid(f"sub_id 0x{value:02x}: each id must be 1~{max_each} (0x_1 ~ 0x_{max_each:x})")
        return value

    return validator


def measure_schema(class_, **kwargs):
    return sensor.sensor_schema(class_, **kwargs).extend(cv.polling_component_schema("60s"))


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Navien_Wallpad),
            cv.Optional(CONF_VALVE_SENSOR): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_GAS,
            ),
            cv.Optional(CONF_VALVE_CLOSE_BUTTON): button.button_schema(
                WallpadValveCloseButton,
                icon=ICON_VALVE_CLOSED,
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_LIGHTS): cv.ensure_list(
                light.BINARY_LIGHT_SCHEMA.extend(
                    {
                        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(WallpadLightOutput),
                        cv.Required(CONF_SUB_ID): validate_sub_id(MAX_LIGHTS),
                    }
                ).extend(cv.COMPONENT_SCHEMA)
            ),
            cv.Optional(CONF_SUPPORT_TEMPERATURE_0_5, default=False): cv.boolean,
            cv.Optional(CONF_CLIMATES): cv.ensure_list(
                climate.climate_schema(WallpadClimate).extend(
                    {
                        cv.Required(CONF_SUB_ID): validate_sub_id(MAX_CLIMATES),
                    }
                ).extend(cv.COMPONENT_SCHEMA)
            ),
            cv.Optional(CONF_MEASURES): cv.Schema(
                {
                    cv.Optional(CONF_WATER_CURRENT): measure_schema(
                        WallpadWaterCurrent,
                        unit_of_measurement=UNIT_CUBIC_METER_PER_HOUR,
                        accuracy_decimals=3,
                        device_class=DEVICE_CLASS_VOLUME_FLOW_RATE,
                        state_class=STATE_CLASS_MEASUREMENT,
                        icon=ICON_WATER,
                    ),
                    cv.Optional(CONF_WATER_TOTAL): measure_schema(
                        WallpadWaterTotal,
                        unit_of_measurement=UNIT_CUBIC_METER,
                        accuracy_decimals=2,
                        device_class=DEVICE_CLASS_WATER,
                        state_class=STATE_CLASS_TOTAL_INCREASING,
                        icon=ICON_WATER,
                    ),
                    cv.Optional(CONF_GAS_CURRENT): measure_schema(
                        WallpadGasCurrent,
                        unit_of_measurement=UNIT_CUBIC_METER_PER_HOUR,
                        accuracy_decimals=3,
                        device_class=DEVICE_CLASS_VOLUME_FLOW_RATE,
                        state_class=STATE_CLASS_MEASUREMENT,
                        icon=ICON_FIRE,
                    ),
                    cv.Optional(CONF_GAS_TOTAL): measure_schema(
                        WallpadGasTotal,
                        unit_of_measurement=UNIT_CUBIC_METER,
                        accuracy_decimals=2,
                        device_class=DEVICE_CLASS_GAS,
                        state_class=STATE_CLASS_TOTAL_INCREASING,
                        icon=ICON_FIRE,
                    ),
                    cv.Optional(CONF_POWER_CURRENT): measure_schema(
                        WallpadPowerCurrent,
                        unit_of_measurement=UNIT_WATT,
                        accuracy_decimals=0,
                        device_class=DEVICE_CLASS_POWER,
                        state_class=STATE_CLASS_MEASUREMENT,
                    ),
                    cv.Optional(CONF_POWER_TOTAL): measure_schema(
                        WallpadPowerTotal,
                        unit_of_measurement=UNIT_KILOWATT_HOURS,
                        accuracy_decimals=1,
                        device_class=DEVICE_CLASS_ENERGY,
                        state_class=STATE_CLASS_TOTAL_INCREASING,
                    ),
                }
            ),
            cv.Optional(CONF_BREAKER): switch.switch_schema(
                WallpadBreakLightsSwitch,
                icon=ICON_ELECTRIC_SWITCH,
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_DETAILED_LOG): switch.switch_schema(
                WallpadLogSwitch,
                icon=ICON_NOTE_EDIT,
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_WALLPAD_TEST): button.button_schema(
                WallpadTestButton,
                icon=ICON_CODE_BRACES,
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_WALLPAD_SCAN): button.button_schema(
                WallpadScanButton,
                icon=ICON_CODE_BRACES,
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_WRITE_RETRY, default=2): cv.int_range(min=1, max=5),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "navien_wallpad",
    baud_rate=9600,
    require_tx=True,
    require_rx=True,
)


async def to_code(config):
    uart_component = await cg.get_variable(config[CONF_UART_ID])

    wp = cg.new_Pvariable(config[CONF_ID], uart_component)
    await cg.register_component(wp, config)

    if CONF_VALVE_SENSOR in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_VALVE_SENSOR])
        cg.add(wp.register_valve_close_sensor(sens))

    if CONF_VALVE_CLOSE_BUTTON in config:
        conf = config[CONF_VALVE_CLOSE_BUTTON]
        var = await button.new_button(conf)
        await cg.register_component(var, conf)

    for conf in config.get(CONF_LIGHTS, []):
        rhs = cg.new_Pvariable(conf[CONF_OUTPUT_ID])
        await cg.register_component(rhs, conf)
        await light.register_light(rhs, conf)
        cg.add(rhs.set_sub_id(conf[CONF_SUB_ID]))
        cg.add(wp.register_light(rhs))

    cg.add(wp.set_support_temperature_0_5(config[CONF_SUPPORT_TEMPERATURE_0_5]))

    for conf in config.get(CONF_CLIMATES, []):
        rhs = await climate.new_climate(conf)
        await cg.register_component(rhs, conf)
        cg.add(rhs.set_sub_id(conf[CONF_SUB_ID]))
        cg.add(wp.register_climate(rhs))

    measures = config.get(CONF_MEASURES, {})
    for key in (
        CONF_WATER_CURRENT,
        CONF_WATER_TOTAL,
        CONF_GAS_CURRENT,
        CONF_GAS_TOTAL,
        CONF_POWER_CURRENT,
        CONF_POWER_TOTAL,
    ):
        if key in measures:
            conf = measures[key]
            var = await sensor.new_sensor(conf)
            await cg.register_component(var, conf)

    if CONF_BREAKER in config:
        conf = config[CONF_BREAKER]
        var = await switch.new_switch(conf)
        await cg.register_component(var, conf)
        cg.add(wp.register_breaker(var))

    if CONF_DETAILED_LOG in config:
        conf = config[CONF_DETAILED_LOG]
        var = await switch.new_switch(conf)
        await cg.register_component(var, conf)

    for key in (CONF_WALLPAD_TEST, CONF_WALLPAD_SCAN):
        if key in config:
            conf = config[key]
            var = await button.new_button(conf)
            await cg.register_component(var, conf)

    cg.add(wp.set_write_retry(config[CONF_WRITE_RETRY]))

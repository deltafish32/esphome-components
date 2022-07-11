#pragma once



namespace esphome {
namespace navien_wallpad {


typedef struct _DEVICE_DATA {
  bool recv{false};
  bool init_publish{false};
} DEVICE_DATA;


typedef struct _GROUP_DEVICE_DATA : public DEVICE_DATA {
  byte group_id{0};
} GROUP_DEVICE_DATA;


typedef struct _LIGHT_DATA : public GROUP_DEVICE_DATA {
  bool state[KSX4506_MAX_LIGHTS]{false, };
  bool state_new[KSX4506_MAX_LIGHTS]{false, };
} LIGHT_DATA;


typedef struct _VALVE_DATA : public DEVICE_DATA {
  byte state{0};
  // byte state_new{0};
} VALVE_DATA;


typedef struct _MEASURE_DATA : public DEVICE_DATA {
  unsigned long water_current{0};
  unsigned long water_total{0};
  unsigned long gas_current{0};
  unsigned long gas_total{0};
  unsigned long power_current{0};
  unsigned long power_total{0};
} MEASURE_DATA;


typedef struct _THERMOSTAT_STATE {
  byte error{0};
  byte heating{0};
  byte outing{0};
  byte reservation{0};
  byte hotwater{0};
  byte target[KSX4506_MAX_CLIMATES]{0, };
  byte current[KSX4506_MAX_CLIMATES]{0, };
} THERMOSTAT_STATE;

typedef struct _THERMOSTAT_DATA : public GROUP_DEVICE_DATA {
  THERMOSTAT_STATE state;
  // THERMOSTAT_STATE state_new;
} THERMOSTAT_DATA;


typedef struct _BREAKER_DATA : public DEVICE_DATA {
  bool break_lights{false};
  // bool break_lights_new{false};
} BREAKER_DATA;


} // navien_wallpad
} // esphome

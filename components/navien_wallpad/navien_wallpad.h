#pragma once


#include "esphome/components/uart/uart.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"

#include "cyclequeue.h"
#include "ksx4506.h"
#include <list>

#include "wallpad_struct.h"
#include "wallpad_climate.h"
#include "wallpad_light.h"


namespace esphome {
namespace navien_wallpad {

#define VERSION "0.2.0"


class Navien_Wallpad : public uart::UARTDevice, public Component {
public:
  Navien_Wallpad(uart::UARTComponent *parent);


private:
  Navien_Wallpad(const Navien_Wallpad&);


protected:
  static Navien_Wallpad* __this;


public:
  float get_setup_priority() const override;

  void setup() override;
  void loop() override;
  void dump_config() override;


protected:
  void loop_read();
  void loop_write();
  void loop_publish();

  void publish_lights();
  void publish_valve();
  void publish_breaker();
  void publish_thermostat();


public:
  void register_valve_close_sensor(binary_sensor::BinarySensor *sensor);
  void register_breaker(switch_::Switch *sw);
  void register_climate(WallpadClimate *climate);
  void register_light(WallpadLightOutput *light);

  void set_support_temperature_0_5(bool b);
  void set_write_retry(int retry);


protected:
  binary_sensor::BinarySensor *valve_close_sensor_{nullptr};
  switch_::Switch *breaker_switch_{nullptr};
  std::vector<WallpadClimate *> climates_;
  std::vector<WallpadLightOutput *> light_outputs_;

  bool support_temperature_0_5_{false};
  int write_retry_{2};


protected:
  CycleQueue<byte> read_queue_;
  std::list<KSX4506_DATA> write_queue_;

  bool read_flag_{false};
  unsigned long read_tick_;

  KSX4506_DATA write_last_;
  int write_try_;

  int polling_step_{0};
  bool polling_flag_;
  unsigned long write_tick_;
  const unsigned long write_delay_{20};
  const unsigned long write_timeout_{200};

  unsigned long publish_tick_;
  const unsigned long publish_period_{1500};
  unsigned long publish_slow_tick_; // thermostat
  const unsigned long publish_slow_period_{6000};

  bool recv_thermostat_req_{false}; // Wallpad 에서 자체적으로 패킷이 날아오는 경우
  unsigned long recv_thermostat_req_tick_;
  const unsigned long thermostat_req_timeout_{3000};

  bool show_detailed_log_{false};


protected:
  VALVE_DATA valve_;
  MEASURE_DATA measure_;
  BREAKER_DATA breaker_;
  std::vector<LIGHT_DATA> lights_;
  std::vector<THERMOSTAT_DATA> thermostats_;


protected:
  void restart_publish_timer();
  static int parse_packet(const CycleQueue<byte>& queue, KSX4506_DATA* p);
  static unsigned long bcd2dec(byte b3, byte b2, byte b1, byte b0);
  bool on_recv_ksx4506(const KSX4506_DATA& ksx_data);
  template <class _Ty> void trace_data(const char* tag, const _Ty& data, byte len);
  //bool light_store_state(byte sub_id, bool state);

  static bool is_valid_sub_id_light(byte sub_id);
  static bool is_valid_sub_id_climate(byte sub_id);


public:
  static bool get_breaker_init();
  static bool get_climate_init(byte sub_id);
  static bool get_light_init(byte sub_id);

  static void set_light_state(byte sub_id, bool onoff);
  static void set_valve_close();
  static void set_break_lights(bool onoff);
  static void set_show_detailed_log(bool onoff);
  static void set_thermostat_onoff(byte sub_id, bool onoff);
  static void set_thermostat_temperature(byte sub_id, float temperature);

  static double get_water_current();
  static double get_water_total();
  static double get_gas_current();
  static double get_gas_total();
  static double get_power_current();
  static double get_power_total();

  static void push_queue(const KSX4506_DATA& ksx_data);
  

};


} // namespace navien_wallpad
} // namespace esphome

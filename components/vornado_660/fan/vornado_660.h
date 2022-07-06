#pragma once

#include <Arduino.h>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/fan/fan.h"

#include <list>


namespace esphome {
namespace vornado_660 {

#define VERSION "1.0.0"


class Vornado_660 : public fan::Fan, public Component {
public:
  Vornado_660();


public:
  void setup() override;
  void loop() override;

  void dump_config() override;
  fan::FanTraits get_traits() override;


protected:
  void control(const fan::FanCall &call) override;


protected:
  InternalGPIOPin *power_pin_;
  InternalGPIOPin *ll_pin_;
  InternalGPIOPin *l_pin_;
  InternalGPIOPin *h_pin_;
  InternalGPIOPin *hh_pin_;
  InternalGPIOPin *ll_fb_pin_;
  InternalGPIOPin *l_fb_pin_;
  InternalGPIOPin *h_fb_pin_;
  InternalGPIOPin *hh_fb_pin_;
  // InternalGPIOPin *relay_pin_{NULL};


public:
  void set_power_pin(InternalGPIOPin *pin);
  void set_ll_pin(InternalGPIOPin *pin);
  void set_l_pin(InternalGPIOPin *pin);
  void set_h_pin(InternalGPIOPin *pin);
  void set_hh_pin(InternalGPIOPin *pin);
  void set_ll_fb_pin(InternalGPIOPin *pin);
  void set_l_fb_pin(InternalGPIOPin *pin);
  void set_h_fb_pin(InternalGPIOPin *pin);
  void set_hh_fb_pin(InternalGPIOPin *pin);
  // void set_relay_pin(InternalGPIOPin *pin);


protected:
  int next_command_{-1};
  int step_{0};
  int command_{0};
  unsigned long tick_{0};
  bool power_toggled_{false};

  const unsigned long trigger_length_{50};
  const unsigned long next_delay_{100};
  const unsigned long feedback_delay_{500};
  // const unsigned long relay_delay_{500};


protected:
  int feedback();


};


} // vornado_660
} // esphome

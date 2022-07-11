#pragma once

#include <Arduino.h>
#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"


namespace esphome {
namespace navien_wallpad {


class WallpadClimate : public climate::Climate, public Component {
public:
  WallpadClimate();
  
  float get_setup_priority() const override;

  void setup() override;
  void dump_config() override;


public:
  climate::ClimateTraits traits() override;
  void control(const climate::ClimateCall &call) override;


public:
  void set_sub_id(int sub_id);
  int get_sub_id() const;


protected:
  int sub_id_;
};


} // namespace navien_wallpad
} // namespace esphome

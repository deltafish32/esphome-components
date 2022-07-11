#pragma once

#include <Arduino.h>
#include "esphome/core/component.h"
#include "esphome/components/light/light_effect.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"


namespace esphome {
namespace navien_wallpad {


class WallpadLightOutput : public light::LightOutput, public Component {
public:
  WallpadLightOutput();
  
  float get_setup_priority() const override;

  void setup() override;
  void dump_config() override;


public:
  light::LightTraits get_traits() override;
  void setup_state(light::LightState *state) override;
  void write_state(light::LightState *state) override;


public:
  light::LightState *get_light_state();

  void set_sub_id(int sub_id);
  int get_sub_id() const;


protected:
  light::LightState *light_state_;
  int sub_id_;
};


} // namespace navien_wallpad
} // namespace esphome

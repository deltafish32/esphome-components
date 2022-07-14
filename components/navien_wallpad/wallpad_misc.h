#pragma once


#include "esphome/core/component.h"
#include "esphome/components/button/button.h"
#include "esphome/components/switch/switch.h"


namespace esphome {
namespace navien_wallpad {


class WallpadBreakLightsSwitch : public switch_::Switch, public Component {
public:
  float get_setup_priority() const override;

  void setup() override;
  void dump_config() override;


protected:
  void write_state(bool state) override;

};


class WallpadLogSwitch : public switch_::Switch, public Component {
public:
  float get_setup_priority() const override;

  void setup() override;
  void dump_config() override;


protected:
  void write_state(bool state) override;

};


class WallpadTestButton : public button::Button, public Component {
public:
  void dump_config() override;
  float get_setup_priority() const override;

protected:
  void press_action() override;

};


class WallpadScanButton : public button::Button, public Component {
public:
  void dump_config() override;
  float get_setup_priority() const override;

protected:
  void press_action() override;

};


} // navien_wallpad
} // esphome

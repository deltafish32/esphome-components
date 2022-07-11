#pragma once

#include <Arduino.h>
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"


namespace esphome {
namespace navien_wallpad {


class WallpadMeasure : public sensor::Sensor, public PollingComponent {
protected:
  WallpadMeasure();


public:
  float get_setup_priority() const override;

  void setup() override;
  void update() override;
  void dump_config() override;


protected:
  const char* class_name_;

};


class WallpadWaterCurrent : public WallpadMeasure {
public:
  WallpadWaterCurrent();
  
  void update() override;

};


class WallpadWaterTotal : public WallpadMeasure {
public:
  WallpadWaterTotal();
  
  void update() override;

};


class WallpadGasCurrent : public WallpadMeasure {
public:
  WallpadGasCurrent();
  
  void update() override;

};


class WallpadGasTotal : public WallpadMeasure {
public:
  WallpadGasTotal();
  
  void update() override;

};


class WallpadPowerCurrent : public WallpadMeasure {
public:
  WallpadPowerCurrent();
  
  void update() override;

};


class WallpadPowerTotal : public WallpadMeasure {
public:
  WallpadPowerTotal();
  
  void update() override;

};

} // namespace navien_wallpad
} // namespace esphome

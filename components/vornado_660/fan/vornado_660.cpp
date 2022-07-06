#include "vornado_660.h"


namespace esphome {
namespace vornado_660 {

static const char *TAG = "Vornado_660";



Vornado_660::Vornado_660() {
}


void Vornado_660::setup() {
  power_pin_->pin_mode(gpio::FLAG_OUTPUT);
  ll_pin_->pin_mode(gpio::FLAG_OUTPUT);
  l_pin_->pin_mode(gpio::FLAG_OUTPUT);
  h_pin_->pin_mode(gpio::FLAG_OUTPUT);
  hh_pin_->pin_mode(gpio::FLAG_OUTPUT);

  ll_fb_pin_->pin_mode(gpio::FLAG_INPUT);
  l_fb_pin_->pin_mode(gpio::FLAG_INPUT);
  h_fb_pin_->pin_mode(gpio::FLAG_INPUT);
  hh_fb_pin_->pin_mode(gpio::FLAG_INPUT);

  // if (relay_pin_ != NULL) {
  //   relay_pin_->pin_mode(gpio::FLAG_OUTPUT);
  // }

  // FanCall.perform 내의 validate_ 에서 state == false, speed == 0 인 경우,
  // speed 를 최대로 올려버리는 문제가 있습니다. 초기 속도를 설정해줍니다.
  speed = 4;
}


void Vornado_660::loop() {
  switch (step_) {
  case 0: // init
    if (next_command_ >= 0) {
      command_ = next_command_;
      next_command_ = -1;
      power_toggled_ = false;
      step_ = 100;
    }
    else {
      int fb = feedback();

      if ((state != (fb > 0))
       || (state && speed != fb)) {
        tick_ = millis();
        step_ = 200;
      }
    }
    break;
  case 100: // operation
    switch (command_) {
    case 0: // off
      // fb off: not to do
      // fb on : power toggle (to off)
      if (feedback() > 0) {
        step_ = 1000; // Power toggle
      }
      else {
        step_ = 3000; // Set speed
      }
      break;
    case 1: // 1~4
    case 2:
    case 3:
    case 4: 
      // fb off: power toggle (to on), set speed
      // fb on : set speed
      if (feedback() == 0) {
        step_ = 1000; // Power toggle
      }
      else {
        step_ = 3000; // Set speed
      }
      break;
    default:
      ESP_LOGW(TAG, "invalid speed %d", command_);
      step_ = 0;
      break;
    }
    break;
  case 200: // feedback
    {
      int fb = feedback();

      if ((state != (fb > 0))
       || (state && speed != fb)) {
        if (millis() - tick_ >= feedback_delay_) {
          state = (fb > 0);
          if (state) speed = fb;
          publish_state();
          step_ = 0;
        }
      }
      else {
        // cancel
        step_ = 0;
      }
    }
    break;

  // Relay On
  case 1000:
  //   if (command_ > 0 && relay_pin_ != NULL) {
  //     relay_pin_->digital_write(HIGH);
  //     tick_ = millis();
  //     step_ = 1100;
  //   }
  //   else {
  //     step_ = 2000;
  //   }
  //   break;
  // case 1100:
  //   if (millis() - tick_ >= relay_delay_) {
      step_ = 2000;
    // }
    break;

  // Power toggle
  case 2000: // power_pin high
    power_toggled_ = true;
    power_pin_->digital_write(HIGH);
    tick_ = millis();
    step_ = 2100;
    break;
  case 2100: // delay
    if (millis() - tick_ >= trigger_length_) {
      step_ = 2200;
    }
    break;
  case 2200: // power_pin low
    power_pin_->digital_write(LOW);
    tick_ = millis();
    step_ = 2300;
    break;
  case 2300: // delay
    if (millis() - tick_ >= next_delay_) {
      step_ = 2400;
    }
    break;
  case 2400:
    if (command_ == 0) {
      step_ = 4000;
    }
    else if (command_ == 4 && power_toggled_) { // optimize
      step_ = 4000;
    }
    else {
      step_ = 3000; // Set speed
    }
    break;

  // Set speed
  case 3000: // speed pin high
    switch (command_) {
    case 1: ll_pin_->digital_write(HIGH); break;
    case 2:  l_pin_->digital_write(HIGH); break;
    case 3:  h_pin_->digital_write(HIGH); break;
    case 4: hh_pin_->digital_write(HIGH); break;
    }
    
    tick_ = millis();
    step_ = 3100;
    break;
  case 3100: // delay
    if (millis() - tick_ >= trigger_length_) {
      step_ = 3200;
    }
    break;
  case 3200: // speed pin low
    switch (command_) {
    case 1: ll_pin_->digital_write(LOW); break;
    case 2:  l_pin_->digital_write(LOW); break;
    case 3:  h_pin_->digital_write(LOW); break;
    case 4: hh_pin_->digital_write(LOW); break;
    }
    
    tick_ = millis();
    step_ = 3300;
    break;
  case 3300: // delay
    if (millis() - tick_ >= next_delay_) {
      step_ = 4000;
    }
    break;

  // Relay Off
  case 4000:
  //   if (command_ == 0 && relay_pin_ != NULL) {
  //     relay_pin_->digital_write(LOW);
  //     tick_ = millis();
  //     step_ = 4100;
  //   }
  //   else {
  //     step_ = 0;
  //   }
  //   break;
  // case 4100:
  //   if (millis() - tick_ >= next_delay_) {
      step_ = 0;
    // }
    break;

  }
}


void Vornado_660::dump_config() {
  ESP_LOGCONFIG(TAG, "%s:", TAG);
  LOG_PIN("  power_pin: ", power_pin_);
  LOG_PIN("  ll_pin: ", ll_pin_);
  LOG_PIN("  l_pin: ", l_pin_);
  LOG_PIN("  h_pin: ", h_pin_);
  LOG_PIN("  hh_pin: ", hh_pin_);
  LOG_PIN("  ll_fb_pin: ", ll_fb_pin_);
  LOG_PIN("  l_fb_pin: ", l_fb_pin_);
  LOG_PIN("  h_fb_pin: ", h_fb_pin_);
  LOG_PIN("  hh_fb_pin: ", hh_fb_pin_);
  // LOG_PIN("  relay_pin: ", relay_pin_);
  ESP_LOGCONFIG(TAG, "  step: %d", step_);
  ESP_LOGCONFIG(TAG, "  command: %d", command_);
}


fan::FanTraits Vornado_660::get_traits() {
  // oscillation : false
  // speed       : true
  // direction   : false
  // speed_count : 4
  return fan::FanTraits(false, true, false, 4);
}


void Vornado_660::control(const fan::FanCall &call) {
  if (call.get_state().has_value()) {
    state = *call.get_state();

    if (state) {
      if (call.get_speed().has_value()) {
        speed = *call.get_speed();
      }
      else {
        // off -> on: default speed 4
        // ESP_LOGD(TAG, "control speed hasn\'t value. use default speed.");
        // speed = 4;
      }

      next_command_ = speed;
    }
    else {
      next_command_ = 0;
    }

    ESP_LOGD(TAG, "control state=%s, speed=%d", state ? "true" : "false", speed);

    publish_state();
  }
}


void Vornado_660::set_power_pin(InternalGPIOPin *pin) {
  power_pin_ = pin;
}


void Vornado_660::set_ll_pin(InternalGPIOPin *pin) {
  ll_pin_ = pin;
}


void Vornado_660::set_l_pin(InternalGPIOPin *pin) {
  l_pin_ = pin;
}


void Vornado_660::set_h_pin(InternalGPIOPin *pin) {
  h_pin_ = pin;
}


void Vornado_660::set_hh_pin(InternalGPIOPin *pin) {
  hh_pin_ = pin;
}


void Vornado_660::set_ll_fb_pin(InternalGPIOPin *pin) {
  ll_fb_pin_ = pin;
}


void Vornado_660::set_l_fb_pin(InternalGPIOPin *pin) {
  l_fb_pin_ = pin;
}


void Vornado_660::set_h_fb_pin(InternalGPIOPin *pin) {
  h_fb_pin_ = pin;
}


void Vornado_660::set_hh_fb_pin(InternalGPIOPin *pin) {
  hh_fb_pin_ = pin;
}


// void Vornado_660::set_relay_pin(InternalGPIOPin *pin) {
//   relay_pin_ = pin;
// }


int Vornado_660::feedback() {
  // if (relay_pin_ != NULL && relay_pin_->digital_read() == LOW) return 0;

  if (hh_fb_pin_->digital_read()) return 4;
  if (h_fb_pin_->digital_read())  return 3;
  if (l_fb_pin_->digital_read())  return 2;
  if (ll_fb_pin_->digital_read()) return 1;
  
  return 0;
}


} // vornado_660
} // esphome

#include "ksx4506.h"


namespace esphome {
namespace navien_wallpad {


KSX4506_DATA::KSX4506_DATA() {
  device_id = 0;
  device_sub_id = 0;
  command_type = 0;
  data_len = 0;
  memset(data, 0, sizeof(data));
};


KSX4506_DATA::KSX4506_DATA(byte _id, byte _sub_id, byte _cmd_type, byte _d_len, const byte* _d) {
  device_id = _id;
  device_sub_id = _sub_id;
  command_type = _cmd_type;
  set_data(_d_len, _d);
};


KSX4506_DATA::KSX4506_DATA(const KSX4506_DATA& _other) {
  *this = _other;
};


KSX4506_DATA& KSX4506_DATA::operator =(const KSX4506_DATA& _other) {
  device_id     = _other.device_id;
  device_sub_id = _other.device_sub_id;
  command_type  = _other.command_type;
  set_data(_other.data_len, _other.data);

  return *this;
}


void KSX4506_DATA::set_data(byte _d_len, const byte* _d) {
  data_len = _d_len;
  if (data_len > 0) {
    if (_d != nullptr) {
      memcpy(data, _d, data_len);
    }
  }
}


byte KSX4506_DATA::to_buffer(byte* _buffer) const {
  _buffer[0] = 0xf7;
  _buffer[1] = device_id;
  _buffer[2] = device_sub_id;
  _buffer[3] = command_type;
  _buffer[4] = data_len;
  if (data_len > 0) {
    memcpy(_buffer + 5, data, data_len);
  }

  _buffer[5 + data_len] = xor_sum(_buffer, 5 + data_len);
  _buffer[5 + data_len + 1] = add_sum(_buffer, 5 + data_len + 1);

  return (5 + data_len + 1 + 1);
}


byte KSX4506_DATA::xor_sum(const byte* _d, int _c) {
  byte sum = 0;

  for (int i = 0; i < _c; i++) {
    sum ^= _d[i];
  }

  return sum;
}


byte KSX4506_DATA::add_sum(const byte* _d, int _c) {
  byte sum = 0;

  for (int i = 0; i < _c; i++) {
    sum += _d[i];
  }

  return sum;
}


} // namespace navien_wallpad
} // namespace esphome

#pragma once


#include <Arduino.h>


namespace esphome {
namespace navien_wallpad {


// https://standard.go.kr/KSCI/ksNotification/getKsNotificationView.do?ntfcManageNo=2016-00079&menuId=921&topMenuId=502

typedef enum _KSX4506_DEVICE_ID {
  //KID_AIRCONDITIONER     = 0x02, // 시스템 에어컨
  //KID_MICROWAVE          = 0x04, // 전자레인지
  //KID_DISHWASHER         = 0x09, // 식기세척기
  //KID_WASHER             = 0x0a, // 드럼세탁기
  KID_LIGHT              = 0x0e, // 조명
  KID_GASVALVE           = 0x12, // 가스밸브
  //KID_COVER              = 0x13, // 커튼
  KID_MONITOR            = 0x30, // 원격검침기
  //KID_DOORLOCK           = 0x31, // 도어락
  //KID_FAN                = 0x32, // 실내환기 시스템
  KID_BREAKER            = 0x33, // 일괄차단기
  //KID_SECURITY           = 0x34, // 방범확장
  //KID_BOILER             = 0x35, // 보일러
  KID_THERMOSTAT         = 0x36, // 온도조절기
  //KID_ZIGBEE             = 0x37, // ZigBee 모듈
  //KID_SMARTPOWER         = 0x38, // 스마트 전력량계
  //KID_POWERSAVER         = 0x39, // 대기전력 차단기기
} KSX4506_DEVICE_ID;



// RPY: (REQ | 0x80)
typedef enum _KSX4506_COMMAND_TYPE {
  KCT_REQ_STATE          = 0x01,
  KCT_RPY_STATE          = 0x81,
  KCT_REQ_SPEC           = 0x0f,
  KCT_RPY_SPEC           = 0x8f,
  KCT_REQ_CONTROL        = 0x41,
  KCT_RPY_CONTROL        = 0xc1,
  KCT_REQ_CONTROLALL     = 0x42,
  KCT_RPY_CONTROLALL     = 0xc2,

  // thermostat
  KCT_REQ_HEATING        = 0x43,
  KCT_RPY_HEATING        = 0xc3,
  KCT_REQ_TEMPERATURE    = 0x44,
  KCT_RPY_TEMPERATURE    = 0xc4,
  KCT_REQ_OUTING         = 0x45, // 문서에는 0x46 으로 되어있으나 0x45 로 동작.
  KCT_RPY_OUTING         = 0xc5, // 문서에는 0xc6 으로 되어있으나 0xc5 로 동작.
} KSX4506_COMMAND_TYPE;


// 명령어 최소/최대 길이
#define KSX4506_MIN_SIZE          7
#define KSX4506_MAX_SIZE          262

#define KSX4506_GROUP_ALL         0x0f
#define KSX4506_ID_ALL            0x0f

// 1부터 시작, 끝 값 포함
#define KSX4506_MAX_GROUPS        14
#define KSX4506_MAX_LIGHTS        14
#define KSX4506_MAX_CLIMATES      8

#define MAKE_SUB_ID(GROUP, EACH)  (((GROUP) << 4) | (EACH))
#define GET_GROUP(SUB_ID)         (((SUB_ID) & 0xf0) >> 4)
#define GET_EACH(SUB_ID)          ((SUB_ID) & 0x0f)




class KSX4506_DATA
{
public:
  byte device_id;
  byte device_sub_id;
  byte command_type;
  byte data_len;
  byte* data;


public:
  KSX4506_DATA();
  KSX4506_DATA(byte _id, byte _sub_id, byte _cmd_type, byte _d_len, const byte* _d);
  KSX4506_DATA(const KSX4506_DATA& _other);

  ~KSX4506_DATA();


public:
  KSX4506_DATA& operator =(const KSX4506_DATA& _other) ;

  void set_data(byte _d_len, const byte* _d);
  void release();
  byte to_buffer(byte* _buffer) const;


public:
  static byte xor_sum(const byte* _d, int _c);
  static byte add_sum(const byte* _d, int _c);

};


} // namespace navien_wallpad
} // namespace esphome

#pragma once
#include "esphome/core/log.h"



#define BOOL_STR(VAL)           ( (VAL) ? "true" : "false" )

#define LOG_NOT_INITIALIZED()   ESP_LOGW(TAG, "not initialized (%s)", __func__);

#define LOG_SET_NULL(VAL)       ESP_LOGD(TAG, "%s()", __func__)
#define LOG_SET_INT(VAL)        ESP_LOGD(TAG, "%s(%d)", __func__, VAL)
#define LOG_SET_BOOL(VAL)       ESP_LOGD(TAG, "%s(%s)", __func__, BOOL_STR(VAL))
#define LOG_SET_FLOAT(VAL)      ESP_LOGD(TAG, "%s(%.3f)", __func__, VAL)

//
// Created by 李世星 on 2020-09-14.
//

#pragma once

#ifndef SXPLAYER_ANDROIDLOG_H
#define SXPLAYER_ANDROIDLOG_H

#include <android/log.h>
#define LOG_SHOW true

#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG, "sxplayer",FORMAT, ##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, "sxplayer",FORMAT, ##__VA_ARGS__);

#endif //SXPLAYER_ANDROIDLOG_H

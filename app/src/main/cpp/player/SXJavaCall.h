//
// Created by 李世星 on 2020-09-14.
//

#ifndef SXPLAYER_SXJAVACALL_H
#define SXPLAYER_SXJAVACALL_H

#include <jni.h>
#include <stdint.h>
#include "SXPlayStatus.h"
#include "SXStataus.h"

class SXJavaCall {
public:
    _JavaVM *javaVm = NULL;
    JNIEnv  *jniEnv = NULL;
    jobject jobj;

    jmethodID jmid_error;
    jmethodID jmid_load;
    jmethodID jmid_parpared;
    jmethodID jmid_init_mediacodec;
    jmethodID jmid_dec_mediacode;
    jmethodID jmid_gl_yuv;
    jmethodID jmid_info;
    jmethodID jmid_complete;
    jmethodID jmid_onlysoft;

public:
    SXJavaCall(_JavaVM *javaVM, JNIEnv *env, jobject *jobj);
    ~SXJavaCall();
    void onError(int type, int code, const char *msg);
    void onLoad(int type, bool load);
    void onParpared(int type);

    void onInitMediacodec(int type, int mimetype, int width, int height, int csd_0_size, int csd_1_size, uint8_t * csd_0, uint8_t * csd_1);

    void onDecMediacodec(int type, int size, uint8_t *data, int pts);

    void onGlRenderYuv(int type, int width, int height, uint8_t *y, uint8_t *u, uint8_t *v);

    void onVideoInfo(int type, int currt_secd, int total_secd);

    void onComplete(int type);

    void release();

    bool isOnlySoft(int type);
};

#endif //SXPLAYER_SXJAVACALL_H

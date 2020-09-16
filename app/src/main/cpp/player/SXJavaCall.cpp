//
// Created by 李世星 on 2020-09-14.
//

#include <libavutil/mem.h>
#include "SXJavaCall.h"
#include "../common/AndroidLog.h"
#include "../../../../../../../../../../Library/Android/sdk/ndk/21.1.6352462/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/jni.h"

SXJavaCall::SXJavaCall(_JavaVM *vm, JNIEnv *env, jobject *obj) {
    javaVm = vm;
    jniEnv = env;
    jobj = *obj;

    jobj = env->NewGlobalRef(jobj);
    jclass jlz = jniEnv->GetObjectClass(jobj);
    if (!jlz) {
        LOGE("find jclass failed");
        return;
    }

    jmid_error = jniEnv->GetMethodID(jlz, "onError", "(ILjava/lang/String;)V");
    jmid_load = jniEnv->GetMethodID(jlz, "onLoad", "(Z)V");
    jmid_parpared = jniEnv->GetMethodID(jlz, "onParpared", "()V");
    jmid_init_mediacodec = jniEnv->GetMethodID(jlz, "mediacodecInit", "(III[B[B)V");
    jmid_dec_mediacode = jniEnv->GetMethodID(jlz, "mediacodecDecode", "([BII)V");
    jmid_gl_yuv = jniEnv->GetMethodID(jlz, "setFrameData", "(II[B[B[B)V");
    jmid_info = jniEnv->GetMethodID(jlz, "setVideoInfo", "(II)V");
    jmid_complete = jniEnv->GetMethodID(jlz, "videoComplete", "()V");
    jmid_onlysoft = jniEnv->GetMethodID(jlz, "isOnlySoft", "()Z");
}

SXJavaCall::~SXJavaCall() {

}

void SXJavaCall::onError(int type, int code, const char *msg) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }

        jstring jmsg = tempJniEnv->NewStringUTF(msg);
        tempJniEnv->CallVoidMethod(jobj, jmid_error, code, jmsg);
        tempJniEnv->DeleteLocalRef(jmsg);

        javaVm->DetachCurrentThread();
    } else {
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);
    }
}

void SXJavaCall::onLoad(int type, bool load) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }

        tempJniEnv->CallVoidMethod(jobj, jmid_load, load);
        javaVm->DetachCurrentThread();
    } else {
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
    }
}

void SXJavaCall::onParpared(int type) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
//            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }
        tempJniEnv->CallVoidMethod(jobj, jmid_parpared);
        javaVm->DetachCurrentThread();
    } else {
        jniEnv->CallVoidMethod(jobj, jmid_parpared);
    }
}

void SXJavaCall::onInitMediacodec(int type, int mimetype, int width, int height, int csd_0_size,
                                  int csd_1_size, uint8_t *csd_0, uint8_t *csd_1) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }

        jbyteArray csd0 = tempJniEnv->NewByteArray(csd_0_size);
        tempJniEnv->SetByteArrayRegion(csd0, 0, csd_0_size, (jbyte *) csd_0);
        jbyteArray csd1 = tempJniEnv->NewByteArray(csd_1_size);
        tempJniEnv->SetByteArrayRegion(csd1, 0, csd_1_size, (jbyte *) csd_1);

        tempJniEnv->CallVoidMethod(jobj, jmid_init_mediacodec, mimetype,
                                   width, height, csd0, csd1);

        tempJniEnv->DeleteLocalRef(csd0);
        tempJniEnv->DeleteLocalRef(csd1);
        javaVm->DetachCurrentThread();
    } else {
        jbyteArray csd0 = jniEnv->NewByteArray(csd_0_size);
        jniEnv->SetByteArrayRegion(csd0, 0, csd_0_size, (jbyte *) csd_0);
        jbyteArray csd1 = jniEnv->NewByteArray(csd_1_size);
        jniEnv->SetByteArrayRegion(csd1, 0, csd_1_size, (jbyte *) csd_1);

        jniEnv->CallVoidMethod(jobj, jmid_init_mediacodec, mimetype,
                               width, height, csd0, csd1);

        jniEnv->DeleteLocalRef(csd0);
        jniEnv->DeleteLocalRef(csd1);
    }
}

void SXJavaCall::onDecMediacodec(int type, int size, uint8_t *packet_data, int pts) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }

        jbyteArray data = tempJniEnv->NewByteArray(size);
        tempJniEnv->SetByteArrayRegion(data, 0, size, (jbyte *) packet_data);

        tempJniEnv->CallVoidMethod(jobj, jmid_dec_mediacode, data, size, pts);

        tempJniEnv->DeleteLocalRef(data);
        javaVm->DetachCurrentThread();
    } else {
        jbyteArray data = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(data, 0, size, (jbyte *) packet_data);
        jniEnv->CallVoidMethod(jobj, jmid_dec_mediacode, data, size, pts);
        jniEnv->DeleteLocalRef(data);
    }
}

void
SXJavaCall::onGlRenderYuv(int type, int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }

        jbyteArray y = tempJniEnv->NewByteArray(width * height);
        tempJniEnv->SetByteArrayRegion(y, 0, width * height, (jbyte *) fy);

        jbyteArray u = tempJniEnv->NewByteArray(width * height / 4);
        tempJniEnv->SetByteArrayRegion(u, 0, width * height / 4, (jbyte *) fu);

        jbyteArray v = tempJniEnv->NewByteArray(width * height / 4);
        tempJniEnv->SetByteArrayRegion(v, 0, width * height / 4, (jbyte *) fv);

        tempJniEnv->CallVoidMethod(jobj, jmid_gl_yuv, width, height, y, u, v);

        tempJniEnv->DeleteLocalRef(y);
        tempJniEnv->DeleteLocalRef(u);
        tempJniEnv->DeleteLocalRef(v);

        javaVm->DetachCurrentThread();
    } else {
        jbyteArray y = jniEnv->NewByteArray(width * height);
        jniEnv->SetByteArrayRegion(y, 0, width * height, (jbyte *) fy);

        jbyteArray u = jniEnv->NewByteArray(width * height / 4);
        jniEnv->SetByteArrayRegion(u, 0, width * height / 4, (jbyte *) fu);

        jbyteArray v = jniEnv->NewByteArray(width * height / 4);
        jniEnv->SetByteArrayRegion(v, 0, width * height / 4, (jbyte *) fv);

        jniEnv->CallVoidMethod(jobj, jmid_gl_yuv, width, height, y, u, v);

        jniEnv->DeleteLocalRef(y);
        jniEnv->DeleteLocalRef(u);
        jniEnv->DeleteLocalRef(v);
    }
}

void SXJavaCall::onVideoInfo(int type, int currt_secd, int total_secd) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
//            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }
        tempJniEnv->CallVoidMethod(jobj, jmid_info, currt_secd, total_secd);
        javaVm->DetachCurrentThread();
    } else {
        jniEnv->CallVoidMethod(jobj, jmid_info, currt_secd, total_secd);
    }
}

void SXJavaCall::onComplete(int type) {
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
//            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }
        tempJniEnv->CallVoidMethod(jobj, jmid_complete);
        javaVm->DetachCurrentThread();
    } else {
        jniEnv->CallVoidMethod(jobj, jmid_complete);
    }
}

void SXJavaCall::release() {
    if (javaVm != NULL) {
        javaVm = NULL;
    }

    if (jniEnv != NULL) {
        jniEnv = NULL;
    }
}

bool SXJavaCall::isOnlySoft(int type) {
    bool soft = false;
    if (type == SX_THREAD_CHILD) {
        JNIEnv *tempJniEnv;
        if (javaVm->AttachCurrentThread(&tempJniEnv, 0) != JNI_OK) {
//            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return soft;
        }
        soft = tempJniEnv->CallBooleanMethod(jobj, jmid_onlysoft);
        javaVm->DetachCurrentThread();
    } else {
        soft = jniEnv->CallBooleanMethod(jobj, jmid_onlysoft);
    }
    return soft;
}

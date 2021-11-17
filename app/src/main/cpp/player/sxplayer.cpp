#include <jni.h>
#include <string>
#include "../common/AndroidLog.h"
#include "SXJavaCall.h"
#include "SXFFmpeg.h"

_JavaVM *javaVM = NULL;
SXJavaCall *sxJavaCall = NULL;
SXFFmpeg *sxFFmpeg = NULL;

extern "C" JNIEXPORT jstring JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxPrepared(JNIEnv *env, jobject thiz,
                                                      jstring data_source, jboolean is_only_music) {
    const char *url = env->GetStringUTFChars(data_source, 0);

    if (sxJavaCall == NULL) {
        sxJavaCall = new SXJavaCall(javaVM, env, &thiz);
    }

    if (sxFFmpeg == NULL) {
        sxFFmpeg = new SXFFmpeg(sxJavaCall, url, is_only_music);
        sxJavaCall->onLoad(SX_THREAD_MAIN, true);
        sxFFmpeg->preparedFFmpeg();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxStart(JNIEnv *env, jobject thiz) {
    if (sxFFmpeg != NULL) {
        sxFFmpeg->start();
    }
}

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jint result = -1;
    javaVM = vm;
    JNIEnv* env;

    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        if(LOG_SHOW)
        {
            LOGE("GetEnv failed!");
        }
        return result;
    }
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxStop(JNIEnv *env, jobject thiz, jboolean exit) {
    if (sxFFmpeg != NULL) {
        sxFFmpeg->exitByUser = true;
        sxFFmpeg->release();
        delete(sxFFmpeg);
        sxFFmpeg = NULL;
        if (sxJavaCall != NULL) {
            sxJavaCall->release();
            sxJavaCall = NULL;
        }

        if (!exit) {
            jclass jlz = env->GetObjectClass(thiz);
            jmethodID  jmid_stop = env->GetMethodID(jlz, "onStopComplete","()V");
            env->CallVoidMethod(thiz, jmid_stop);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxPause(JNIEnv *env, jobject thiz) {
    if (sxFFmpeg != NULL) {
        sxFFmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxResume(JNIEnv *env, jobject thiz) {
    if (sxFFmpeg != NULL) {
        sxFFmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxSeek(JNIEnv *env, jobject thiz, jint secds) {
    if (sxFFmpeg != NULL) {
        sxFFmpeg->seek(secds);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxSetAudioChannels(JNIEnv *env, jobject thiz,
                                                              jint index) {
    if (sxFFmpeg != NULL) {
        sxFFmpeg->setAudioChannel(index);
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxGetDuration(JNIEnv *env, jobject thiz) {
    if(sxFFmpeg != NULL)
    {
        return sxFFmpeg->getDuration();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxGetAudioChannels(JNIEnv *env, jobject thiz) {
    if(sxFFmpeg != NULL)
    {
        return sxFFmpeg->getAudioChannels();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxGetVideoChannels(JNIEnv *env, jobject thiz) {
    if(sxFFmpeg != NULL)
    {
        return sxFFmpeg->getVideoChannels();
    }
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxGetVideoWidth(JNIEnv *env, jobject thiz) {
    if(sxFFmpeg != NULL)
    {
        return sxFFmpeg->getVideoWidth();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxGetVideoHeight(JNIEnv *env, jobject thiz) {
    if(sxFFmpeg != NULL)
    {
        return sxFFmpeg->getVideoHeight();
    }
    return 0;
}
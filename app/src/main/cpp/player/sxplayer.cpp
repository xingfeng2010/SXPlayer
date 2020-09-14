#include <jni.h>
#include <string>
#include "../common/AndroidLog.h"

_JavaVM *javaVM = NULL;
//SXJavaCall *wlJavaCall = NULL;
//SXFFmpeg *wlFFmpeg = NULL;

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
    // TODO: implement sxPrepared()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_sxStart(JNIEnv *env, jobject thiz) {
    // TODO: implement sxStart()
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
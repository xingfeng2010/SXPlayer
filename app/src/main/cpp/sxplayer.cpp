#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_xingfeng_sxplayer_player_SXPlayer_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

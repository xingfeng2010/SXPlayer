//
// Created by 李世星 on 2020-09-19.
//

#pragma once
#ifndef SXPLAYER_SXAUDIO_H
#define SXPLAYER_SXAUDIO_H

#include "SXBasePlayer.h"
#include "SXQueue.h"
#include "../common/AndroidLog.h"
#include "SXPlayStatus.h"
#include "SXJavaCall.h"

extern "C"
{
#include "../include/libswresample/swresample.h"
#include "../include/libavutil/time.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class SXAudio : public SXBasePlayer {
public:
    SXQueue *queue = NULL;
    SXPlayStatus *sxPlayStatus = NULL;
    SXJavaCall *sxJavaCall = NULL;
    pthread_t audioThread;

    /** 函数调用返回结果*/
    int ret = 0;
    /** 重采样为立体声*/
    int64_t dst_layout = 0;
    /** 计算转换后的sample个数 a * b / c*/
    int dst_nb_samples = 0;
    /** 转换，返回值为转换后的sample个数*/
    int nb = 0;
    /** buffer 内存区域*/
    uint8_t *out_buffer = NULL;
    /** 输出声道数*/
    int out_channels = 0;
    /** buffer大小*/
    int data_size = 0;
    enum AVSampleFormat dst_format;
    //opensl es

    void *buffer = NULL;
    int pcmsize = 0;
    int sample_rate = 44100;
    bool isExit = false;
    bool isVideo = false;

    bool isReadPacketFinish = true;
    AVPacket *packet;

    //引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmPlayerVolume = NULL;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

public:
    SXAudio(SXPlayStatus *playStatus,int samplate, SXJavaCall *javaCall);
    ~SXAudio();

    void setVideo(bool video);

    void playAudio();
    int getPcmData(void **pcm);
    int initOpenSL();
    void pause();
    void resume();
    void release();
    int getSLSampleRate();
    void setClock(int secds);
};

#endif //SXPLAYER_SXAUDIO_H

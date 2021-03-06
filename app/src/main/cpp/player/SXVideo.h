//
// Created by 李世星 on 2020-09-20.
//

#ifndef SXPLAYER_SXVIDEO_H
#define SXPLAYER_SXVIDEO_H

#include "SXBasePlayer.h"
#include "SXQueue.h"
#include "SXJavaCall.h"
#include "../common/AndroidLog.h"
#include "SXAudio.h"

extern "C"
{
#include <../include/libavutil/time.h>
};

class SXVideo : public SXBasePlayer {
public:
    SXQueue *queue = NULL;
    SXAudio *sxAudio = NULL;
    SXPlayStatus *sxPlayStatus = NULL;
    pthread_t videoThread;
    pthread_t decFrame;
    SXJavaCall *sxJavaCall = NULL;

    double delayTime = 0;
    int rate = 0;
    bool isExit = true;
    bool isExit2 = true;
    int codecType = -1;
    double video_clock = 0;
    double framePts = 0;
    bool frameratebig = false;
    int playcount = -1;

public:
    SXVideo(SXJavaCall *sxJavaCall, SXAudio *audio, SXPlayStatus *playStatus);
    ~SXVideo();

    void playVideo(int codecType);
    void decodVideo();
    void release();
    double synchronize(AVFrame *srcFrame, double pts);

    double getDelayTime(double diff);
    void setClock(int secds);
};

#endif //SXPLAYER_SXVIDEO_H

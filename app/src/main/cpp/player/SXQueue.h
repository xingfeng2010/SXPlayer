//
// Created by 李世星 on 2020-09-17.
//

#ifndef SXPLAYER_SXQUEUE_H
#define SXPLAYER_SXQUEUE_H

#include "queue"
#include "SXPlayStatus.h"

extern "C"
{
#include "../include/libavcodec/avcodec.h"
#include "../include/libavutil/frame.h"
#include "pthread.h"
};

class SXQueue {
public:
    std::queue<AVPacket*> queuePacket;
    std::queue<AVFrame*> queueFrame;
    pthread_mutex_t mutexFrame;
    pthread_cond_t condFrame;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    SXPlayStatus *sxPlayStatus = NULL;

public:
    SXQueue(SXPlayStatus *playStatus);
    ~SXQueue();

    int putAVPacket(AVPacket *avPacket);
    int getAVPacket(AVPacket *avPacket);
    int clearAVPacket();
    int clearToKeyFrame();

    int putAVFrame(AVFrame *avFrame);
    int getAVFrame(AVFrame *avFrame);
    int clearAVFrame();

    void release();
    int getAVPacketSize();
    int getAVFrameSize();

    int noticeThread();
};

#endif //SXPLAYER_SXQUEUE_H

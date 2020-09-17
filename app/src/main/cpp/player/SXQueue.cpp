//
// Created by 李世星 on 2020-09-17.
//

#include "SXQueue.h"
#include "../common/AndroidLog.h"

SXQueue::SXQueue(SXPlayStatus *playStatus) {
    sxPlayStatus = playStatus;
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);

    pthread_mutex_init(&mutexFrame, NULL);
    pthread_cond_init(&condFrame, NULL);
}

SXQueue::~SXQueue() {
    sxPlayStatus = NULL;
    pthread_mutex_destroy(&mutexPacket);
    pthread_mutex_destroy(&mutexFrame);
    pthread_cond_destroy(&condPacket);
    pthread_cond_destroy(&condFrame);
}

int SXQueue::putAVPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&mutexPacket);
    queuePacket.push(avPacket);
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);

    return 0;
}

int SXQueue::getAVPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&mutexPacket);

    while (sxPlayStatus != NULL && !sxPlayStatus->exit)
    {
        if (queuePacket.size() > 0)
        {
            AVPacket *pkt = queuePacket.front();
            if (av_packet_ref(avPacket, pkt) == 0)
            {
                queuePacket.pop();
            }
            av_packet_free(&pkt);
            av_free(pkt);
            pkt = NULL;
            break;
        } else{
            if (!sxPlayStatus->exit)
            {
                pthread_cond_wait(&condPacket, &mutexPacket);
            }
        }
    }

    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int SXQueue::clearAVPacket() {
    pthread_cond_signal(&condPacket);
    pthread_mutex_lock(&mutexPacket);

    while (!queuePacket.empty())
    {
        AVPacket *pkt = queuePacket.front();
        queuePacket.pop();
        av_free(pkt->data);
        av_free(pkt->buf);
        av_free(pkt->side_data);
        pkt = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int SXQueue::getAVPacketSize() {
    int size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);

    return size;
}

int SXQueue::putAVFrame(AVFrame *avFrame) {
    pthread_mutex_lock(&mutexFrame);
    queueFrame.push(avFrame);
    pthread_cond_signal(&condFrame);
    pthread_mutex_unlock(&mutexFrame);

    return 0;
}

int SXQueue::getAVFrame(AVFrame *avFrame) {
    pthread_mutex_lock(&mutexFrame);

    while (sxPlayStatus != NULL && !sxPlayStatus->exit)
    {
        if (queueFrame.size() > 0)
        {
            AVFrame *frame = queueFrame.front();
            if (av_frame_ref(avFrame, frame) == 0)
            {
                queueFrame.pop();
            }
            av_frame_free(&frame);
            av_free(frame);
            frame = NULL;
            break;
        } else{
            if (!sxPlayStatus->exit)
            {
                pthread_cond_wait(&condFrame, &mutexFrame);
            }
        }
    }

    pthread_mutex_unlock(&mutexFrame);
    return 0;
}

int SXQueue::clearAVFrame() {
    pthread_cond_signal(&condFrame);
    pthread_mutex_lock(&mutexFrame);

    while (!queueFrame.empty())
    {
        AVFrame *frame = queueFrame.front();
        queueFrame.pop();
        av_frame_free(&frame);
        frame = NULL;
    }
    pthread_mutex_unlock(&mutexFrame);
    return 0;
}

int SXQueue::getAVFrameSize() {
    int size = 0;
    pthread_mutex_lock(&mutexFrame);
    size = queueFrame.size();
    pthread_mutex_unlock(&mutexFrame);

    return size;
}

int SXQueue::noticeThread() {
    pthread_cond_signal(&condFrame);
    pthread_cond_signal(&condPacket);
    return 0;
}

int SXQueue::clearToKeyFrame() {
    pthread_cond_signal(&condPacket);
    pthread_mutex_lock(&mutexPacket);
    while (!queuePacket.empty())
    {
        AVPacket *pkt = queuePacket.front();
        if(pkt->flags != AV_PKT_FLAG_KEY)
        {
            queuePacket.pop();
            av_free(pkt->data);
            av_free(pkt->buf);
            av_free(pkt->side_data);
            pkt = NULL;
        } else{
            break;
        }
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

void SXQueue::release() {
    noticeThread();
    clearAVPacket();
    clearAVFrame();
}
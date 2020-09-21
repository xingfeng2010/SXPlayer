//
// Created by 李世星 on 2020-09-20.
//

#ifndef SXPLAYER_SXFFMPEG_H
#define SXPLAYER_SXFFMPEG_H

#include "../common/AndroidLog.h"
#include "pthread.h"
#include "SXBasePlayer.h"
#include "SXJavaCall.h"
#include "SXAudio.h"
#include "SXVideo.h"
#include "SXPlayStatus.h"
#include "SXAudioChannel.h"

extern "C"
{
#include "../include/libavformat/avformat.h"
};

class SXFFmpeg {
public:
    const char *urlpath = NULL;
    SXJavaCall *sxJavaCall = NULL;
    pthread_t decodThread;
    AVFormatContext *pFormatCtx = NULL;
    int duration = 0;
    SXAudio *sxAudio = NULL;
    SXVideo *sxVideo = NULL;
    SXPlayStatus *sxPlayStatus = NULL;
    bool exit = false;
    bool exitByUser = false;
    int mimeType = 1;
    bool isavi = false;
    bool isOnlyMusic = false;

    std::deque<SXAudioChannel*> audiochannels;
    std::deque<SXAudioChannel*> vidoechannels;

    pthread_mutex_t init_mutex;
    pthread_mutex_t seek_mutex;

public:
    SXFFmpeg(SXJavaCall *javaCall, const char *urlpath, bool onlymusic);
    ~SXFFmpeg();
    int preparedFFmpeg();
    int decodeFFmpeg();
    int start();
    int seek(int64_t sec);
    int getDuration();
    int getAVCodecContext(AVCodecParameters * parameters, SXBasePlayer *wlBasePlayer);
    void release();
    void pause();
    void resume();
    int getMimeType(const char* codecName);
    void setAudioChannel(int id);
    void setVideoChannel(int id);
    int getAudioChannels();
    int getVideoWidth();
    int getVideoHeight();
};

#endif //SXPLAYER_SXFFMPEG_H

//
// Created by 李世星 on 2020-09-19.
//

#ifndef SXPLAYER_SXBASEPLAYER_H
#define SXPLAYER_SXBASEPLAYER_H

#include "../include/libavutil/rational.h"

extern "C"
{
#include "../include/libavcodec/avcodec.h"
};


class SXBasePlayer {
public:
    int streamIndex;
    int duration;
    double clock = 0;
    double now_time = 0;
    AVCodecContext *avCodecContext = NULL;
    AVRational time_base;

public:
    SXBasePlayer();
    ~SXBasePlayer();
};


#endif //SXPLAYER_SXBASEPLAYER_H

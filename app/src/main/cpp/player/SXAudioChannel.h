//
// Created by 李世星 on 2020-09-17.
//

#ifndef SXPLAYER_SXAUDIOCHANNEL_H
#define SXPLAYER_SXAUDIOCHANNEL_H


extern "C"
{
    #include "../include/libavutil/rational.h"
};


class SXAudioChannel {
public:
    int channelId = -1;
    AVRational time_base;
    int fps;
    int sampelRate;

public:
    SXAudioChannel(int id, AVRational base);

    SXAudioChannel(int id, AVRational base, int fps);

};






#endif //SXPLAYER_SXAUDIOCHANNEL_H

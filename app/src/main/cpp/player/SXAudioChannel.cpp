//
// Created by 李世星 on 2020-09-17.
//

#include "SXAudioChannel.h"

SXAudioChannel::SXAudioChannel(int id, AVRational base) {
    channelId = id;
    time_base = base;
}

SXAudioChannel::SXAudioChannel(int id, AVRational base, int f) {
    channelId = id;
    time_base = base;
    fps = f;
}

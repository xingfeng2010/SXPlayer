//
// Created by 李世星 on 2020-09-14.
//

#ifndef SXPLAYER_SXPLAYSTATUS_H
#define SXPLAYER_SXPLAYSTATUS_H

class SXPlayStatus {
public:
    bool exit;
    bool pause;
    bool load;
    bool seek;

public:
    SXPlayStatus();
    ~SXPlayStatus();
};


#endif //SXPLAYER_SXPLAYSTATUS_H

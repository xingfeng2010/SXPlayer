//
// Created by 李世星 on 2020-09-20.
//

#include "SXFFmpeg.h"

void *decodThread(void *data) {
    SXFFmpeg *sxfFmpeg = (SXFFmpeg *) data;
    sxfFmpeg->decodeFFmpeg();
    pthread_exit(&sxfFmpeg->decodThread);
}

void SXFFmpeg::setAudioChannel(int index) {
    if(sxAudio != NULL)
    {
        int channelsize = audiochannels.size();
        if(index < channelsize)
        {
            for(int i = 0; i < channelsize; i++)
            {
                if(i == index)
                {
                    sxAudio->time_base = audiochannels.at(i)->time_base;
                    sxAudio->streamIndex = audiochannels.at(i)->channelId;
                }
            }
        }
    }
}

void SXFFmpeg::setVideoChannel(int id) {
    if(sxVideo != NULL)
    {
        sxVideo->streamIndex = vidoechannels.at(id)->channelId;
        sxVideo->time_base = vidoechannels.at(id)->time_base;
        sxVideo->rate = 1000 / vidoechannels.at(id)->fps;
        if(vidoechannels.at(id)->fps >= 60)
        {
            sxVideo->frameratebig = true;
        } else{
            sxVideo->frameratebig = false;
        }

    }
}

int SXFFmpeg::getAudioChannels() {
    return audiochannels.size();
}

int SXFFmpeg::getVideoWidth() {
    if(sxVideo != NULL && sxVideo->avCodecContext != NULL)
    {
        return sxVideo->avCodecContext->width;
    }
    return 0;
}

int SXFFmpeg::getVideoHeight() {
    if(sxVideo != NULL && sxVideo->avCodecContext != NULL)
    {
        return sxVideo->avCodecContext->height;
    }
    return 0;
}

void SXFFmpeg::release() {
    sxPlayStatus->exit = true;
    pthread_mutex_lock(&init_mutex);
    if (LOG_SHOW) {
        LOGE("开始释放 wlffmpeg");
    }
    int sleepCount = 0;
    while (!exit) {
        if (sleepCount > 1000)//十秒钟还没有退出就自动强制退出
        {
            exit = true;
        }
        if (LOG_SHOW) {
            LOGE("wait ffmpeg  exit %d", sleepCount);
        }

        sleepCount++;
        av_usleep(1000 * 10);//暂停10毫秒
    }
    if (LOG_SHOW) {
        LOGE("释放audio....................................");
    }

    if (sxAudio != NULL) {
        if (LOG_SHOW) {
            LOGE("释放audio....................................2");
        }

        sxAudio->realease();
        delete (sxAudio);
        sxAudio = NULL;
    }
    if (LOG_SHOW) {
        LOGE("释放video....................................");
    }

    if (sxVideo != NULL) {
        if (LOG_SHOW) {
            LOGE("释放video....................................2");
        }

        sxVideo->release();
        delete (sxVideo);
        sxVideo = NULL;
    }
    if (LOG_SHOW) {
        LOGE("释放format...................................");
    }

    if (pFormatCtx != NULL) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = NULL;
    }
    if (LOG_SHOW) {
        LOGE("释放javacall.................................");
    }

    if (sxJavaCall != NULL) {
        sxJavaCall = NULL;
    }
    pthread_mutex_unlock(&init_mutex);
}

void SXFFmpeg::pause() {
    if (sxPlayStatus != NULL) {
        sxPlayStatus->pause = true;
        if (sxAudio != NULL) {
            sxAudio->pause();
        }
    }
}

void SXFFmpeg::resume() {
    if (sxPlayStatus != NULL) {
        sxPlayStatus->pause = false;
        if (sxAudio != NULL) {
            sxAudio->resume();
        }
    }
}

int SXFFmpeg::getMimeType(const char *codecName) {

    if (strcmp(codecName, "h264") == 0) {
        return 1;
    }
    if (strcmp(codecName, "hevc") == 0) {
        return 2;
    }
    if (strcmp(codecName, "mpeg4") == 0) {
        isavi = true;
        return 3;
    }
    if (strcmp(codecName, "wmv3") == 0) {
        isavi = true;
        return 4;
    }

    return -1;
}

int SXFFmpeg::getAVCodecContext(AVCodecParameters *parameters, SXBasePlayer *basePlayer) {
    AVCodec *dec = avcodec_find_decoder(parameters->codec_id);
    if (!dec) {
        sxJavaCall->onError(SX_THREAD_CHILD, 3, "get avcodec fail");
        exit = true;
        return 1;
    }

    basePlayer->avCodecContext = avcodec_alloc_context3(dec);
    if (!basePlayer->avCodecContext) {
        sxJavaCall->onError(SX_THREAD_CHILD, 4, "alloc avcodec fail");
        exit = true;
        return 1;
    }

    if (avcodec_parameters_to_context(basePlayer->avCodecContext, parameters) != 0) {
        sxJavaCall->onError(SX_THREAD_CHILD, 5, "copy avcodec fail");
        exit = true;
        return 1;
    }

    if (avcodec_open2(basePlayer->avCodecContext, dec, 0) != 0) {
        sxJavaCall->onError(SX_THREAD_CHILD, 6, "open avcodec fail");
        exit = true;
        return 1;
    }

    return 0;
}

int SXFFmpeg::preparedFFmpeg() {
    pthread_create(&decodThread, NULL, decodThread, this);
}

int SXFFmpeg::getDuration() {
    return duration;
}

int SXFFmpeg::start() {
    exit = false;
    int count = 0;
    int ret = -1;
    if (sxAudio != NULL) {
        sxAudio->playAudio();
    }

    if (sxVideo != NULL) {
        if (mimeType == -1) {
            sxVideo->playVideo(0);
        } else {
            sxVideo->playVideo(1);
        }
    }

    AVBitStreamFilterContext * mimeType = NULL;
    if (mimeType == 1) {
        mimType = av_bitstream_filter_init("h264_mp4toannexb");
    } else if (mimeType == 2) {
        mimType = av_bitstream_filter_init("hevc_mp4toannexb");
    } else if (mimeType == 3) {
        mimType = av_bitstream_filter_init("h264_mp4toannexb");
    } else if (mimeType == 4) {
        mimType = av_bitstream_filter_init("h264_mp4toannexb");
    }

    while (!sxPlayStatus->exit) {
        exit = false;
        if (sxPlayStatus->pause) {
            av_usleep(1000 * 100);
            continue;
        }

        if (sxAudio != NULL && sxAudio->queue->getAVPacketSize() > 100) {
            LOGE("wlAudio 等待..........");
            av_usleep(1000 * 100);
            continue;
        }

        if (sxVideo != NULL && sxVideo->queue->getAVPacketSize() > 100) {
            LOGE("wlVideo 等待..........");
            av_usleep(1000 * 100);
            continue;
        }

        AVPacket *packet = av_packet_alloc();
        pthread_mutex_lock(&seek_mutex);
        ret = av_read_frame(pFormatCtx, packet);
        pthread_mutex_unlock(&seek_mutex);
        if (sxPlayStatus->seek) {
            av_packet_free(&packet);
            av_free(packet);
            continue;
        }

        if (ret == 0) {
            if (sxAudio != NULL && packet->stream_index == sxAudio->streamIndex) {
                count++;
                if (LOG_SHOW) {
                    LOGE("解码第 %d 帧", count);
                }
                sxAudio->queue->putAVPacket(packet);
            } else if (sxVideo != NULL && packet->stream_index = sxVideo->streamIndex) {
                if (mimeType != NULL && !isavi) {
                    uint8_t * data;
                    av_bitstream_filter_filter(mimeType,
                                               pFormatCtx->streams[sxVideo->streamIndex]->codecpar,
                                               NULL, &data, &packet->size, packet->data,
                                               packet->size, 0);
                    uint8_t * tdata = NULL;
                    tdata = packet->data;
                    packet->data = data;
                    if (tdata != NULL) {
                        av_free(tdata);
                    }

                    sxVideo->queue->putAVPacket(packet);
                }
            } else {
                av_packet_free(&packet);
                av_free(packet);
                packet = NULL;
            }
        } else {
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
            if ((sxVideo != NULL && sxVideo->queue->getAVFrameSize() == 0) ||
                (sxAudio != NULL && sxAudio->queue->getAVPacketSize() == 0)) {
                sxPlayStatus->exit = true;
                break;
            }
        }
    }
    if (mimeType != NULL) {
        av_bitstream_filter_close(mimType);
    }

    if (!exitByUser && sxJavaCall != NULL) {
        sxJavaCall->onComplete(SX_THREAD_CHILD);
    }
    exit = true;
    return 0;
}

int avformat_interrupt_cb(void *ctx) {
    SXFFmpeg *sxFFmpeg = (SXFFmpeg *) ctx;
    if (sxFFmpeg->sxPlayStatus->exit) {
        if (LOG_SHOW) {
            LOGE("avformat_interrupt_cb return 1")
        }
        return AVERROR_EOF;
    }
    if (LOG_SHOW) {
        LOGE("avformat_interrupt_cb return 0")
    }
    return 0;
}

int SXFFmpeg::decodeFFmpeg() {
    pthread_mutex_lock(&init_mutex);
    exit = false;
    isavi = false;
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, urlpath, NULL, NULL) != 0) {
        if (LOG_SHOW) {
            LOGE("can not open url: %s", urlpath);
        }

        if (sxJavaCall != NULL) {
            sxJavaCall->onError(SX_THREAD_CHILD, SX_FFMPEG_CAN_NOT_OPEN_URL, "can not open url");
        }

        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    pFormatCtx->interrupt_callback.callback = avformat_interrupt_cb;
    pFormatCtx->interrupt_callback.opaque = this;

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        if (LOG_SHOW) {
            LOGE("can not find streams from %s", urlpath);
        }
        if (sxJavaCall != NULL) {
            sxJavaCall->onError(SX_THREAD_CHILD, SX_FFMPEG_CAN_NOT_FIND_STREAMS,
                                "can not find streams from url");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    if (pFormatCtx == NULL) {
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    duration = pFormatCtx->duration / 1000000;
    if (LOG_SHOW) {
        LOGD("channel numbers is %d", pFormatCtx->nb_streams);
    }
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type = AVMEDIA_TYPE_AUDIO) {
            if (LOG_SHOW) {
                LOGE("找到音频");
            }
            SXAudioChannel *sxAudioChannel = new SXAudioChannel(i,
                                                                pFormatCtx->streams[i]->time_base);
            audiochannels.push_front(sxAudioChannel);
        } else if (pFormatCtx->streams[i]->codecpar->codec_type = AVMEDIA_TYPE_VIDEO) {
            if (!isOnlyMusic) {
                if (LOG_SHOW) {
                    LOGE("找到视频");
                }

                int num = pFormatCtx->streams[i]->avg_frame_rate.num;
                int den = pFormatCtx->streams[i]->avg_frame_rate.den;
                if (num != 0 && den != 0) {
                    int fps = pFormatCtx->streams[i]->avg_frame_rate.num /
                              pFormatCtx->streams[i]->avg_frame_rate.den;
                    SXAudioChannel *sxAudioChannel = new SXAudioChannel(i,
                                                                        pFormatCtx->streams[i]->time_base,
                                                                        fps);
                    vidoechannels.push_front(sxAudioChannel);
                }
            }
        }
    }

    if (audiochannels.size() > 0) {
        sxAudio = new SXAudio(sxPlayStatus, sxJavaCall);
        setAudioChannel(0);
        if (sxAudio->streamIndex >= 0 && sxAudio->streamIndex < pFormatCtx->nb_streams) {
            if (getAVCodecContext(pFormatCtx->streams[sxAudio->streamIndex]->codecpar, sxAudio) !=
                0) {
                exit = true;
                pthread_mutex_unlock(&init_mutex);
                return 1;
            }
        }
    }

    if (vidoechannels.size() > 0) {
        sxVideo = new SXVideo(sxJavaCall, sxAudio, sxPlayStatus);
        setVideoChannel(0);
        if (sxVideo->streamIndex >= 0 && sxVideo->streamIndex < pFormatCtx->nb_streams) {
            if (getAVCodecContext(pFormatCtx->streams[sxVideo->streamIndex]->codecpar, sxVideo) !=
                0) {
                exit = true;
                pthread_mutex_unlock(&init_mutex);
                return 1;
            }
        }
    }

    if (sxAudio == NULL && sxVideo == NULL) {
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return 1;
    }
    if (sxAudio != NULL) {
        sxAudio->duration = pFormatCtx->duration / 1000000;
        sxAudio->sample_rate = sxAudio->avCodecContext->sample_rate;
        if (sxVideo != NULL) {
            sxAudio->setVideo(true);
        }
    }

    if (sxVideo != NULL) {
        if (LOG_SHOW) {
            LOGE("codec name is %s", sxVideo->avCodecContext->codec->name);
            LOGE("codec long name is %s", sxVideo->avCodecContext->codec->long_name);
        }

        if (!sxJavaCall->isOnlySoft(SX_THREAD_CHILD)) {
            mimeType = getMimeType(sxVideo->avCodecContext->codec->name);
        } else {
            mimeType = -1;
        }

        if (mimeType != -1) {
            sxJavaCall->onInitMediacodec(SX_THREAD_CHILD, mimeType, sxVideo->avCodecContext->width,
                                         sxVideo->avCodecContext->height,
                                         sxVideo->avCodecContext->extradata_size,
                                         sxVideo->avCodecContext->extradata_size,
                                         sxVideo->avCodecContext->extradata,
                                         sxVideo->avCodecContext->extradata)
        }
        sxVideo->duration = pFormatCtx->duration / 1000000;
    }

    if (LOG_SHOW) {
        LOGE("准备ing");
    }
    sxJavaCall->onParpared(SX_THREAD_CHILD);
    if (LOG_SHOW) {
        LOGE("准备end");
    }
    exit = true;
    pthread_mutex_unlock(&init_mutex);
    return 0;
}

SXFFmpeg::SXFFmpeg(SXJavaCall *javaCall, const char *url, bool onlymusic) {
    pthread_mutex_init(&init_mutex, NULL);
    pthread_mutex_init(&seek_mutex, NULL);
    exitByUser = false;
    isOnlyMusic = onlymusic;
    sxJavaCall = javaCall;
    urlpath = url;
    sxPlayStatus = new SXPlayStatus();
}
//
// Created by 李世星 on 2020-09-20.
//

#include <string>
#include "SXVideo.h"

SXVideo::SXVideo(SXJavaCall *javaCall, SXAudio *audio, SXPlayStatus *playStatus) {
    streamIndex = -1;
    clock = 0;
    sxJavaCall = javaCall;
    sxAudio = audio;
    queue = new SXQueue(playStatus);
    sxPlayStatus = playStatus;
}

SXVideo::~SXVideo() {
    if (LOG_SHOW) {
        LOGE("video s释放完");
    }
}

void SXVideo::release() {
    if (LOG_SHOW) {
        LOGE("开始释放audio ...");
    }

    if (sxPlayStatus != NULL) {
        sxPlayStatus->exit = true;
    }
    if (queue != NULL) {
        queue->noticeThread();
    }
    int count = 0;
    while (!isExit || !isExit2) {
        if (LOG_SHOW) {
            LOGE("等待渲染线程结束...%d", count);
        }

        if (count > 1000) {
            isExit = true;
            isExit2 = true;
        }
        count++;
        av_usleep(1000 * 10);
    }
    if (queue != NULL) {
        queue->release();
        delete (queue);
        queue = NULL;
    }
    if (sxJavaCall != NULL) {
        sxJavaCall = NULL;
    }
    if (sxAudio != NULL) {
        sxAudio = NULL;
    }
    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }
    if (sxPlayStatus != NULL) {
        sxPlayStatus = NULL;
    }

    if (pMediaCodec != NULL) {
        pMediaCodec = NULL;
    }

    if (format != NULL) {
        format = NULL;
    }

    if (bufferInfo != NULL) {
        bufferInfo = NULL;
    }
}

void *decodeVideoT(void *data) {
    SXVideo *sxVideo = (SXVideo *) data;
    sxVideo->decodVideo();
    pthread_exit(&sxVideo->videoThread);

}

static const char *getMimeType(int mimetype) {
    const char *type = 0;
    switch (mimetype) {
        case 1:
            type = "video/avc";
            break;
        case 2:
            type = "video/hevc";
            break;
        case 3:
            type = "video/mp4v-es";
            break;
        case 4:
            type = "video/x-ms-wmv";
            break;
    }

    return type;
}

void
SXVideo::initMediaCodec(ANativeWindow *window, int mimetype, int width, int height, int csd_0_size,
                        int csd_1_size, uint8_t *csd_0, uint8_t *csd_1) {
    const char *realtype = getMimeType(mimetype);
    format = AMediaFormat_new();
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, realtype);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, height);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_MAX_INPUT_SIZE, width * height);
    AMediaFormat_setBuffer(format, "csd-0", csd_0, csd_0_size);
    AMediaFormat_setBuffer(format, "csd-1", csd_1, csd_1_size);

    pMediaCodec = AMediaCodec_createDecoderByType(realtype);
    LOGD("[%s][%s][%d]fromat:%s\n", __FUNCTION__, __DATE__, __LINE__,
         AMediaFormat_toString(format));

    media_status_t status = AMediaCodec_configure(pMediaCodec, format, window, NULL, 0);
    if (status != 0) {
        LOGD("erro config %d\n", status);
        return;
    }

    bufferInfo = (AMediaCodecBufferInfo *) malloc(sizeof(AMediaCodecBufferInfo));

    status = AMediaCodec_start(pMediaCodec);
    if (status != 0) {
        LOGD("start erro %d\n", status);
        return;
    }
}

void SXVideo::nativeDecMediacodec(int size, uint8_t *packet_data, int pts) {
    LOGD("input size %d:", size);
    if (packet_data != NULL && pMediaCodec != NULL) {
        ssize_t index = AMediaCodec_dequeueInputBuffer(pMediaCodec, 10);
        size_t out_size;
        if (index >= 0) {
            uint8_t *buffer = AMediaCodec_getInputBuffer(pMediaCodec, index, &out_size);
            LOGD("output size %d:", out_size);
            memcpy(buffer, packet_data, size);
            AMediaCodec_queueInputBuffer(pMediaCodec, index, 0, size, pts, 0);
        }

        ssize_t outIndex = AMediaCodec_dequeueOutputBuffer(pMediaCodec, bufferInfo, 10);
        while (outIndex >= 0) {
            AMediaCodec_releaseOutputBuffer(pMediaCodec, outIndex, true);
            outIndex = AMediaCodec_dequeueOutputBuffer(pMediaCodec, bufferInfo, 10);
        }
    }
}

void SXVideo::decodVideo() {
    while (!sxPlayStatus->exit) {
        isExit = false;
        if (sxPlayStatus->pause) {
            continue;
        }

        if (sxPlayStatus->seek) {
            sxJavaCall->onLoad(SX_THREAD_CHILD, true);
            sxPlayStatus->load = true;
            continue;
        }

        if (queue->getAVPacketSize() == 0) {
            if (!sxPlayStatus->load) {
                sxJavaCall->onLoad(SX_THREAD_CHILD, true);
                sxPlayStatus->load = true;
            }
            continue;
        } else {
            if (sxPlayStatus->load) {
                sxJavaCall->onLoad(SX_THREAD_CHILD, false);
                sxPlayStatus->load = false;
            }
        }

        if (codecType == 1) {
            AVPacket *packet = av_packet_alloc();
            if (queue->getAVPacket(packet) != 0) {
                av_free(packet->data);
                av_free(packet->buf);
                av_free(packet->side_data);
                packet = NULL;
                continue;
            }
            double time = packet->pts * av_q2d(time_base);

            if (LOG_SHOW) {
                LOGE("video clock is %f", time);
                LOGE("audio clock is %f", sxAudio->clock);
            }

            if (time < 0) {
                time = packet->dts * av_q2d(time_base);
            }

            if (time < clock) {
                time = clock;
            }
            clock = time;
            double diff = 0;
            if (sxAudio != NULL) {
                diff = sxAudio->clock - clock;
            }
            playcount++;
            if (playcount > 500) {
                playcount = 0;
            }

            if (diff >= 0.5) {
                if (frameratebig) {
                    if (playcount % 3 == 0 && packet->flags != AV_PKT_FLAG_KEY) {
                        av_free(packet->data);
                        av_free(packet->buf);
                        av_free(packet->side_data);
                        packet = NULL;
                        continue;
                    }
                } else {
                    av_free(packet->data);
                    av_free(packet->buf);
                    av_free(packet->side_data);
                    packet = NULL;
                    continue;
                }
            }

            delayTime = getDelayTime(diff);
            if (LOG_SHOW) {
                LOGE("delay time %f diff is %f", delayTime, diff);
            }

            av_usleep(delayTime * 1000);
            sxJavaCall->onVideoInfo(SX_THREAD_CHILD, clock, duration);
//            sxJavaCall->onDecMediacodec(SX_THREAD_CHILD, packet->size, packet->data, time);
            nativeDecMediacodec(packet->size, packet->data, time);
            av_free(packet->data);
            av_free(packet->buf);
            av_free(packet->side_data);
            packet = NULL;
            continue;
        } else if (codecType == 0) {
            AVFrame *frame = av_frame_alloc();
            if (queue->getAVFrame(frame) != 0) {
                av_frame_free(&frame);
                av_free(frame);
                frame = NULL;
                continue;
            }
            if ((framePts = av_frame_get_best_effort_timestamp(frame)) == AV_NOPTS_VALUE) {
                framePts = 0;
            }
            framePts *= av_q2d(time_base);
            clock = synchronize(frame, framePts);
            double diff = 0;
            if (sxAudio != NULL) {
                diff = sxAudio->clock - clock;
            }

            delayTime = getDelayTime(diff);
            if (LOG_SHOW) {
                LOGE("delay time %f diff is %f", delayTime, diff);
            }
//            if (diff >= 0.8) {
//                av_frame_free(&frame);
//                av_free(frame);
//                frame = NULL;
//                continue;
//            }

            playcount++;
            if (playcount > 500) {
                playcount = 0;
            }

            if (diff >= 0.5) {
                if (frameratebig) {
                    if (playcount % 3 == 0) {
                        av_frame_free(&frame);
                        av_free(frame);
                        frame = NULL;
                        queue->clearToKeyFrame();
                        continue;
                    }
                } else {
                    av_frame_free(&frame);
                    av_free(frame);
                    frame = NULL;
                    queue->clearToKeyFrame();
                    continue;
                }
            }

            av_usleep(delayTime * 1000);
            sxJavaCall->onVideoInfo(SX_THREAD_CHILD, clock, duration);
            sxJavaCall->onGlRenderYuv(SX_THREAD_CHILD, frame->linesize[0], frame->height,
                                      frame->data[0], frame->data[1], frame->data[2]);
            av_frame_free(&frame);
            av_free(frame);
            frame = NULL;
        }
    }

    isExit = true;
}

void *codecFrame(void *data) {
    SXVideo *sxVideo = (SXVideo *) data;
    while (!sxVideo->sxPlayStatus->exit) {
        if (sxVideo->sxPlayStatus->seek) {
            continue;
        }

        sxVideo->isExit2 = false;
        if (sxVideo->queue->getAVFrameSize() > 20) {
            continue;
        }

        if (sxVideo->codecType == 1) {
            if (sxVideo->queue->getAVPacketSize() == 0)//加载
            {
                if (!sxVideo->sxPlayStatus->load) {
                    sxVideo->sxJavaCall->onLoad(SX_THREAD_CHILD, true);
                    sxVideo->sxPlayStatus->load = true;
                }
                continue;
            } else {
                if (sxVideo->sxPlayStatus->load) {
                    sxVideo->sxJavaCall->onLoad(SX_THREAD_CHILD, false);
                    sxVideo->sxPlayStatus->load = false;
                }
            }
        }

        AVPacket *packet = av_packet_alloc();
        if (sxVideo->queue->getAVPacket(packet) != 0) {
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
            continue;
        }

        int ret = avcodec_send_packet(sxVideo->avCodecContext, packet);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
            continue;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(sxVideo->avCodecContext, frame);
        if (ret < 0 && ret != AVERROR_EOF) {
            av_frame_free(&frame);
            av_free(frame);
            frame = NULL;
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
            continue;
        }

        sxVideo->queue->putAVFrame(frame);
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }

    sxVideo->isExit2 = true;
    pthread_exit(&sxVideo->decFrame);
}

double SXVideo::getDelayTime(double diff) {
    if (LOG_SHOW) {
        LOGD("audio video diff is %f", diff);
    }

    if (diff > 0.003) {
        delayTime = delayTime / 3 * 2;
        if (delayTime < rate / 2) {
            delayTime = rate / 3 * 2;
        } else if (delayTime > rate * 2) {
            delayTime = rate * 2;
        }

    } else if (diff < -0.003) {
        delayTime = delayTime * 3 / 2;
        if (delayTime < rate / 2) {
            delayTime = rate / 3 * 2;
        } else if (delayTime > rate * 2) {
            delayTime = rate * 2;
        }
    } else if (diff == 0) {
        delayTime = rate;
    }
    if (diff > 1.0) {
        delayTime = 0;
    }
    if (diff < -1.0) {
        delayTime = rate * 2;
    }
    if (fabs(diff) > 10) {
        delayTime = rate;
    }
    return delayTime;
}

double SXVideo::synchronize(AVFrame *srcFrame, double pts) {
    double frame_delay;

    if (pts != 0) {
        video_clock = pts;// Get pts,then set video clock to it
    } else {
        pts = video_clock;// Don't get pts,set it to video clock
    }

    frame_delay = av_q2d(time_base);
    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);

    video_clock += frame_delay;

    return pts;
}

void SXVideo::setClock(int secds) {
    clock = secds;
}

void SXVideo::playVideo(int type) {
    AMediaCodec_createCodecByName("video/avc");
    codecType = type;
    if (codecType == 0) {
        pthread_create(&decFrame, NULL, codecFrame, this);
    }

    pthread_create(&videoThread, NULL, decodeVideoT, this);
}

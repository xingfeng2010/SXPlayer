package com.xingfeng.sxplayer.player;

import android.graphics.Bitmap;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import com.xingfeng.sxplayer.listener.SXOnCompleteListener;
import com.xingfeng.sxplayer.listener.SXOnCutVideoImgListener;
import com.xingfeng.sxplayer.listener.SXOnErrorListener;
import com.xingfeng.sxplayer.listener.SXOnInfoListener;
import com.xingfeng.sxplayer.listener.SXOnLoadListener;
import com.xingfeng.sxplayer.listener.SXOnPreparedListener;
import com.xingfeng.sxplayer.listener.SXOnStopListener;
import com.xingfeng.sxplayer.listener.SurfaceViewCreateListener;
import com.xingfeng.sxplayer.opengles.SXSurfaceView;
import com.xingfeng.sxplayer.util.CommonUtil;
import com.xingfeng.sxplayer.util.LogTag;

import java.io.IOException;
import java.nio.ByteBuffer;

import static com.xingfeng.sxplayer.util.PlayerStatus.SX_STATUS_DATASOURCE_NULL;
import static com.xingfeng.sxplayer.util.PlayerStatus.SX_STATUS_SURFACE_NULL;

public class SXPlayer {
    static {
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("postproc-54");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");

        System.loadLibrary("sxplayer");
    }

    /**
     * 播放文件路径
     */
    private String dataSource;
    /**
     * 硬解码mime
     */
    private MediaFormat mediaFormat;
    /**
     * 视频硬解码器
     */
    private MediaCodec mediaCodec;
    /**
     * 渲染surface
     */
    private Surface surface;

    private SXSurfaceView sxSurfaceView;

    private MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();

    private SXOnPreparedListener onPreparedListener;

    private SXOnErrorListener onErrorListener;

    private SXOnLoadListener onLoadListener;

    /**
     * 更新时间回调
     */
    private SXOnInfoListener onInfoListener;

    /**
     * 播放完成回调
     */
    private SXOnCompleteListener onCompleteListener;

    /**
     * 视频截图回调
     */
    private SXOnCutVideoImgListener onCutVideoImgListener;

    /**
     * 停止完成回调
     */
    private SXOnStopListener onStopListener;

    /**
     * 就否已经准备好
     */
    private boolean parpared = false;

    private SXTimeBean timeBean;

    /**
     * 上一次播放时间
     */
    private int lastCurrTime = 0;

    /**
     * 是否只有音频
     */
    private boolean isOnlyMusic = false;

    private boolean isOnlySoft = false;

    public SXPlayer() {
        timeBean = new SXTimeBean();
    }

    public void setDataSource(String dataSource) { this.dataSource = dataSource;}

    public void setOnlyMusic(boolean onlyMusic) { isOnlyMusic = onlyMusic; }

    public void setSurface(Surface surface) {
        this.surface = surface;
    }

    public void setSxSurfaceView(SXSurfaceView sxSurfaceView) {
        this.sxSurfaceView = sxSurfaceView;
        sxSurfaceView.setSurfaceViewCreateListener(new SurfaceViewCreateListener() {
            @Override
            public void onGlSurfaceViewOncreate(Surface s) {
                if (surface == null) {
                    setSurface(s);
                }

                if (parpared && !TextUtils.isDigitsOnly(dataSource)) {
                    sxPrepared(dataSource, surface, isOnlyMusic);
                }
            }

            @Override
            public void onCutVideoImg(Bitmap bitmap) {
                 if (onCutVideoImgListener != null) {
                     onCutVideoImgListener.onCutVideoImg(bitmap);
                 }
            }
        });
    }

    /**
     * 准备
     * @param dataSource
     * @param isOnlyMusic
     */
    private native void sxPrepared(String dataSource, Surface surface, boolean isOnlyMusic);

    /**
     * 开始
     */
    private native void sxStart();

    /**
     * 停止并释放资源
     * @param exit
     */
    private native void sxStop(boolean exit);

    /**
     * 暂停
     */
    private native void sxPause();

    /**
     * 恢复播放
     */
    private native void sxResume();

    /**
     * seek
     * @param secds
     */
    private native void sxSeek(int secds);

    /**
     * 设置音轨，根据获取的音轨数 排序
     * @param index
     */
    private native void sxSetAudioChannels(int index);

    /**
     * 获取总时长
     * @return
     */
    private native int sxGetDuration();

    /**
     * 获取音轨数
     * @return
     */
    private native int sxGetAudioChannels();

    /**
     * 获取视频通道
     * @return
     */
    private native int sxGetVideoChannels();

    /**
     * 获取视频宽度
     */
    private native int sxGetVideoWidth();

    /**
     * 获取视频长度
     * @return
     */
    private native int sxGetVideoHeight();

    public int getDuration()
    {
        return sxGetDuration();
    }

    public int getVideoChannels()
    {
        return sxGetVideoChannels();
    }

    public int getAudioChannels()
    {
        return sxGetAudioChannels();
    }

    public int getVideoWidth()
    {
        return sxGetVideoWidth();
    }

    public int getVideoHeight()
    {
        return sxGetVideoHeight();
    }

    public void setAudioChannels(int index)
    {
        sxSetAudioChannels(index);
    }


    public void setOnPreparedListener(SXOnPreparedListener preparedListener) {
        this.onPreparedListener = preparedListener;
    }



    public void setOnErrorListener(SXOnErrorListener errorListener) {
        this.onErrorListener = errorListener;
    }

    public void prepared() {
        if (TextUtils.isEmpty(dataSource)) {
            onError(SX_STATUS_DATASOURCE_NULL, "datasource is null");
            return;
        }

        parpared = true;
        if(isOnlyMusic)
        {
            //sxPrepared(dataSource, isOnlyMusic);
        }
        else
        {
            if(surface != null)
            {
                sxPrepared(dataSource, surface, isOnlyMusic);
            }
        }
    }

    public void start()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                if(TextUtils.isEmpty(dataSource))
                {
                    onError(SX_STATUS_DATASOURCE_NULL, "datasource is null");
                    return;
                }
                if(!isOnlyMusic)
                {
                    if(surface == null)
                    {
                        onError(SX_STATUS_SURFACE_NULL, "surface is null");
                        return;
                    }
                }

                if(timeBean == null)
                {
                    timeBean = new SXTimeBean();
                }
                sxStart();
            }
        }).start();
    }

    private void onLoad(boolean load)
    {
        if(onLoadListener != null)
        {
            onLoadListener.onLoad(load);
        }
    }

    private void onError(int code, String msg)
    {
        if(onErrorListener != null)
        {
            onErrorListener.onError(code, msg);
        }
        stop(true);
    }

    private void onParpared()
    {
        if(onPreparedListener != null)
        {
            onPreparedListener.onPrepared();
        }
    }

    public void mediacodecInit(int mimetype, int width, int height,
                               byte[] csd0, byte[] csd1) {
        if (surface != null) {
            sxSurfaceView.setCodecType(1);
            String mtype = getMimeType(mimetype);
            mediaFormat = MediaFormat.createVideoFormat(mtype, width, height);
            mediaFormat.setInteger(MediaFormat.KEY_WIDTH, width);
            mediaFormat.setInteger(MediaFormat.KEY_HEIGHT, height);
            mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
            mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd0));
            mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd1));

            Log.d(LogTag.TAG, "mediacodecInit:" + mediaFormat.toString());
            try {
                mediaCodec = MediaCodec.createDecoderByType(mtype);
                mediaCodec.configure(mediaFormat,surface,null, 0);
                mediaCodec.start();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            if(onErrorListener != null)
            {
                onErrorListener.onError(SX_STATUS_SURFACE_NULL, "surface is null");
            }
        }
    }

    private String getMimeType(int mimetype) {
        String type = "";
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

    public void mediacodecDecode(byte[] bytes, int size, int pts) {
        Log.i("LISHIXING","mediacodecDecode size " + size);
        if (bytes != null && mediaCodec != null && bufferInfo != null) {
            try {
                int inputBufferIndex = mediaCodec.dequeueInputBuffer(10);
                if (inputBufferIndex >= 0) {
                    ByteBuffer buffer = mediaCodec.getInputBuffer(inputBufferIndex);
                    buffer.clear();
                    buffer.put(bytes);
                    mediaCodec.queueInputBuffer(inputBufferIndex, 0, size, pts, 0);
                }

                int index = mediaCodec.dequeueOutputBuffer(bufferInfo, 10);
                while (index >= 0) {
                    mediaCodec.releaseOutputBuffer(index, true);
                    index = mediaCodec.dequeueOutputBuffer(bufferInfo, 10);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void stop(final boolean exit)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                sxStop(exit);
                if(mediaCodec != null)
                {
                    try
                    {
                        mediaCodec.flush();
                        mediaCodec.stop();
                        mediaCodec.release();
                    }
                    catch (Exception e)
                    {
                        e.printStackTrace();
                    }
                    mediaCodec = null;
                    mediaFormat = null;
                }
                if(sxSurfaceView != null)
                {
                    sxSurfaceView.setCodecType(-1);
                    sxSurfaceView.requestRender();
                }

            }
        }).start();
    }

    public void pause()
    {
        sxPause();

    }

    public void resume()
    {
        sxResume();
    }

    public void seek(final int secds)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                sxSeek(secds);
                lastCurrTime = secds;
            }
        }).start();
    }

    public void setOnLoadListener(SXOnLoadListener onLoadListener) {
        this.onLoadListener = onLoadListener;
    }

    public void setOnInfoListener(SXOnInfoListener listener) {
        this.onInfoListener = listener;
    }

    public void setVideoInfo(int currt_secd, int total_secd)
    {
        if(onInfoListener != null && timeBean != null)
        {
            if(currt_secd < lastCurrTime)
            {
                currt_secd = lastCurrTime;
            }
            timeBean.setCurrt_secds(currt_secd);
            timeBean.setTotal_secds(total_secd);
            onInfoListener.onInfo(timeBean);
            lastCurrTime = currt_secd;
        }
    }

    public void setOnCompleteListener(SXOnCompleteListener listener) {
        this.onCompleteListener = listener;
    }

    public void videoComplete()
    {
        if(onCompleteListener != null)
        {
            setVideoInfo(sxGetDuration(), sxGetDuration());
            timeBean = null;
            onCompleteListener.onComplete();
        }
    }

    public void setCutVideoImgListener(SXOnCutVideoImgListener listener) {
        this.onCutVideoImgListener = listener;
    }

    public void cutVideoImg()
    {
        if(sxSurfaceView != null)
        {
            sxSurfaceView.cutVideoImg();
        }
    }

    public void setOnStopListener(SXOnStopListener stopListener) {
        this.onStopListener = stopListener;
    }

    public void onStopComplete()
    {
        if(onStopListener != null)
        {
            onStopListener.onStop();
        }
    }

    public void setFrameData(int w, int h, byte[] y, byte[] u, byte[] v)
    {
        if(sxSurfaceView != null)
        {
            Log.i(LogTag.TAG,"setFrameData");
            sxSurfaceView.setCodecType(0);
            sxSurfaceView.setFrameData(w, h, y, u, v);
        }
    }

    public void setOnlySoft(boolean soft)
    {
        this.isOnlySoft = soft;
    }

    public boolean isOnlySoft()
    {
        return isOnlySoft;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native static String stringFromJNI();
}

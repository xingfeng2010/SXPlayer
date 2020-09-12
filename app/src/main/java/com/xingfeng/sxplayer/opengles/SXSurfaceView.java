package com.xingfeng.sxplayer.opengles;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.xingfeng.sxplayer.listener.SurfaceViewCreateListener;

public class SXSurfaceView extends GLSurfaceView {

    private SXRender sxRender;
    private SurfaceViewCreateListener mSurfaceViewCreateListener;

    public SXSurfaceView(Context context) {
        this(context, null);
    }

    public SXSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        sxRender = new SXRender(context);
        //设置egl版本为2.0
        setEGLContextClientVersion(2);
        //设置render
        setRenderer(sxRender);
        //设置为手动刷新模式
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        sxRender.setOnRenderRefreshListener(() -> requestRender());
    }

    public void setSurfaceViewCreateListener(SurfaceViewCreateListener surfaceViewCreateListener) {
        if (sxRender != null) {
            sxRender.setSurfaceViewCreateListener(surfaceViewCreateListener);
        }
    }

    public void setCodecType(int type) {
        if (sxRender != null) {
            sxRender.setCodecType(type);
        }
    }


    public void setFrameData(int w, int h, byte[] y, byte[] u, byte[] v) {
        if (sxRender != null) {
            sxRender.setFrameData(w, h, y, u, v);
            requestRender();
        }
    }

    public void cutVideoImg() {
        if (sxRender != null) {
            sxRender.cutVideoImg();
            requestRender();
        }
    }
}

package com.xingfeng.sxplayer.opengles;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;

import com.xingfeng.sxplayer.listener.RenderRefreshListener;
import com.xingfeng.sxplayer.listener.SurfaceViewCreateListener;

import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class SXRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener{

    private Context context;
    private FloatBuffer vertexBuffer;
    private final float[] vertexData = {
            1f,1f,0f,
            -1f,1f,0f,
            1f,-1f,0f,
            -1f,-1f,0f
    };

    private FloatBuffer textureBuffer;
    private final float[] textureVertexData = {
            1f,0f,
            0f,0f,
            1f,1f,
            0f,1f
    };

    public SXRender(Context context) {

    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {

    }

    public void setOnRenderRefreshListener(RenderRefreshListener renderRefreshListener) {
    }

    public void setSurfaceViewCreateListener(SurfaceViewCreateListener surfaceViewCreateListener) {
    }

    public void setCodecType(int type) {
    }

    public void setFrameData(int w, int h, byte[] y, byte[] u, byte[] v) {
    }

    public void cutVideoImg() {
    }
}

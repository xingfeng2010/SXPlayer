package com.xingfeng.sxplayer.listener;

import android.graphics.Bitmap;
import android.view.Surface;

public interface SurfaceViewCreateListener {
    void onGlSurfaceViewOncreate(Surface surface);

    void onCutVideoImg(Bitmap bitmap);
}

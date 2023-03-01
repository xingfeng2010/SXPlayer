package com.xingfeng.sxplayer.activity;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.xingfeng.sxplayer.R;
import com.xingfeng.sxplayer.opengles.SXSurfaceView;
import com.xingfeng.sxplayer.player.SXPlayer;
import com.xingfeng.sxplayer.player.SXTimeBean;
import com.xingfeng.sxplayer.util.LogTag;
import com.xingfeng.sxplayer.util.SXTimeUtil;

public class VideoLiveActivity2 extends BaseActivity {

    private SurfaceView surfaceview;
    private SXPlayer sxPlayer;
    private ProgressBar progressBar;
    private TextView tvTime;
    private ImageView ivPause;
    private SeekBar seekBar;
    private String pathurl;
    private LinearLayout lyAction;
    private ImageView ivCutImg;
    private ImageView ivShowImg;
    private boolean ispause = false;
    private int position;
    private boolean isSeek = false;
    private String codecType = "mediacodec";

    private ImageButton imageButton;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.video_activity2);
        surfaceview = findViewById(R.id.surfaceview);
        progressBar = findViewById(R.id.pb_loading);
        ivPause = findViewById(R.id.iv_pause);
        tvTime = findViewById(R.id.tv_time);
        seekBar = findViewById(R.id.seekbar);
        lyAction = findViewById(R.id.ly_action);
        ivCutImg = findViewById(R.id.iv_cutimg);
        ivShowImg = findViewById(R.id.iv_show_img);
        imageButton = findViewById(R.id.pb_back);

        sxPlayer = new SXPlayer();
        sxPlayer.setOnlyMusic(false);

        pathurl = getIntent().getExtras().getString("url");
        codecType = getIntent().getExtras().getString("codecType");

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                position = sxPlayer.getDuration() * progress / 100;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                isSeek = true;
                sxPlayer.seek(position);
            }
        });
        sxPlayer.setDataSource(pathurl);
//        if (codecType.equalsIgnoreCase("ffmpeg")) {
//            sxPlayer.setOnlySoft(true);
//            surfaceview.setCodecType(0);
//        } else {
//            sxPlayer.setOnlySoft(false);
//            surfaceview.setCodecType(1);
//        }

        SurfaceHolder surfaceHolder = surfaceview.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                sxPlayer.setSurface(holder.getSurface());

                sxPlayer.setOnErrorListener((code, msg) -> {
                    Log.d(LogTag.TAG,"code:" + code + ",msg:" + msg);
                    Message message = Message.obtain();
                    message.obj = msg;
                    message.what = 3;
                    handler.sendMessage(message);
                });

                sxPlayer.setOnPreparedListener(() -> {
                    Log.d(LogTag.TAG,"starting......");
                    sxPlayer.start();
                    int audioChannels = sxPlayer.getAudioChannels();
                    int videoChannels = sxPlayer.getVideoChannels();
                    Log.d(LogTag.TAG,"audioChannels:" + audioChannels + "  videoChannels:" + videoChannels);
                });

                sxPlayer.setOnLoadListener(load -> {
                    Log.d(LogTag.TAG,"loading......>>" + load);
                    Message message = Message.obtain();
                    message.what = 1;
                    message.obj = load;
                    handler.sendMessage(message);
                });

                sxPlayer.setOnInfoListener(wlTimeBean -> {
                    Message message = Message.obtain();
                    message.what = 2;
                    message.obj = wlTimeBean;
                    Log.d(LogTag.TAG,"nowTime is " +wlTimeBean.getCurrt_secds());
                    handler.sendMessage(message);

                    //surfaceview.requestRender();
                });

                sxPlayer.setOnCompleteListener(() -> {
                    Log.d(LogTag.TAG,"complete .....................");
                    sxPlayer.stop(true);
                });

                sxPlayer.setCutVideoImgListener(bitmap -> {
                    Message message = Message.obtain();
                    message.what = 4;
                    message.obj = bitmap;
                    handler.sendMessage(message);
                });

                sxPlayer.prepared();
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if(sxPlayer != null) {
            sxPlayer.stop(true);
            sxPlayer = null;
        }
    }


    @SuppressLint("HandlerLeak")
    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if(msg.what == 1)
            {
                boolean load = (boolean) msg.obj;
                if(load)
                {
                    progressBar.setVisibility(View.VISIBLE);
                    imageButton.setVisibility(View.INVISIBLE);
                }
                else
                {
                    if(lyAction.getVisibility() != View.VISIBLE)
                    {
                        lyAction.setVisibility(View.VISIBLE);
                        ivCutImg.setVisibility(View.VISIBLE);
                    }
                    imageButton.setVisibility(View.VISIBLE);
                    progressBar.setVisibility(View.GONE);
                }
            }
            else if(msg.what == 2)
            {
                SXTimeBean wlTimeBean = (SXTimeBean) msg.obj;
                if(wlTimeBean.getTotal_secds() > 0)
                {
                    seekBar.setVisibility(View.VISIBLE);
                    if(isSeek)
                    {
                        seekBar.setProgress(position * 100 / wlTimeBean.getTotal_secds());
                        isSeek = false;
                    }
                    else
                    {
                        seekBar.setProgress(wlTimeBean.getCurrt_secds() * 100 / wlTimeBean.getTotal_secds());
                    }
                    tvTime.setText(SXTimeUtil.secdsToDateFormat(wlTimeBean.getTotal_secds()) + "/" + SXTimeUtil.secdsToDateFormat(wlTimeBean.getCurrt_secds()));
                }
                else
                {
                    seekBar.setVisibility(View.GONE);
                    tvTime.setText(SXTimeUtil.secdsToDateFormat(wlTimeBean.getCurrt_secds()));
                }
            }
            else if(msg.what == 3)
            {
                String err = (String) msg.obj;
                Toast.makeText(VideoLiveActivity2.this, err, Toast.LENGTH_SHORT).show();
            }
            else if(msg.what == 4)
            {
                Bitmap bitmap = (Bitmap) msg.obj;
                if(bitmap != null)
                {
                    ivShowImg.setVisibility(View.VISIBLE);
                    ivShowImg.setImageBitmap(bitmap);
                }
            }
        }
    };

    public void pause(View view) {
        if(sxPlayer != null)
        {
            ispause = !ispause;
            if(ispause)
            {
                sxPlayer.pause();
                ivPause.setImageResource(R.mipmap.ic_play_play);
            }
            else
            {
                sxPlayer.resume();
                ivPause.setImageResource(R.mipmap.ic_play_pause);
            }
        }

    }

    public void cutImg(View view) {
        if(sxPlayer != null)
        {
            sxPlayer.cutVideoImg();
        }
    }

    public void closeActivity(View view) {
        VideoLiveActivity2.this.finish();
    }
}

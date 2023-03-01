package com.xingfeng.sxplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.xingfeng.sxplayer.activity.VideoLiveActivity;
import com.xingfeng.sxplayer.activity.VideoLiveActivity2;
import com.xingfeng.sxplayer.player.SXPlayer;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.

    String liveUrl = "rtmp://58.200.131.2:1935/livetv/hunantv";
//    String liveUrl2 = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";
    String liveUrl2 = "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear3/prog_index.m3u8";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
//        TextView tv = findViewById(R.id.sample_text);
//        tv.setText(SXPlayer.stringFromJNI());
        requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
    }

    public void startPlayFFMpeg(View view) {
        Intent intent = new Intent(this, VideoLiveActivity.class);
        intent.putExtra("url", liveUrl2);
        intent.putExtra("codecType", "ffmpeg");
        startActivity(intent);
    }

    public void startPlayMediaCodec(View view) {
        Intent intent = new Intent(this, VideoLiveActivity.class);
        intent.putExtra("url", liveUrl2);
        intent.putExtra("codecType", "mediacodec");
        startActivity(intent);
    }

    public void startANativeWindowBuffer(View view) {
        Intent intent = new Intent(this, VideoLiveActivity2.class);
        intent.putExtra("url", liveUrl2);
        intent.putExtra("codecType", "mediacodec");
        startActivity(intent);
    }
}

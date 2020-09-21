package com.xingfeng.sxplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.xingfeng.sxplayer.activity.VideoLiveActivity;
import com.xingfeng.sxplayer.player.SXPlayer;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(SXPlayer.stringFromJNI());

    }

    public void startPlay(View view) {
        Intent intent = new Intent(this, VideoLiveActivity.class);
        //String pathurl = "http://pl28.live.panda.tv/live_panda/dd9f182bcec99d04099113e618cfc5b3_mid.flv?sign=1153e03b4fff24bf5eb2c311c871a423&time=&ts=5a461efe&rid=-465228";
        intent.putExtra("url", "rtmp://58.200.131.2:1935/livetv/hunantv");
        startActivity(intent);
    }
}

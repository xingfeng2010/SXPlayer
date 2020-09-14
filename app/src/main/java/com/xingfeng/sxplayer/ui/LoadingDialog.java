package com.xingfeng.sxplayer.ui;

import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.widget.TextView;


import com.xingfeng.sxplayer.R;

import butterknife.BindView;


/**
 * Created by ywl5320 on 2017/9/5.
 */

public class LoadingDialog extends BaseDialog {

    @BindView(R.id.tv_msg)
    TextView tvMsg;

    public LoadingDialog(Context context) {
        super(context);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.loading_dialog);
    }

    public void setMsg(String msg)
    {
        if(tvMsg != null && !TextUtils.isEmpty(msg))
        {
            tvMsg.setText(msg);
        }
    }
}

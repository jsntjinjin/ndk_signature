package com.fastaoe.ndkdemo1;

import android.app.Application;

/**
 * Created by jinjin on 18/1/30.
 * description:
 */

public class BaseApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        SignatureUtil.signatureVerify(this);
    }
}

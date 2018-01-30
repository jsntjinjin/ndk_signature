package com.fastaoe.ndkdemo1;

import android.content.Context;

/**
 * Created by jinjin on 18/1/30.
 * description:
 */

public class SignatureUtil {

    static {
        System.loadLibrary("native-lib");
    }

    public static native String signatureParams(String text);

    public static native void signatureVerify(Context context);
}

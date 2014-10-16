package com.easydlna.application;

import java.lang.Thread.UncaughtExceptionHandler;

import android.app.Application;
import android.util.Log;

public class App extends Application {
    private static String TAG = "easydlna-app";

    private class EasyshareUncaughtExceptionHandler implements
            UncaughtExceptionHandler {
        private boolean mCrashing = false;

        @Override
        public void uncaughtException(Thread t, Throwable e) {
            try {
                // Don't re-enter -- avoid infinite loops if crash-reporting
                // crashes.
                if (mCrashing)
                    return;
                mCrashing = true;

                Log.e(TAG, "FATAL EXCEPTION: " + t.getName(), e);
            } catch (Throwable t2) {
                try {
                    Log.e(TAG, "Error reporting crash", t2);
                } catch (Throwable t3) {
                    // Even Slog.e() fails! Oh well.
                }
            } finally {
                Log.e(TAG, "before we quit, kill easyDlna");
                EasyDlnaUtil.killEasyDlna();
            }
        }
    }

    @Override
    public void onCreate() {
        new EasyDlnaUtil();
        super.onCreate();
        Thread.setDefaultUncaughtExceptionHandler(new EasyshareUncaughtExceptionHandler());
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        EasyDlnaUtil.killEasyDlna();
    }
}

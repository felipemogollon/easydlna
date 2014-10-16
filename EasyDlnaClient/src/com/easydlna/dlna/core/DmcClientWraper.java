package com.easydlna.dlna.core;

import android.content.Context;
import com.easydlna.dlna.dlnaclient.DMCClient;
import com.easydlna.dlna.service.DlnaDevice;
import com.easydlna.dlna.service.RenderStatus;

public class DmcClientWraper implements DMCClient.DMCCallback {

    public interface IDmcBrowserClient {
        public void DMCServiceStatusNotify(int status);

        public void DlnaDeviceStatusNotify(DlnaDevice device);

        public void DlnaFilesNotify(String udn, int VideoCounts,
                int AudioCounts, int ImageCounts);
    }

    public interface IDmcControllerClient {
        public void DlnaPlayStatusNotify(String arg0, RenderStatus arg1);
    }

    public static DMCClient sClient;

    private static DmcClientWraper sInstance = new DmcClientWraper();

    public static void startDMC(Context context) {
        sClient = new DMCClient(context);
        sClient.startDMC(sInstance);
    }

    public static void stopDMC() {
        sClient.stopDMC();
    }

    private IDmcBrowserClient mBrowserClient = null;

    public static void setBrowserClient(IDmcBrowserClient browserClient) {
        sInstance.mBrowserClient = browserClient;
    }

    private IDmcControllerClient mControllerClient = null;

    public static void setControllerClient(IDmcControllerClient controllerClient) {
        sInstance.mControllerClient = controllerClient;
    }

    private DmcClientWraper() {
    }

    public void DMCServiceStatusNotify(int status) {
        if (mBrowserClient != null) {
            mBrowserClient.DMCServiceStatusNotify(status);
        }
    }

    public void DlnaDeviceStatusNotify(DlnaDevice device) {
        if (mBrowserClient != null) {
            mBrowserClient.DlnaDeviceStatusNotify(device);
        }
    }

    public void DlnaFilesNotify(String udn, int VideoCounts, int AudioCounts,
            int ImageCounts) {
        if (mBrowserClient != null) {
            mBrowserClient.DlnaFilesNotify(udn, VideoCounts, AudioCounts,
                    ImageCounts);
        }
    }

    public void DlnaPlayStatusNotify(String arg0, RenderStatus arg1) {
        if (mControllerClient != null) {
            mControllerClient.DlnaPlayStatusNotify(arg0, arg1);
        }
    }
}

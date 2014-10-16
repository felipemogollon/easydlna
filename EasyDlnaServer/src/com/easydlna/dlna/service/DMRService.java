package com.easydlna.dlna.service;

import com.easydlna.application.EasyDlnaUtil;
import com.easydlna.dlna.service.IDMRCallback;
import com.easydlna.dlna.service.IDMRService;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class DMRService extends Service {

    public final static String DMR_AUTOSTART_KEY = "DMRAutoStart";
    private final static String DMRSER_STRING = "DMRService";
    private final static String DMR_NAME_KEY = "DMR Friendly Name";
    private final static String DMR_NAME_VALUE = "Easydlna Media Renderer";

    private final static String RENDER_ACTIVITY_NAME_STRING = "com.easydlna.video";

    private boolean bAutoStartup = false;
    private IDMRServiceImpl dmrServiceImpl = null;
    private DMRObserver dmrObserver = null;
    volatile IDMRCallback outDMRCallback = null;
    private String renderNameString = null;

    private interface IDMRObserver {
        public int setDataSource(String urlSource);

        public int stop();

        public int play();

        public int pause();

        public int seek(int seekTo);

        public int getPosition();
    }

    private class DMRObserver implements IDMRObserver {
        public int setDataSource(String urlSource) {
            try {
                if (outDMRCallback != null) {
                    return outDMRCallback.setDataSource(urlSource);
                }
            } catch (RemoteException e) {
                return -1;
            }

            /*
             * Context context = getApplicationContext();
             * context.startActivity(new Intent(RENDER_ACTIVITY_NAME_STRING));
             * synchronized (this) { try { wait(5000); } catch (Exception e) {
             * Log.e(DMRSER_STRING, "Fail to start Render"); return -1; } }
             */

            if (outDMRCallback == null) {
                return -1;
            } else {
                try {
                    return outDMRCallback.setDataSource(urlSource);
                } catch (Exception e) {
                    return -1;
                }
            }
        }

        public int stop() {
            try {
                if (outDMRCallback != null) {
                    return outDMRCallback.stop();
                } else {
                    return -1;
                }
            } catch (RemoteException e) {
                return -1;
            }
        }

        public int play() {
            try {
                if (outDMRCallback != null) {
                    return outDMRCallback.play();
                }
                return -1;
            } catch (RemoteException e) {
                return -1;
            }
        }

        public int pause() {
            try {
                if (outDMRCallback != null) {
                    return outDMRCallback.pause();
                }
                return -1;
            } catch (RemoteException e) {
                return -1;
            }
        }

        public int seek(int seekTo) {
            try {
                if (outDMRCallback != null) {
                    return outDMRCallback.seek(seekTo);
                }
                return -1;
            } catch (RemoteException e) {
                return -1;
            }
        }

        public int getPosition() {
            try {
                if (outDMRCallback != null) {
                    return outDMRCallback.getPosition();
                }
                return 0;
            } catch (RemoteException e) {
                return 0;
            }
        }
    }

    private class IDMRServiceImpl extends IDMRService.Stub {
        public void addDMRCallback(IDMRCallback callback)
                throws RemoteException {
            if (callback != null) {
                outDMRCallback = callback;
            }
        }

        public void removeDMRCallback(IDMRCallback callback)
                throws RemoteException {
            outDMRCallback = null;
        }

        public int setRenderFriendlyName(String name, boolean reStartRender)
                throws RemoteException {
            if (name == null)
                return -1;
            if (renderNameString.equals(name))
                return 0;
            renderNameString = name;
            Context context = getApplicationContext();
            if (context != null) {
                SharedPreferences preferences = context.getSharedPreferences(
                        EasyDlnaUtil.TAG_STRING, 0);
                Editor editor = preferences.edit();
                editor.putString(DMR_NAME_KEY, renderNameString);
                editor.commit();
            }
            native_set_DMRFriendlyName(renderNameString);
            if (reStartRender) {
                native_start_stop_DMR(false);
                native_start_stop_DMR(true);
            }
            return 0;
        }

        public int setRenderStatus(RenderStatus renderStatus)
                throws RemoteException {
            native_send_RenderStatus(renderStatus);
            return 0;
        }

        public void start_stop_DMR(boolean bStart) {
            int nret = native_start_stop_DMR(bStart);
            if (nret == 0) {
                if (bStart) {
                    if (renderNameString != null) {
                        native_set_DMRFriendlyName(renderNameString);
                    }
                }
                if (!bStart && dmrObserver != null) {
                    native_add_remove_DMRObserver(dmrObserver, bStart);
                    dmrObserver = null;
                } else if (bStart) {
                    if (dmrObserver == null) {
                        dmrObserver = new DMRObserver();
                        native_add_remove_DMRObserver(dmrObserver, true);
                    }
                }
            }
        }

        public String getRenderFriendlyName() {
            return renderNameString;
        }

        public boolean setAutoStartup(boolean bAutoStartup)
                throws RemoteException {
            boolean bOldValue = DMRService.this.bAutoStartup;
            DMRService.this.bAutoStartup = bAutoStartup;

            Context context = getApplicationContext();
            Editor editor = null;

            if (context != null) {
                SharedPreferences preferences = context.getSharedPreferences(
                        EasyDlnaUtil.TAG_STRING, 0);
                editor = preferences.edit();
            }
            if (editor != null) {
                editor.putBoolean(DMR_AUTOSTART_KEY, bAutoStartup);
                editor.commit();
            }

            return bAutoStartup;
        }

        @Override
        public void sendBroadcast(int timeout) throws RemoteException {
            native_sendBroadcast(timeout);
        }
    }

    public void onCreate() {
        super.onCreate();
        if (dmrServiceImpl == null) {
            startUp();
        }
    }

    public void onDestroy() {
        if (dmrServiceImpl != null) {
            cleanUp();
            Log.i(DMRSER_STRING, "DMR Service onDestroy");
        }
        super.onDestroy();
    }

    public boolean onUnbind(Intent intent) {
        // TODO_TASK Auto-generated method stub
        return super.onUnbind(intent);
    }

    public IBinder onBind(Intent intent) {
        if (dmrServiceImpl == null) {
            startUp();
        }
        return dmrServiceImpl;
    }

    private void startUp() {
        if (EasyDlnaUtil.startDLNA(null) == 0) {
            dmrServiceImpl = new IDMRServiceImpl();
            renderNameString = getRenderNameFromPreferences();
            dmrServiceImpl.start_stop_DMR(true);
            Log.i(DMRSER_STRING, "DMR Service onStart");
        }
    }

    private String getRenderNameFromPreferences() {
        Context context = getApplicationContext();
        if (context != null) {
            SharedPreferences preferences = context.getSharedPreferences(
                    EasyDlnaUtil.TAG_STRING, 0);
            return preferences.getString(DMR_NAME_KEY, DMR_NAME_VALUE);
        }
        return null;
    }

    private void cleanUp() {
        dmrServiceImpl.start_stop_DMR(false);
        dmrServiceImpl = null;
        EasyDlnaUtil.stopDLNA(null);
    }

    private native void native_sendBroadcast(int timeOut);

    private native void native_add_remove_DMRObserver(IDMRObserver observer,
            boolean bAdd);

    private native int native_start_stop_DMR(boolean bStart);

    private native void native_set_DMRFriendlyName(String name);

    private native void native_send_RenderStatus(RenderStatus renderStatus);
}

package com.easydlna.dlna.service;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import java.util.ArrayList;
import java.util.List;

import com.easydlna.application.EasyDlnaUtil;
import com.easydlna.dlna.service.IDMCCallback;
import com.easydlna.dlna.service.IDMCService;

public class DMCService extends Service {
    private final static String TAG = "DMCService";

    public final static String DMC_AUTOSTART_KEY = "DMCAutoStart";
    private final static String DMCSER_STRING = "DMCService";

    private boolean bAutoStartup = false;
    private IDMCServiceImpl dmcServiceImpl = null;
    private DMCObserver dmcObserver = null;
    private ArrayList<IDMCCallback> dmcCallbacksList = new ArrayList<IDMCCallback>();

    private static String vedioPatternString = "http-get:\\*:video([\\s\\S]*)";
    private static String audioPattern = "http-get:\\*:audio([\\s\\S]*)";
    private static String imagePattern = "http-get:\\*:image([\\s\\S]*)";

    private static class AVTransportOpr {
        static int AVT_SETDATASOURCE = 0;
        static int AVT_PLAY = 1;
        static int AVT_STOP = 2;
        static int AVT_PAUSE = 3;
        static int AVT_SEEK = 4;
        static int AVT_GETPOS = 5;
        static int AVT_UNKNOWN = 6;
    }

    private class DlnaDeviceEx {
        public DlnaDevice device = null;

        // DMS info
        public String cdsActionUrl = null;
        public ArrayList<ContentInfo> videoContents = null;
        public ArrayList<ContentInfo> audioContents = null;
        public ArrayList<ContentInfo> imageContents = null;
        private int nWaterMarkAudio = 3;
        private int nWaterMarkVedio = 3;
        private int nLastAudioCounts = 0;
        private int nLastVedioCounts = 0;
        private Thread fetchThread = null;

        // DMR info
        public String avtActionUrl = null;

        volatile private boolean bquit = false;

        RenderStatus dmrStatus = null;

        DlnaDeviceEx(DlnaDevice device) {
            this.device = device;
            if (device.type == DlnaDevice.DEVICE_TYPE_DMS) {
                cdsActionUrl = native_getDeviceActionUrl(device.udnString,
                        DlnaDevice.CDS_TYPEString);
                videoContents = new ArrayList<ContentInfo>();
                audioContents = new ArrayList<ContentInfo>();
                imageContents = new ArrayList<ContentInfo>();
            } else if (device.type == DlnaDevice.DEVICE_TYPE_DMR) {
                avtActionUrl = native_getDeviceActionUrl(device.udnString,
                        DlnaDevice.AVT_TYPEString);
                dmrStatus = new RenderStatus();
            }
        }

        boolean hitThisDevice(String udn) {
            if (device == null)
                return false;
            return device.udnString.compareTo(udn) == 0;
        }

        void update(String url, String state, String status, String duration) {
            if (url.length() != 0) {
                dmrStatus.urlString = url;
            }
            if (state.length() != 0) {
                dmrStatus.stateString = state;
                if (state.equals("STOPPED")) {
                    dmrStatus.state = RenderStatus.STATE_STOPPED;
                } else if (state.equals("PLAYING")) {
                    dmrStatus.state = RenderStatus.STATE_PLAYING;
                } else if (state.equals("PAUSED_PLAYBACK")) {
                    dmrStatus.state = RenderStatus.STATE_PAUSE_PLAYBACK;
                } else if (state.equals("NO_MEDIA_PRESENT")) {
                    dmrStatus.state = RenderStatus.STATE_NO_MEDIA_PRESENT;
                }
            }
            if (status.length() != 0) {
                dmrStatus.statusString = status;
                if (status.equals("OK"))
                    dmrStatus.errcode = 0;
                else
                    dmrStatus.errcode = -1;
            }
            if (duration.length() != 0
                    && duration.equals("NOT_IMPLEMENTED") != true) {
                dmrStatus.durationString = duration.trim();
                String time[] = dmrStatus.durationString.split(":");
                dmrStatus.duration = 0;
                dmrStatus.duration += new Integer(time[0].trim()).intValue() * 3600;
                dmrStatus.duration += new Integer(time[1].trim()).intValue() * 60;
                dmrStatus.duration += new Integer(time[2].trim()).intValue();
            }
        }

        void update(DlnaDevice dlnaDevice) {
            device.stateNow = dlnaDevice.stateNow;
            device.baseUrlString = dlnaDevice.baseUrlString;
        }

        void refreshContents() {
            stopFetchInfo();
            destroyResources(false);
            bquit = false;
            startFetchInfo(nWaterMarkVedio, nWaterMarkAudio);
        }

        void destroyResources(boolean forceDestroy) {
            if (forceDestroy) {
                device = null;
            }

            synchronized (videoContents) {
                for (ContentInfo info : videoContents) {
                    info = null;
                }
            }
            if (forceDestroy) {
                videoContents = null;
            } else {
                videoContents.clear();
            }

            synchronized (audioContents) {
                for (ContentInfo info : audioContents) {
                    info = null;
                }
            }
            if (forceDestroy) {
                audioContents = null;
            } else {
                audioContents.clear();
            }

            synchronized (imageContents) {
                for (ContentInfo info : imageContents) {
                    info = null;
                }
            }
            if (forceDestroy) {
                imageContents = null;
            } else {
                imageContents.clear();
            }
        }

        private int browseDirectory(ContentInfo info,
                ArrayList<ContentInfo> arry) {

            int nindex = 0;
            int nMaxCount = 5 > info.childCount ? info.childCount : 5;
            int nLeft = info.childCount;
            while (nindex < info.childCount && !bquit) {
                ContentInfo outInfo[] = null;

                // Log.i("CDlnaCDService", "browse device " + strDevice +
                // "directory info of id = " + info.myId +
                // "[startindex:maxcount]" + "[" + nindex +":" + nMaxCount +
                // "]");

                outInfo = native_browseChildren(cdsActionUrl, info.id, nindex,
                        nMaxCount);
                if (outInfo == null) {
                    nLeft -= nMaxCount;
                    nindex += nMaxCount;
                    nMaxCount = nLeft > 5 ? 5 : nLeft;
                    continue;
                }
                for (int i = 0; i < outInfo.length; i++) {
                    ContentInfo outInfoItem = outInfo[i];
                    if (outInfoItem.childCount > 0) {
                        // Log.i("CDlnaCDService", "DIR of " + strDevice +
                        // "title = " + outMetaInfo.title);
                        arry.add(outInfoItem);
                    } else if (outInfoItem.childCount == -1) {
                        // Log.i("CDlnaCDService", "metainfo of " + strDevice +
                        // "title = " + outMetaInfo.title);
                        parseMetaDataInfo(outInfoItem);
                    }
                    outInfo[i] = null;
                    outInfoItem = null;
                }

                nLeft -= nMaxCount;
                nindex += nMaxCount;
                nMaxCount = nLeft > 5 ? 5 : nLeft;
                outInfo = null;
            }

            return nindex;
        }

        private void parseMetaDataInfo(ContentInfo info) {
            if (info.protocalInfo != null) {
                int vedioCounts = 0;
                int audioCounts = 0;
                int imageCounts = 0;
                if (info.protocalInfo.matches(audioPattern)) {
                    synchronized (audioContents) {
                        audioContents.add(info);
                        audioCounts = audioContents.size();
                    }
                } else if (info.protocalInfo.matches(vedioPatternString)) {
                    synchronized (videoContents) {
                        videoContents.add(info);
                        audioCounts = videoContents.size();
                    }
                } else if (info.protocalInfo.matches(imagePattern)) {
                    synchronized (imageContents) {
                        imageContents.add(info);
                        imageCounts = imageContents.size();
                    }
                }

                if (audioCounts - nLastAudioCounts >= nWaterMarkAudio
                        || vedioCounts - nLastVedioCounts >= nWaterMarkVedio) {

                    NotfiyCallbacks(this, vedioCounts, audioCounts, imageCounts);

                    if (audioCounts - nLastAudioCounts >= nWaterMarkAudio)
                        nLastAudioCounts = audioCounts;
                    if (vedioCounts - nLastVedioCounts >= nWaterMarkVedio)
                        nLastVedioCounts = vedioCounts;
                }

            }

            return;
        }

        public void startFetchInfo(int nwaterMarkVideo, int nwaterMarkAudio) {
            this.nWaterMarkVedio = nwaterMarkVideo;
            this.nWaterMarkAudio = nwaterMarkAudio;

            fetchThread = new Thread(new Runnable() {
                public void run() {
                    ArrayList<ContentInfo> tempArry = new ArrayList<ContentInfo>();
                    ContentInfo initInfo = new ContentInfo();

                    int nret = native_browseMetaData(cdsActionUrl, "0",
                            initInfo);
                    if (nret != 0) {
                        initInfo = null;
                        // Log.i("DlnaCDService", "device" + strDevice +
                        // " browse metada fails\n");
                        return;
                    }

                    tempArry.add(initInfo);
                    while (!bquit && tempArry.size() != 0) {
                        ContentInfo info = tempArry.get(0);
                        tempArry.remove(0);

                        if (info.childCount > 0) {
                            browseDirectory(info, tempArry);
                        } else if (info.childCount == -1) {
                            parseMetaDataInfo(info);
                        }

                        info = null;
                    }

                    NotfiyCallbacks(DlnaDeviceEx.this, videoContents.size(),
                            audioContents.size(), imageContents.size());

                    tempArry = null;
                }

            });

            fetchThread.start();
        }

        public void stopFetchInfo() {
            bquit = true;
            try {
                if (fetchThread != null) {
                    fetchThread.join();
                    fetchThread = null;
                }

                nWaterMarkAudio = nWaterMarkVedio = 3;
                nLastAudioCounts = nLastVedioCounts = 0;
            } catch (Exception e) {
                e.getMessage();
            }
            // Log.i("DlnaCDService", "device" + strDevice + " stop fetch\n");
        }
    }

    private ArrayList<DlnaDeviceEx> dmsDevicesList = null;
    private ArrayList<DlnaDeviceEx> dmrDeviceList = null;

    private native String native_getDeviceActionUrl(String udn,
            String actionType);

    private native int native_start_stop_DMC(boolean bStart);

    private native void native_add_remove_DMCObserver(
            IDMCObserver IDMCObserver, boolean bAdd);

    private native ContentInfo[] native_browseChildren(String cdsActionUrl,
            String id, int nindex, int nMaxCount);

    private native int native_browseMetaData(String cdsActionUrl, String id,
            ContentInfo initInfo);

    private native void native_searchNetwork(int type);

    private native int native_playControl(String avtActionUrl, int type,
            String param);

    private void startUp() {
        if (EasyDlnaUtil.startDLNA(getApplicationContext()) != 0) {
            Log.e(DMCSER_STRING, "start dlna fails");
            return;
        }

        dmcServiceImpl = new IDMCServiceImpl();

        dmsDevicesList = new ArrayList<DlnaDeviceEx>();
        dmrDeviceList = new ArrayList<DlnaDeviceEx>();

        dmcServiceImpl.start_stop_DMC(true);
        Log.i(DMCSER_STRING, "DMCService started");
        return;
    }

    private void cleanUp() {
        synchronized (dmcCallbacksList) {
            for (IDMCCallback iterable_element : dmcCallbacksList) {
                iterable_element = null;
            }
            dmcCallbacksList.clear();
            dmcCallbacksList = null;
        }

        synchronized (dmsDevicesList) {
            for (DlnaDeviceEx iterable_element : dmsDevicesList) {
                iterable_element.stopFetchInfo();
                iterable_element.destroyResources(true);
                iterable_element = null;
            }
            dmsDevicesList.clear();
            dmsDevicesList = null;
        }

        synchronized (dmrDeviceList) {
            for (DlnaDeviceEx iterable_element : dmrDeviceList) {
                iterable_element = null;
            }
            dmrDeviceList.clear();
            dmrDeviceList = null;
        }

        dmcServiceImpl.start_stop_DMC(false);
        dmcServiceImpl = null;
        EasyDlnaUtil.stopDLNA(null);
    }

    private interface IDMCObserver {
        public int Nortify(DlnaDevice dlnaDevice);

        public int AVTStatusChangesNotify(String udn, String urlString,
                String stateString, String statusString, String durationString);
    }

    private class DMCObserver implements IDMCObserver {

        public int Nortify(DlnaDevice dlnaDevice) {
            int bOffLined = -1;

            dlnaDevice.confirmTypes();

            if (dlnaDevice.type == DlnaDevice.DEVICE_TYPE_DMS) {
                Log.i(TAG, "Notify an DMS:" + dlnaDevice.nameString);
                DlnaDeviceEx hitDeviceEx = null;
                synchronized (dmsDevicesList) {
                    for (DlnaDeviceEx existDeivce : dmsDevicesList) {
                        if (existDeivce.hitThisDevice(dlnaDevice.udnString)) {

                            existDeivce.update(dlnaDevice);
                            hitDeviceEx = existDeivce;
                            bOffLined = 0;

                            if (dlnaDevice.stateNow == DlnaDevice.DEVICE_OFFLINE) {
                                Log.i(TAG, "Notify DEVICE_OFFLINE");
                                dmsDevicesList.remove(existDeivce);
                                existDeivce.stopFetchInfo();
                                bOffLined = 1;
                                Intent i = new Intent(
                                        "com.easydlna.CurrentDMSServerDown");
                                sendBroadcast(i);
                                NotifyCallbacks(existDeivce);
                            } else {
                                Log.i(TAG, "Notify not DEVICE_OFFLINE");
                                NotifyCallbacks(existDeivce);
                            }

                            break;
                        }
                    }

                    if (bOffLined == -1) {
                        DlnaDeviceEx newDeviceWithContent = new DlnaDeviceEx(
                                dlnaDevice);
                        dmsDevicesList.add(newDeviceWithContent);
                        hitDeviceEx = newDeviceWithContent;
                    }
                }

                NotifyCallbacks(hitDeviceEx);

                if (bOffLined == -1) {
                    hitDeviceEx.startFetchInfo(3, 3);
                } else if (bOffLined == 1) {
                    hitDeviceEx.destroyResources(true);
                    hitDeviceEx = null;
                }
            } else if (dlnaDevice.type == DlnaDevice.DEVICE_TYPE_DMR) {
                DlnaDeviceEx hitDeviceEx = null;
                synchronized (dmrDeviceList) {
                    for (DlnaDeviceEx existDeivce : dmrDeviceList) {
                        if (existDeivce.hitThisDevice(dlnaDevice.udnString)) {
                            existDeivce.update(dlnaDevice);
                            hitDeviceEx = existDeivce;
                            bOffLined = 0;

                            if (dlnaDevice.stateNow == DlnaDevice.DEVICE_OFFLINE) {
                                dmrDeviceList.remove(existDeivce);
                                bOffLined = 1;
                                NotifyCallbacks(existDeivce);
                            } else {
                                NotifyCallbacks(existDeivce);
                            }

                            break;
                        }
                    }
                    if (bOffLined == -1) {
                        DlnaDeviceEx newDeviceEx = new DlnaDeviceEx(dlnaDevice);
                        dmrDeviceList.add(newDeviceEx);
                        hitDeviceEx = newDeviceEx;
                    }
                }

                NotifyCallbacks(hitDeviceEx);
            }

            return 0;
        }

        /* @Override */
        public int AVTStatusChangesNotify(String udn, String urlString,
                String stateString, String statusString, String durationString) {
            synchronized (dmrDeviceList) {
                for (DlnaDeviceEx dlnaDeviceEx : dmrDeviceList) {
                    if (dlnaDeviceEx.hitThisDevice(udn)) {
                        dlnaDeviceEx.update(urlString, stateString,
                                statusString, durationString);
                        synchronized (dmcCallbacksList) {
                            for (IDMCCallback callback : dmcCallbacksList) {
                                try {
                                    callback.PlayStatusNotify(udn,
                                            dlnaDeviceEx.dmrStatus);
                                } catch (RemoteException e) {
                                    e.printStackTrace();
                                    dmcCallbacksList.clear();
                                    return 0;
                                }
                            }
                        }
                        return 0;
                    }
                }
            }

            return 0;
        }

    }

    private void NotfiyCallbacks(DlnaDeviceEx device, int nVedio, int nAudio,
            int nImage) {
        if (dmcCallbacksList != null) {
            synchronized (dmcCallbacksList) {
                for (IDMCCallback callback : dmcCallbacksList) {
                    try {
                        callback.ContentNotify(device.device.udnString, nVedio,
                                nAudio, nImage);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                        dmcCallbacksList.clear();
                        return;
                    }
                }
            }
        }
        return;
    }

    private void NotifyCallbacks(DlnaDeviceEx device) {

        synchronized (dmcCallbacksList) {
            for (IDMCCallback callback : dmcCallbacksList) {
                try {
                    callback.DeviceNotify(device.device);
                } catch (RemoteException e) {
                    e.printStackTrace();
                    dmcCallbacksList.clear();
                    return;
                }
            }
        }
        return;
    }

    private class IDMCServiceImpl extends IDMCService.Stub {

        public void addDMCCallback(IDMCCallback callback)
                throws RemoteException {
            if (callback == null)
                return;
            synchronized (dmcCallbacksList) {
                dmcCallbacksList.add(callback);
                native_searchNetwork(DlnaDevice.DEVICE_TYPE_UNKNOWN);
            }
            return;
        }

        public void removeDMCCallback(IDMCCallback callback)
                throws RemoteException {

            synchronized (dmcCallbacksList) {
                dmcCallbacksList.clear();
                // dmcCallbacksList.remove(callback);
            }
            return;
        }

        public void start_stop_DMC(boolean bStart) {
            int nret = native_start_stop_DMC(bStart);
            if (nret == 0) {
                if (dmcObserver == null) {
                    dmcObserver = new DMCObserver();
                }
                if (bStart) {
                    native_add_remove_DMCObserver(dmcObserver, bStart);
                    // native_searchNetwork();
                } else {
                    dmcObserver = null;
                }

            }
        }

        public void getOnLineDevices(List<DlnaDevice> onLineDevicesList,
                int type) {
            if (type == DlnaDevice.DEVICE_TYPE_DMS) {
                synchronized (dmsDevicesList) {
                    for (DlnaDeviceEx deviceEx : dmsDevicesList) {
                        onLineDevicesList.add(deviceEx.device);
                    }
                }
            } else if (type == DlnaDevice.DEVICE_TYPE_DMR) {
                synchronized (dmrDeviceList) {
                    for (DlnaDeviceEx deviceEx : dmrDeviceList) {
                        onLineDevicesList.add(deviceEx.device);
                    }
                }
            }

            return;
        }

        public int playControl(String deviceUDN, int CONTROL_TYPE, String param) {

            if (CONTROL_TYPE >= AVTransportOpr.AVT_UNKNOWN) {
                Log.e(TAG, "Unknow playControl Command");
                return -1;
            }
            synchronized (dmrDeviceList) {
                for (DlnaDeviceEx dlnaDeviceEx : dmrDeviceList) {
                    if (dlnaDeviceEx.hitThisDevice(deviceUDN)) {
                        return native_playControl(dlnaDeviceEx.avtActionUrl,
                                CONTROL_TYPE, param);
                    }

                }
            }
            return -1;
        }

        public void searchDevice(int type) {
            native_searchNetwork(type);
        }

        public int getContents(String deviceUDN, int type,
                List<ContentInfo> filesList, int StartingIndex, int Count) {
            synchronized (dmsDevicesList) {
                for (DlnaDeviceEx deviceInfo : dmsDevicesList) {
                    if (deviceInfo.hitThisDevice(deviceUDN)) {
                        ArrayList<ContentInfo> existInfos = null;
                        if (type == 0)
                            existInfos = deviceInfo.videoContents;
                        else if (type == 1)
                            existInfos = deviceInfo.audioContents;
                        else
                            existInfos = deviceInfo.imageContents;

                        synchronized (existInfos) {
                            int nleft = existInfos.size() - StartingIndex;
                            if (nleft < 0 || Count < 0)
                                return -1;

                            nleft = Count < nleft ? Count : nleft;
                            for (int i = StartingIndex; i < StartingIndex
                                    + nleft; i++) {
                                filesList.add(existInfos.get(i));
                            }
                        }
                    }
                }
            }

            return 0;
        }

        /* @Override */
        public boolean setAutoStartup(boolean bAutoStartup)
                throws RemoteException {
            boolean bOldValue = DMCService.this.bAutoStartup;
            DMCService.this.bAutoStartup = bAutoStartup;

            Context context = getApplicationContext();
            Editor editor = null;

            if (context != null) {
                SharedPreferences preferences = context.getSharedPreferences(
                        EasyDlnaUtil.TAG_STRING, 0);
                editor = preferences.edit();
            }
            if (editor != null) {
                editor.putBoolean(DMC_AUTOSTART_KEY, bAutoStartup);
                editor.commit();
            }

            return bAutoStartup;
        }

        @Override
        public int refreshContents(String deviceUDN) throws RemoteException {
            synchronized (dmsDevicesList) {
                for (DlnaDeviceEx dlnaDeviceEx : dmsDevicesList) {
                    if (dlnaDeviceEx.hitThisDevice(deviceUDN)) {
                        dlnaDeviceEx.refreshContents();
                    }

                }
            }
            return 0;
        }

    }

    @Override
    public void onCreate() {
        if (dmcServiceImpl == null) {
            startUp();
        }
        super.onCreate();
    }

    @Override
    public void onDestroy() {
        if (dmcServiceImpl != null) {
            cleanUp();
            Log.i(DMCSER_STRING, "DMCService onDestroy");
        }
        super.onDestroy();
    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
    }

    @Override
    public boolean onUnbind(Intent intent) {
        // TODO_TASK Auto-generated method stub
        return super.onUnbind(intent);
    }

    @Override
    public IBinder onBind(Intent intent) {
        if (dmcServiceImpl == null) {
            startUp();
        }
        return dmcServiceImpl;
    }

}

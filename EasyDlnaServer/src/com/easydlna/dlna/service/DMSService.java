package com.easydlna.dlna.service;

import android.app.Service;

import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.database.ContentObserver;
import android.database.Cursor;

import android.net.Uri;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.PowerManager;

import android.os.RemoteException;
import android.provider.MediaStore;
import android.util.Log;
import java.io.File;

import com.easydlna.application.EasyDlnaUtil;
import com.easydlna.dlna.service.IDMSService;

public class DMSService extends Service {
    public static String DMS_AUTOSTART_KEY = "DMSAutoStart";

    public static int SHARED_TYPE_VIDEO = 0x00000001;
    public static int SHARED_TYPE_AUDIO = 0x00000010;
    public static int SHARED_TYPE_IMAGE = 0x00000100;

    private final static String DMSSER_STRING = "DMSService";
    private final static String DMS_NAME_KEY = "DMS Friendly Name";

    private final static int dbTypeEV = 0;
    private final static int dbTypeEA = 1;
    private final static int dbTypeEI = 2;
    private final static int dbTypeIV = 3;
    private final static int dbTypeIA = 4;
    private final static int dbTypeII = 5;

    private final static String DMS_NAME_VALUE = "EasyDlna Media Server";

    private final int columnId = 0;
    private final int columnTitle = 1;
    private final int columnSize = 2;
    private final int columnMimeType = 3;
    private final int columnDisplayName = 4;
    private final int columnData = 5;

    private static int containerUpdateId = 0;
    private String[] selectColumnNamesStrings = new String[] {
            MediaStore.Video.Media._ID, MediaStore.Video.Media.TITLE,
            MediaStore.Video.Media.SIZE, MediaStore.Video.Media.MIME_TYPE,
            MediaStore.Video.Media.DISPLAY_NAME, MediaStore.Video.Media.DATA, };

    private String[] selectThumbnailsColumnNamesStrings = new String[] { MediaStore.Video.Thumbnails.DATA, };
    private String DIR_PREFIX = "/dlna/Server";

    private IDMSServiceImpl dmsServiceImpl = null;
    private String serverNameString = null;
    private IDMSObserver dmsObserver = null;

    private final static String DMS_VIDEO_E_DIR = "video-external";
    private final static String DMS_VIDEO_THUNM_E_DIR = "video-thumb-external";
    private final static String DMS_VIDEO_E_ID = "ev";

    private final static String DMS_AUDIO_E_DIR = "audio-external";
    private final static String DMS_AUDIO_E_ID = "ea";

    private final static String DMS_IMAGE_E_DIR = "image-external";
    private final static String DMS_IMAGE_THUNM_E_DIR = "image-thumb-external";
    private final static String DMS_IMAGE_E_ID = "ei";

    private final static String DMS_VIDEO_I_DIR = "video-internal";
    private final static String DMS_VIDEO_THUNM_I_DIR = "video-thumb-internal";
    private final static String DMS_VIDEO_I_ID = "iv";

    private final static String DMS_AUDIO_I_DIR = "audio-internal";
    private final static String DMS_AUDIO_I_ID = "ia";

    private final static String DMS_IMAGE_I_DIR = "image-internal";
    private final static String DMS_IMAGE_THUNM_I_DIR = "image-thumb-internal";
    private final static String DMS_IMAGE_I_ID = "ii";

    private final static String DMS_VIDEOSHARED_KEY = "ShareVideo";
    private final static String DMS_AUDIOSHARED_KEY = "ShareAudio";
    private final static String DMS_IMAGESHARED_KEY = "ShareImage";

    private volatile boolean bVideoShared = true;
    private volatile boolean bAudioShared = true;
    private volatile boolean bImageShared = true;
    private volatile boolean bAutoStartup = false;
    private volatile boolean bStarted = false;

    private Handler uiHandler = null;
    private String dMS_PROTOCAL_PREFIXString = "http-get:*:";
    private String dMSUrlString = null;

    private ContentBroadcastReceiver contentBroadcastReceiver = null;
    private DmsBroadcastReceiver dmsBroadcastReceiver = null;

    private DBObserver dbEVObserver = new DBObserver(dbTypeEV);
    private DBObserver dbEAObserver = new DBObserver(dbTypeEA);
    private DBObserver dbEIObserver = new DBObserver(dbTypeEI);

    private DBObserver dbIVObserver = new DBObserver(dbTypeIV);
    private DBObserver dbIAObserver = new DBObserver(dbTypeIA);
    private DBObserver dbIIObserver = new DBObserver(dbTypeII);

    private PowerManager.WakeLock wakeLock = null;

    private class ContentBroadcastReceiver extends BroadcastReceiver {

        /* @Override */
        public void onReceive(Context context, Intent intent) {
            String intentActionString = intent.getAction();

            if (intentActionString.equals(Intent.ACTION_MEDIA_UNMOUNTED)
                    || intentActionString
                            .equals(Intent.ACTION_MEDIA_BAD_REMOVAL)
                    || intentActionString.equals(Intent.ACTION_MEDIA_SHARED)
                    || intentActionString
                            .equals(Intent.ACTION_MEDIA_SCANNER_FINISHED)) {
                String notifyString = "";
                if (bVideoShared)
                    notifyString += DMS_VIDEO_E_ID + ","
                            + (++containerUpdateId);
                if (bAudioShared)
                    notifyString += "," + DMS_AUDIO_E_ID + ","
                            + (++containerUpdateId);
                if (bImageShared)
                    notifyString += "," + DMS_IMAGE_E_ID + ","
                            + (++containerUpdateId);

                native_notifyChange(notifyString);
            }
            return;
        }
    }

    public class DmsBroadcastReceiver extends BroadcastReceiver {

        final private static String DLNABR_TAG_STRING = "DmsBroadcastReceiver";

        /* @Override */
        public void onReceive(Context context, Intent intent) {
            SharedPreferences sharedPreferences = context.getSharedPreferences(
                    EasyDlnaUtil.TAG_STRING, 0);
            if (sharedPreferences == null)
                return;
            boolean bDMSAutoStart = sharedPreferences.getBoolean(
                    DMSService.DMS_AUTOSTART_KEY, false/* true */);

            String actionName = intent.getAction();
            if (actionName.equals(Intent.ACTION_BOOT_COMPLETED)) {
                Log.i(DLNABR_TAG_STRING, "Receive broadcast to service : "
                        + Intent.ACTION_BOOT_COMPLETED);
                if (bDMSAutoStart) {
                    context.startService(new Intent(
                            "com.easydlna.dlna.service.DMSService"));
                }

            } else if (actionName
                    .equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
                Log.i(DLNABR_TAG_STRING, "Receive broadcast to service : "
                        + WifiManager.NETWORK_STATE_CHANGED_ACTION);
            } else if (actionName
                    .equals(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION)) {
                Log.i(DLNABR_TAG_STRING, "Receive broadcast to service : "
                        + WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION);
            } else if (actionName.equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
                Log.i(DLNABR_TAG_STRING, "Receive broadcast to service : "
                        + WifiManager.WIFI_STATE_CHANGED_ACTION);
            } else if (actionName.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED)) {
                Log.i(DLNABR_TAG_STRING, "Receive broadcast to service : "
                        + Intent.ACTION_AIRPLANE_MODE_CHANGED);
            }
        }

    }

    private class IDMSObserver {

        public IDMSObserver() {

        }

        public void init() {

            return;
        }

        public void cleanup() {
            return;
        }

        /* @Override */
        public ContentInfo[] browseChildren(String id, int index, int maxCount) {
            if (id == null)
                return null;
            if (maxCount >= 100)
                maxCount = 40;
            if (id.compareTo("0") == 0) {
                int count = 7;
                if (index > count)
                    return null;

                ContentInfo[] outInfoArray = new ContentInfo[count];
                int iTemp = 0;
                if (bVideoShared) {
                    outInfoArray[iTemp++] = getContentInfo(DMS_VIDEO_E_ID,
                            SHARED_TYPE_VIDEO, true);
                    outInfoArray[iTemp++] = getContentInfo(DMS_VIDEO_I_ID,
                            SHARED_TYPE_VIDEO, false);
                } else {
                    outInfoArray[iTemp++] = null;
                    outInfoArray[iTemp++] = null;
                }
                if (bAudioShared) {
                    outInfoArray[iTemp++] = getContentInfo(DMS_AUDIO_E_ID,
                            SHARED_TYPE_AUDIO, true);
                    outInfoArray[iTemp++] = getContentInfo(DMS_AUDIO_I_ID,
                            SHARED_TYPE_AUDIO, false);
                } else {
                    outInfoArray[iTemp++] = null;
                    outInfoArray[iTemp++] = null;
                }
                if (bImageShared) {
                    outInfoArray[iTemp++] = getContentInfo(DMS_IMAGE_E_ID,
                            SHARED_TYPE_IMAGE, true);
                    outInfoArray[iTemp++] = getContentInfo(DMS_IMAGE_I_ID,
                            SHARED_TYPE_IMAGE, false);
                } else {
                    outInfoArray[iTemp++] = null;
                    outInfoArray[iTemp++] = null;
                }
                for (int i = 0; i < index; i++)
                    outInfoArray[i] = null;
                if (index + maxCount < count) {
                    for (int i = index + maxCount; i < count; i++)
                        outInfoArray[i] = null;
                }

                return outInfoArray;

            } else if (id.charAt(1) == 'v') {
                if (!bVideoShared)
                    return null;
                return getContentInfoArray(id, SHARED_TYPE_VIDEO, index,
                        maxCount);
            } else if (id.charAt(1) == 'a') {
                if (!bAudioShared)
                    return null;
                return getContentInfoArray(id, SHARED_TYPE_AUDIO, index,
                        maxCount);
            } else if (id.charAt(1) == 'i') {
                if (!bImageShared)
                    return null;
                return getContentInfoArray(id, SHARED_TYPE_IMAGE, index,
                        maxCount);

            }
            return null;
        }

        /* @Override */
        public ContentInfo browseMetaData(String id) {
            if (id == null)
                return null;
            if (id.compareTo("0") == 0) {
                Log.e(DMSSER_STRING, "BrowseMetaData of root");
                ContentInfo contentInfo = new ContentInfo();

                contentInfo.id = "0";
                contentInfo.parentId = "-1";

                contentInfo.title = "root";
                contentInfo.childCount = 6;
                /*
                 * if(bVideoShared) contentInfo.childCount += 2;
                 * if(bAudioShared) contentInfo.childCount += 2;
                 * if(bImageShared) contentInfo.childCount +=2;
                 */

                return contentInfo;
            } else if (id.charAt(1) == 'v') {
                if (!bVideoShared)
                    return null;
                Log.e(DMSSER_STRING, "BrowseMetaData of dirctory of video");
                return getContentInfo(id, SHARED_TYPE_VIDEO);
            } else if (id.charAt(1) == 'a') {
                if (!bAudioShared)
                    return null;
                Log.e(DMSSER_STRING, "BrowseMetaData of dirctory of audio");
                return getContentInfo(id, SHARED_TYPE_AUDIO);
            } else if (id.charAt(1) == 'i') {
                if (!bImageShared)
                    return null;
                Log.e(DMSSER_STRING, "BrowseMetaData of dirctory of image");
                return getContentInfo(id, SHARED_TYPE_IMAGE);

            }
            return null;

        }

        /* @Override */
        public ContentInfo getFileInfo(String filename) {
            if (filename.startsWith(DIR_PREFIX) == false)
                return null;
            int nPos = filename.lastIndexOf('/');
            if (nPos == -1)
                return null;
            filename = filename.substring(nPos + 1);
            nPos = filename.lastIndexOf('.');
            if (nPos != -1)
                filename = filename.substring(0, nPos);
            Log.e("getfileinfo", "filename=" + filename);
            /* external --- */
            if (filename.contains(DMS_VIDEO_E_ID)) {
                if (!bVideoShared)
                    return null;

                Uri uri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, false);
            }
            if (filename.contains(DMS_AUDIO_E_ID)) {
                if (!bAudioShared)
                    return null;
                Uri uri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, false);
            }
            if (filename.contains(DMS_IMAGE_E_ID)) {
                if (!bImageShared)
                    return null;
                Uri uri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, false);
            }

            /* internal --- */
            if (filename.contains(DMS_VIDEO_I_ID)) {
                if (!bVideoShared)
                    return null;
                Uri uri = MediaStore.Video.Media.INTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, false);
            }
            if (filename.contains(DMS_AUDIO_I_ID)) {
                if (!bAudioShared)
                    return null;
                Uri uri = MediaStore.Audio.Media.INTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, false);
            }
            if (filename.contains(DMS_IMAGE_I_ID)) {
                if (!bImageShared)
                    return null;
                Uri uri = MediaStore.Images.Media.INTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, false);
            }

            /* thumbnail track */
            if (filename.contains(DMS_IMAGE_THUNM_I_DIR)) {
                Uri uri = MediaStore.Images.Thumbnails.INTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, true);
            }

            if (filename.contains(DMS_VIDEO_THUNM_I_DIR)) {
                Uri uri = MediaStore.Video.Thumbnails.INTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, true);
            }

            if (filename.contains(DMS_IMAGE_THUNM_E_DIR)) {
                Uri uri = MediaStore.Images.Thumbnails.EXTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, true);
            }
            if (filename.contains(DMS_VIDEO_THUNM_E_DIR)) {
                Uri uri = MediaStore.Video.Thumbnails.EXTERNAL_CONTENT_URI;
                return DMSService.this.getFileInfo(filename, uri, true);
            }

            return null;
        }

    }

    private class DBObserver extends ContentObserver {
        private int type;

        public DBObserver(int type) {
            super(new Handler());
            this.type = type;
        }

        /* @Override */
        public void onChange(boolean selfChange) {
            switch (type) {
            case DMSService.dbTypeEV: {
                if (!bVideoShared)
                    return;
                Log.e(DMSSER_STRING, "DB of external-video notify change");
                native_notifyChange(DMS_VIDEO_E_ID + ","
                        + (++containerUpdateId));
            }
                break;
            case DMSService.dbTypeEA: {
                if (!bAudioShared)
                    return;
                Log.e(DMSSER_STRING, "DB of external-audio notify change");
                native_notifyChange(DMS_AUDIO_E_ID + ","
                        + (++containerUpdateId));
            }
                break;
            case DMSService.dbTypeEI: {
                if (!bImageShared)
                    return;
                Log.e(DMSSER_STRING, "DB of external-image notify change");
                native_notifyChange(DMS_IMAGE_E_ID + ","
                        + (++containerUpdateId));
            }
                break;
            case DMSService.dbTypeIV: {
                if (!bVideoShared)
                    return;
                Log.e(DMSSER_STRING, "DB of internal-video notify change");
                native_notifyChange(DMS_VIDEO_I_ID + ","
                        + (++containerUpdateId));
            }
                break;
            case DMSService.dbTypeIA: {
                if (!bAudioShared)
                    return;
                Log.e(DMSSER_STRING, "DB of internal-audio notify change");
                native_notifyChange(DMS_AUDIO_I_ID + ","
                        + (++containerUpdateId));
            }
                break;
            case DMSService.dbTypeII: {
                if (!bImageShared)
                    return;
                Log.e(DMSSER_STRING, "DB of internal-image notify change");
                native_notifyChange(DMS_IMAGE_I_ID + ","
                        + (++containerUpdateId));
            }
                break;

            default:
                break;
            }

            super.onChange(selfChange);
        }
    }

    private void notifyChange(int sharedType) {
        String msg = null;
        msg = DMS_VIDEO_E_ID + "," + (++containerUpdateId);
        msg += "," + DMS_VIDEO_I_ID + "," + (++containerUpdateId);
        msg += "," + DMS_AUDIO_E_ID + "," + (++containerUpdateId);
        msg += "," + DMS_AUDIO_I_ID + "," + (++containerUpdateId);
        msg += "," + DMS_IMAGE_E_ID + "," + (++containerUpdateId);
        msg += "," + DMS_IMAGE_I_ID + "," + (++containerUpdateId);
        native_notifyChange(msg);
    }

    private class IDMSServiceImpl extends IDMSService.Stub {

        public void setSharedType(int type) throws RemoteException {
            setShared(SHARED_TYPE_VIDEO,
                    (type & SHARED_TYPE_VIDEO) == SHARED_TYPE_VIDEO);
            setShared(SHARED_TYPE_AUDIO,
                    (type & SHARED_TYPE_AUDIO) == SHARED_TYPE_AUDIO);
            setShared(SHARED_TYPE_IMAGE,
                    (type & SHARED_TYPE_IMAGE) == SHARED_TYPE_IMAGE);
            notifyChange(type);
            return;
        }

        private int setShared(int type, boolean bShared) {
            Context context = getApplicationContext();
            Editor editor = null;

            if (context != null) {
                SharedPreferences preferences = context.getSharedPreferences(
                        EasyDlnaUtil.TAG_STRING, 0);
                editor = preferences.edit();
            }

            if (type == DMSService.SHARED_TYPE_AUDIO) {
                bAudioShared = bShared;
                if (editor != null)
                    editor.putBoolean(DMS_AUDIOSHARED_KEY, bAudioShared);

            } else if (type == DMSService.SHARED_TYPE_VIDEO) {
                bVideoShared = bShared;
                if (editor != null)
                    editor.putBoolean(DMS_VIDEOSHARED_KEY, bVideoShared);

            } else if (type == DMSService.SHARED_TYPE_IMAGE) {
                bImageShared = bShared;
                if (editor != null)
                    editor.putBoolean(DMS_IMAGESHARED_KEY, bImageShared);

            } else {
                return -1;
            }
            if (editor != null)
                editor.commit();
            return 0;
        }

        /* @Override */
        public int setServerFriendlyName(String name, boolean reStartServer)
                throws RemoteException {
            if (name == null)
                return -1;
            if (serverNameString.equals(name))
                return 0;
            serverNameString = name;
            Context context = getApplicationContext();
            EasyDlnaUtil.putPreference(context, DMS_NAME_KEY, serverNameString);
            native_set_DMSFriendlyName(serverNameString);
            if (reStartServer) {
                start_stop_DMS(false);
                start_stop_DMS(true);
            }
            return 0;
        }

        public int start_stop_DMS(boolean bStart) {
            int nret = native_start_stop_DMS(bStart);
            if (nret >= 0) {
                if (bStart)
                    if (serverNameString != null) {
                        native_set_DMSFriendlyName(serverNameString);
                    }

                if (!bStart && dmsObserver != null) {
                    native_add_remove_DMSObserver(dmsObserver, bStart);
                    dmsObserver.cleanup();
                    dmsObserver = null;
                } else if (bStart) {
                    dMSUrlString = "http://" + EasyDlnaUtil.dlnaIp + ":" + nret
                            + DIR_PREFIX;
                    if (dmsObserver == null) {
                        dmsObserver = new IDMSObserver();
                        dmsObserver.init();
                        native_add_remove_DMSObserver(dmsObserver, true);
                    }
                }
            } else {
                return -1;
            }

            DMSService.this.bStarted = bStart;
            return 0;
        }

        public String getServerFriendlyName() throws RemoteException {
            return serverNameString;
        }

        /* @Override */
        public int addSharedContents(int type) throws RemoteException {
            return setShared(type, true);
        }

        /* @Override */
        public int removeSharedContents(int type) throws RemoteException {
            return setShared(type, false);
        }

        /* @Override */
        public boolean setAutoStartup(boolean bAutoStartup)
                throws RemoteException {
            boolean bOldValue = DMSService.this.bAutoStartup;
            DMSService.this.bAutoStartup = bAutoStartup;

            Context context = getApplicationContext();
            EasyDlnaUtil
                    .putPreference(context, DMS_AUTOSTART_KEY, bAutoStartup);
            return bAutoStartup;
        }

        @Override
        public boolean getAutoStartup() throws RemoteException {

            return DMSService.this.bAutoStartup;
        }

        @Override
        public boolean isStarted() throws RemoteException {
            return DMSService.this.bStarted;
        }

        public int getSharedType() throws RemoteException {
            int result = 0;
            if (DMSService.this.bAudioShared)
                result |= SHARED_TYPE_AUDIO;
            if (DMSService.this.bVideoShared)
                result |= SHARED_TYPE_VIDEO;
            if (DMSService.this.bImageShared)
                result |= SHARED_TYPE_IMAGE;
            return result;
        }

        @Override
        public void sendBroadcast(int timeout) throws RemoteException {
            native_sendBroadcast(timeout);
        }
    }

    private native void native_add_remove_DMSObserver(IDMSObserver observer,
            boolean bAdd);

    private native int native_start_stop_DMS(boolean bStart);

    private native void native_set_DMSFriendlyName(String name);

    private native void native_notifyChange(String changeValue);

    private native void native_sendBroadcast(int timeOut);

    private ContentInfo getFileInfo(String id, Uri uriQuery, boolean bThumbnail) {

        Context context = getApplicationContext();
        if (context == null)
            return null;

        ContentResolver resolver = context.getContentResolver();
        boolean bDir = false;

        ContentInfo outContentInfo = new ContentInfo();

        Cursor cursorOut = null;
        try {
            int pos = bThumbnail ? DMS_IMAGE_THUNM_E_DIR.length()
                    : DMS_IMAGE_E_ID.length();
            String itemIdString = id.substring(pos + 1);

            String condiString = "";
            if (bThumbnail) {

                if (id.contains(DMS_IMAGE_THUNM_E_DIR)
                        || id.contains(DMS_IMAGE_THUNM_I_DIR)) {
                    condiString += "image_id = " + itemIdString
                            + " AND kind = "
                            + MediaStore.Images.Thumbnails.MINI_KIND;
                } else {
                    condiString += "video_id = " + itemIdString
                            + " AND kind = "
                            + MediaStore.Video.Thumbnails.MINI_KIND;
                    ;
                }
            } else {
                condiString += "_id = " + itemIdString;
            }
            if (!bThumbnail) {
                Log.e("test query string", condiString);
                cursorOut = resolver.query(uriQuery, selectColumnNamesStrings,
                        condiString, null, null);
                Log.e(DMSSER_STRING, uriQuery.getPath());
                if (cursorOut.getCount() == 0)
                    return null;
                cursorOut.moveToFirst();
                outContentInfo.realPath = cursorOut.getString(columnData);

                outContentInfo.size = cursorOut.getLong(columnSize);
                outContentInfo.protocalInfo = cursorOut
                        .getString(columnMimeType);
            } else {
                Log.e("test query string be thumbnail", condiString);
                cursorOut = resolver.query(uriQuery,
                        selectThumbnailsColumnNamesStrings, condiString, null,
                        null);
                Log.e("test be thumbnail", uriQuery.getPath());
                if (cursorOut.getCount() == 0)
                    return null;

                cursorOut.moveToFirst();
                outContentInfo.realPath = cursorOut.getString(0);

                File fileState = new File(outContentInfo.realPath);

                outContentInfo.size = fileState.length();
                outContentInfo.protocalInfo = "image/jpeg";
                fileState = null;
            }

            return outContentInfo;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        } finally {
            if (cursorOut != null)
                cursorOut.close();
        }
    }

    private Uri buildQueryUri(int type, boolean bExternal) {
        Uri urlQuery = null;
        if (type == SHARED_TYPE_VIDEO) {
            urlQuery = bExternal ? MediaStore.Video.Media.EXTERNAL_CONTENT_URI
                    : MediaStore.Video.Media.INTERNAL_CONTENT_URI;
        } else if (type == SHARED_TYPE_AUDIO) {
            urlQuery = bExternal ? MediaStore.Audio.Media.EXTERNAL_CONTENT_URI
                    : MediaStore.Audio.Media.INTERNAL_CONTENT_URI;
        } else {
            urlQuery = bExternal ? MediaStore.Images.Media.EXTERNAL_CONTENT_URI
                    : MediaStore.Images.Media.INTERNAL_CONTENT_URI;
        }

        return urlQuery;
    }

    private boolean prepareContentInfo(String id, int type,
            ContentInfo outContentInfo, boolean bExternal) {
        boolean bDir = false;

        if (type == SHARED_TYPE_VIDEO) {
            if (id.equals(DMS_VIDEO_E_ID) || id.equals(DMS_VIDEO_I_ID)) {
                bDir = true;
                outContentInfo.title = bExternal ? DMS_VIDEO_E_DIR
                        : DMS_VIDEO_I_DIR;
                outContentInfo.parentId = "0";

            } else {
                outContentInfo.parentId = bExternal ? DMS_VIDEO_E_ID
                        : DMS_VIDEO_I_ID;
            }
            outContentInfo.mediaClasString = "object.item.videoItem";

        } else if (type == SHARED_TYPE_AUDIO) {
            if (id.equals(DMS_AUDIO_E_ID) || id.equals(DMS_AUDIO_I_ID)) {
                bDir = true;
                outContentInfo.title = bExternal ? DMS_AUDIO_E_DIR
                        : DMS_AUDIO_I_DIR;
                outContentInfo.parentId = "0";

            } else {
                outContentInfo.parentId = bExternal ? DMS_AUDIO_E_ID
                        : DMS_AUDIO_I_DIR;

            }
            outContentInfo.mediaClasString = "object.item.audioItem";

        } else if (type == SHARED_TYPE_IMAGE) {
            if (id.equals(DMS_IMAGE_E_ID) || id.equals(DMS_IMAGE_I_ID)) {
                bDir = true;
                outContentInfo.title = bExternal ? DMS_IMAGE_E_DIR
                        : DMS_IMAGE_I_DIR;
                outContentInfo.parentId = "0";
            } else {
                outContentInfo.parentId = bExternal ? DMS_IMAGE_E_ID
                        : DMS_IMAGE_I_DIR;
            }
            outContentInfo.mediaClasString = "object.item.imageItem";

        }
        return bDir;

    }

    private void buildContentInfo(int type, String id,
            ContentInfo outContentInfo, Uri urlQuery, Cursor cursorOut,
            boolean bExternal) {

        String itemIdString = id.substring(4);
        outContentInfo.title = cursorOut.getString(columnTitle);

        outContentInfo.size = cursorOut.getLong(columnSize);

        String mimeTypeString = cursorOut.getString(columnMimeType);
        outContentInfo.protocalInfo = dMS_PROTOCAL_PREFIXString
                + mimeTypeString;

        String posfix = cursorOut.getString(columnDisplayName);
        int nls = posfix.lastIndexOf(".");
        if (nls != -1) {
            posfix = posfix.substring(nls);
        } else
            posfix = "";

        if (bExternal) {
            if (type == SHARED_TYPE_VIDEO)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_VIDEO_THUNM_E_DIR + "-" + itemIdString + ".jpg";
            else if (type == SHARED_TYPE_IMAGE)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_IMAGE_THUNM_E_DIR + "-" + itemIdString + ".jpg";

        } else {
            if (type == SHARED_TYPE_VIDEO)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_VIDEO_THUNM_I_DIR + "-" + itemIdString + ".jpg";
            else if (type == SHARED_TYPE_IMAGE)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_IMAGE_THUNM_I_DIR + "-" + itemIdString + ".jpg";

        }

        if (posfix.length() != 0)
            outContentInfo.resourceUrl = dMSUrlString + "/" + outContentInfo.id
                    + posfix;
        else
            outContentInfo.resourceUrl = dMSUrlString + "/" + outContentInfo.id;
    }

    private ContentInfo getContentInfo(String id, int type, boolean bExternal) {

        Context context = getApplicationContext();
        if (context == null)
            return null;

        ContentResolver resolver = context.getContentResolver();
        Uri urlQuery = buildQueryUri(type, bExternal);
        boolean bDir = false;

        ContentInfo outContentInfo = new ContentInfo();
        outContentInfo.id = id;

        bDir = prepareContentInfo(id, type, outContentInfo, bExternal);
        Cursor cursorOut = null;
        try {
            if (bDir) {
                cursorOut = resolver.query(urlQuery, new String[] { "*" },
                        null, null, null);
                if (cursorOut == null)
                    return null;
                outContentInfo.childCount = cursorOut.getCount();
                Log.e(DMSSER_STRING, "children counts = "
                        + outContentInfo.childCount);
            } else {
                String itemIdString = id.substring(3);
                String select = " _id = " + itemIdString;

                cursorOut = resolver.query(urlQuery, selectColumnNamesStrings,
                        select, null, null);
                if (cursorOut == null)
                    return null;
                if (cursorOut.getCount() == 0)
                    return null;
                cursorOut.moveToFirst();
                buildContentInfo(type, id, outContentInfo, urlQuery, cursorOut,
                        bExternal);
            }
            return outContentInfo;
        } catch (Exception e) {
            e.printStackTrace();
            outContentInfo = null;
            return null;
        } finally {
            if (cursorOut != null)
                cursorOut.close();
        }
    }

    private ContentInfo getContentInfo(String id, int type) {
        boolean bExternal = true;
        if (id.startsWith("i"))
            bExternal = false;
        return getContentInfo(id, type, bExternal);

    }

    private ContentInfo buildContentInfo(int type, Cursor cursorOut, String id,
            String parentid, String mediaClasString, boolean bExternal) {
        ContentInfo outContentInfo = new ContentInfo();
        String itemIdString = cursorOut.getString(columnId);

        outContentInfo.id = parentid + "-" + itemIdString;
        outContentInfo.parentId = parentid;
        outContentInfo.mediaClasString = mediaClasString;

        outContentInfo.title = cursorOut.getString(columnTitle);

        outContentInfo.size = cursorOut.getLong(columnSize);

        String mimeTypeString = cursorOut.getString(columnMimeType);
        outContentInfo.protocalInfo = dMS_PROTOCAL_PREFIXString
                + mimeTypeString;

        String posfix = cursorOut.getString(columnDisplayName);
        int nls = posfix.lastIndexOf(".");
        if (nls != -1) {
            posfix = posfix.substring(nls);
        } else
            posfix = "";

        if (bExternal) {
            if (type == SHARED_TYPE_VIDEO)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_VIDEO_THUNM_E_DIR + "-" + itemIdString + ".jpg";
            else if (type == SHARED_TYPE_IMAGE)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_IMAGE_THUNM_E_DIR + "-" + itemIdString + ".jpg";
        } else {
            if (type == SHARED_TYPE_VIDEO)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_VIDEO_THUNM_I_DIR + "-" + itemIdString + ".jpg";
            else if (type == SHARED_TYPE_IMAGE)
                outContentInfo.thumbnailUrl = dMSUrlString + "/"
                        + DMS_IMAGE_THUNM_I_DIR + "-" + itemIdString + ".jpg";
        }

        if (posfix.length() != 0)
            outContentInfo.resourceUrl = dMSUrlString + "/" + outContentInfo.id
                    + posfix;
        else
            outContentInfo.resourceUrl = dMSUrlString + "/" + outContentInfo.id;
        return outContentInfo;
    }

    private ContentInfo[] getContentInfoArray(String id, int type, int index,
            int maxCount) {
        boolean bExternal = true;
        if (id.startsWith("i"))
            bExternal = false;

        return getContentInfoArray(bExternal, id, type, index, maxCount);

    }

    private ContentInfo[] getContentInfoArray(boolean bExternal, String id,
            int type, int index, int maxCount) {

        Context context = getApplicationContext();
        if (context == null)
            return null;
        Log.e(DMSSER_STRING, "get contentinfoArray  " + id + " index= " + index
                + " maxCount= " + maxCount);
        ContentResolver resolver = context.getContentResolver();
        Uri urlQuery = buildQueryUri(type, bExternal);

        String mediaClasString = null;
        String parentid = null;

        if (type == SHARED_TYPE_VIDEO) {

            parentid = bExternal ? DMS_VIDEO_E_ID : DMS_VIDEO_I_ID;
            mediaClasString = "object.item.videoItem";

        } else if (type == SHARED_TYPE_AUDIO) {

            parentid = bExternal ? DMS_AUDIO_E_ID : DMS_AUDIO_I_ID;
            mediaClasString = "object.item.audioItem";

        } else {
            parentid = bExternal ? DMS_IMAGE_E_ID : DMS_IMAGE_I_ID;
            mediaClasString = "object.item.imageItem";

        }
        Log.e(DMSSER_STRING, urlQuery.toString());
        Cursor cursorOut = null;
        int globalTotalCount = 0;
        try {
            cursorOut = resolver.query(urlQuery, selectColumnNamesStrings,
                    null, null, null);
            int totalCount = cursorOut.getCount();
            if (index > totalCount) {
                return null;
            }
            globalTotalCount = totalCount;
            totalCount = Math.min(maxCount, totalCount);

            if (totalCount == 0) {
                return null;
            }
            ContentInfo[] outInfoArray = new ContentInfo[totalCount];

            cursorOut.moveToPosition(index);
            int ntemp = 0;
            while (!cursorOut.isAfterLast() && totalCount > 0) {

                ContentInfo outContentInfo = buildContentInfo(type, cursorOut,
                        id, parentid, mediaClasString, bExternal);
                outContentInfo.totalMatches = globalTotalCount;
                outInfoArray[ntemp++] = outContentInfo;

                --totalCount;
                cursorOut.moveToNext();
            }
            return outInfoArray;

        } catch (Exception e) {
            e.printStackTrace();
            return null;
        } finally {
            if (cursorOut != null)
                cursorOut.close();
        }
    }

    private void startUp() {
        if (EasyDlnaUtil.startDLNA(null) != 0) {
            return;
        }
        PowerManager pwManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wakeLock = pwManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                DMSSER_STRING);
        wakeLock.acquire();

        dmsServiceImpl = new IDMSServiceImpl();
        Context context = getApplicationContext();
        if (context != null) {
            SharedPreferences preferences = context.getSharedPreferences(
                    EasyDlnaUtil.TAG_STRING, 0);
            serverNameString = preferences.getString(DMS_NAME_KEY,
                    DMS_NAME_VALUE);

            bVideoShared = preferences.getBoolean(DMS_VIDEOSHARED_KEY,
                    bVideoShared);
            bAudioShared = preferences.getBoolean(DMS_AUDIOSHARED_KEY,
                    bAudioShared);
            bImageShared = preferences.getBoolean(DMS_IMAGESHARED_KEY,
                    bImageShared);

            ContentResolver resolver = context.getContentResolver();

            resolver.registerContentObserver(
                    MediaStore.Video.Media.INTERNAL_CONTENT_URI, true,
                    dbIVObserver);
            resolver.registerContentObserver(
                    MediaStore.Audio.Media.INTERNAL_CONTENT_URI, true,
                    dbIAObserver);
            resolver.registerContentObserver(
                    MediaStore.Images.Media.INTERNAL_CONTENT_URI, true,
                    dbIIObserver);

            resolver.registerContentObserver(
                    MediaStore.Video.Media.EXTERNAL_CONTENT_URI, true,
                    dbEVObserver);
            resolver.registerContentObserver(
                    MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, true,
                    dbEAObserver);
            resolver.registerContentObserver(
                    MediaStore.Images.Media.EXTERNAL_CONTENT_URI, true,
                    dbEIObserver);

            contentBroadcastReceiver = new ContentBroadcastReceiver();

            IntentFilter iFilter = new IntentFilter();
            iFilter.addAction(Intent.ACTION_MEDIA_EJECT);
            iFilter.addAction(Intent.ACTION_MEDIA_BAD_REMOVAL);
            iFilter.addAction(Intent.ACTION_MEDIA_UNMOUNTED);
            iFilter.addAction(Intent.ACTION_MEDIA_SHARED);
            context.registerReceiver(contentBroadcastReceiver, iFilter);
        }

        // dmsServiceImpl.start_stop_DMS(true);
    }

    private void cleanUp() {
        Context context = getApplicationContext();
        if (context != null) {
            ContentResolver resolver = context.getContentResolver();
            resolver.unregisterContentObserver(dbEVObserver);
            resolver.unregisterContentObserver(dbEAObserver);
            resolver.unregisterContentObserver(dbEIObserver);

            resolver.unregisterContentObserver(dbIVObserver);
            resolver.unregisterContentObserver(dbIAObserver);
            resolver.unregisterContentObserver(dbIIObserver);
            if (contentBroadcastReceiver != null) {
                context.unregisterReceiver(contentBroadcastReceiver);
                contentBroadcastReceiver = null;
            }
            /*
             * if (dmsBroadcastReceiver != null) {
             * context.unregisterReceiver(dmsBroadcastReceiver);
             * dmsBroadcastReceiver = null; }
             */

        }

        dmsServiceImpl.start_stop_DMS(false);
        dmsServiceImpl = null;
        EasyDlnaUtil.stopDLNA(null);

        if (wakeLock != null)
            wakeLock.release();
    }

    public void onCreate() {
        if (dmsServiceImpl == null) {
            startUp();
        }

        Log.e(DMSSER_STRING, "DMS Service onCreate");
        super.onCreate();
    }

    public void onDestroy() {
        if (dmsServiceImpl != null) {
            cleanUp();
            Log.e(DMSSER_STRING, "DMS Service onDestroy");
        }
        super.onDestroy();
    }

    /* @Override */
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
    }

    /* @Override */
    public boolean onUnbind(Intent intent) {
        // TODO_TASK Auto-generated method stub
        return super.onUnbind(intent);
    }

    /* @Override */
    public IBinder onBind(Intent intent) {
        if (dmsServiceImpl == null) {
            startUp();
        }
        Log.e(DMSSER_STRING, "onBind");
        return dmsServiceImpl;

    }

}

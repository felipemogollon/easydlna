package com.easydlna.dlna.core;

import java.util.ArrayList;

import com.easydlna.dlna.dlnaclient.DMCClient;
import com.easydlna.dlna.service.ContentInfo;
import com.easydlna.dlna.service.DlnaDevice;

public class ContentDevice extends DlnaDevice {

    public static final DeviceCollection<ContentDevice> sDevices = new DeviceCollection<ContentDevice>();

    public static boolean isContentDevice(DlnaDevice device) {
        return device.type == DlnaDevice.DEVICE_TYPE_DMS;
    }

    boolean mIsReady = false;

    public boolean isDeviceReady() {
        return mIsReady;
    }

    public Object mUpdateLock = new Object();

    private ArrayList<ArrayList<ContentInfoEx>> mContents = new ArrayList<ArrayList<ContentInfoEx>>();

    public ArrayList<ContentInfoEx> getContents(int type) {
        return mContents.get(type);
    }

    public ContentDevice(DlnaDevice base) {
        super(base);
        mIsReady = false;
        mContents.add(new ArrayList<ContentInfoEx>());
        mContents.add(new ArrayList<ContentInfoEx>());
        mContents.add(new ArrayList<ContentInfoEx>());
        mContents.add(new ArrayList<ContentInfoEx>());
        mContents.add(new ArrayList<ContentInfoEx>());
    }

    public void updateContent(DMCClient client, int audioCount, int imageCount,
            int videoCount) {
        synchronized (mUpdateLock) {
            ArrayList<ContentInfo> infos = new ArrayList<ContentInfo>();

            infos.clear();
            client.getContents(udnString, ContentInfoEx.MEDIA_TYPE_AUDIO,
                    infos, 0, audioCount);
            updateContent(infos, ContentInfoEx.MEDIA_TYPE_AUDIO);

            infos.clear();
            client.getContents(udnString, ContentInfoEx.MEDIA_TYPE_IMAGE,
                    infos, 0, imageCount);
            updateContent(infos, ContentInfoEx.MEDIA_TYPE_IMAGE);

            infos.clear();
            client.getContents(udnString, ContentInfoEx.MEDIA_TYPE_VIDEO,
                    infos, 0, videoCount);
            updateContent(infos, ContentInfoEx.MEDIA_TYPE_VIDEO);

            mIsReady = true;
        }
    }

    private void updateContent(ArrayList<ContentInfo> source, int mediaType) {
        ArrayList<ContentInfoEx> target = getContents(mediaType);
        target.clear();
        for (ContentInfo info : source) {
            ContentInfoEx infoEx = new ContentInfoEx(info, mediaType);
            target.add(infoEx);
        }
    }

}

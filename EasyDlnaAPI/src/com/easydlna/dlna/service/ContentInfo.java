package com.easydlna.dlna.service;

import android.os.Parcel;
import android.os.Parcelable;

public class ContentInfo implements Parcelable {

    public String id = null;
    public String parentId = null;
    public int childCount = -1;
    public int totalMatches = 0;
    public String title = null;
    public String protocalInfo = null;
    public String resourceUrl = null;
    public String resourceRtspUrl = null;
    public String thumbnailUrl = null;
    public long size = -1;
    public String mediaClasString = null;
    public String realPath = null;

    public static final Parcelable.Creator<ContentInfo> CREATOR = new Parcelable.Creator<ContentInfo>() {

        public ContentInfo createFromParcel(Parcel in) {
            return new ContentInfo(in);
        }

        public ContentInfo[] newArray(int size) {
            return new ContentInfo[size];
        }
    };

    public ContentInfo() {
    }

    private ContentInfo(Parcel in) {
        readFromParcel(in);
    }

    /* @Override */
    public int describeContents() {
        return 0;
    }

    public void readFromParcel(Parcel in) {
        id = in.readString();
        parentId = in.readString();
        childCount = in.readInt();
        title = in.readString();
        protocalInfo = in.readString();
        resourceUrl = in.readString();
        resourceRtspUrl = in.readString();
        thumbnailUrl = in.readString();
        mediaClasString = in.readString();
        size = in.readLong();
        realPath = in.readString();
        totalMatches = in.readInt();
    }

    /* @Override */
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(id);
        dest.writeString(parentId);
        dest.writeInt(childCount);
        dest.writeString(title);
        dest.writeString(protocalInfo);
        dest.writeString(resourceUrl);
        dest.writeString(resourceRtspUrl);
        dest.writeString(thumbnailUrl);
        dest.writeString(mediaClasString);
        dest.writeLong(size);
        dest.writeString(realPath);
        dest.writeInt(totalMatches);
    }
}

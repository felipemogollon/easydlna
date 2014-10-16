package com.easydlna.dlna.service;

import android.os.Parcel;
import android.os.Parcelable;

public class DlnaDevice implements Parcelable {
    public static int DEVICE_TYPE_DMS = 0;
    public static int DEVICE_TYPE_DMR = 1;
    public static int DEVICE_TYPE_UNKNOWN = -1;

    public static String DEVICE_TYPE_DMSString = "urn:schemas-upnp-org:device:MediaServer:1";
    public static String DEVICE_TYPE_DMRString = "urn:schemas-upnp-org:device:MediaRenderer:1";

    public static String CDS_TYPEString = "urn:schemas-upnp-org:service:ContentDirectory:1";
    public static String AVT_TYPEString = "urn:schemas-upnp-org:service:AVTransport:1";

    public int type = DEVICE_TYPE_UNKNOWN;
    public static final int DEVICE_OFFLINE = 0;
    public static final int DEVICE_ONLINE = 1;
    public static final int DEVICE_UPDATED = 2;
    public String udnString = null;
    public String nameString = null;
    public String typeString = null;
    public String baseUrlString = null;
    public int stateNow = DEVICE_OFFLINE;
    public int bSupportRemoteDesktop = 0;
    public static final Parcelable.Creator<DlnaDevice> CREATOR = new Parcelable.Creator<DlnaDevice>() {

        public DlnaDevice createFromParcel(Parcel in) {
            return new DlnaDevice(in);
        }

        public DlnaDevice[] newArray(int size) {
            return new DlnaDevice[size];
        }
    };

    public DlnaDevice(DlnaDevice other) {
        udnString = other.udnString;
        nameString = other.nameString;
        typeString = other.typeString;
        baseUrlString = other.baseUrlString;
        stateNow = other.stateNow;
        type = other.type;
        bSupportRemoteDesktop = other.bSupportRemoteDesktop;
    }

    public DlnaDevice() {
    }

    public void confirmTypes() {
        if (typeString.equals(DEVICE_TYPE_DMSString))
            type = DEVICE_TYPE_DMS;
        else if (typeString.equals(DEVICE_TYPE_DMRString))
            type = DEVICE_TYPE_DMR;
        return;
    }

    private DlnaDevice(Parcel in) {
        readFromParcel(in);
    }

    public int describeContents() {
        return 0;
    }

    public void readFromParcel(Parcel in) {
        udnString = in.readString();
        nameString = in.readString();
        typeString = in.readString();
        baseUrlString = in.readString();
        stateNow = in.readInt();
        type = in.readInt();
        bSupportRemoteDesktop = in.readInt();
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(udnString);
        dest.writeString(nameString);
        dest.writeString(typeString);
        dest.writeString(baseUrlString);
        dest.writeInt(stateNow);
        dest.writeInt(type);
        dest.writeInt(bSupportRemoteDesktop);
    }
}

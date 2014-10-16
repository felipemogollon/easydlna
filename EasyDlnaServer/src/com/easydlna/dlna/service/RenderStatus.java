package com.easydlna.dlna.service;

import android.os.Parcel;
import android.os.Parcelable;

public class RenderStatus implements Parcelable {

	public String urlString = null;
	public String stateString = null;
	public String statusString = null;
	public String durationString = null;

	public static final int STATE_NO_MEDIA_PRESENT = -1;
	public static final int STATE_STOPPED = 0;
	public static final int STATE_PLAYING = 1;
	public static final int STATE_PAUSE_PLAYBACK = 2;

	public int state = STATE_NO_MEDIA_PRESENT;
	public int errcode = 0;
	public long duration = 0;

	public static final Parcelable.Creator<RenderStatus> CREATOR = new Parcelable.Creator<RenderStatus>() {

		public RenderStatus createFromParcel(Parcel in) {
			return new RenderStatus(in);
		}

		public RenderStatus[] newArray(int size) {
			return new RenderStatus[size];
		}
	};

	public RenderStatus() {
	}

	private RenderStatus(Parcel in) {
		readFromParcel(in);
	}

	public int describeContents() {
		return 0;
	}

	public void readFromParcel(Parcel in) {
		urlString = in.readString();
		stateString = in.readString();
		statusString = in.readString();
		durationString = in.readString();
		state = in.readInt();
		errcode = in.readInt();
		duration = in.readLong();
	}

	public void writeToParcel(Parcel dest, int flags) {
		dest.writeString(urlString);
		dest.writeString(stateString);
		dest.writeString(statusString);
		dest.writeString(durationString);
		dest.writeInt(state);
		dest.writeInt(errcode);
		dest.writeLong(duration);
	}
}

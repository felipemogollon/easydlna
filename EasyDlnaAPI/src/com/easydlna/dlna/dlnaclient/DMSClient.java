package com.easydlna.dlna.dlnaclient;

import com.easydlna.dlna.service.IDMSService;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.RemoteException;
import android.os.IBinder;
import android.util.Log;

public class DMSClient {

	final static public int DMS_SERVICE_UP = 0;
	final static public int DMS_SERVICE_DOWN = 1;
	final static public int DMS_SERVICE_ERROR = 2;

	final static public int DMS_SHARE_VIDEO = 0;
	final static public int DMS_SHARE_AUDIO = 1;
	final static public int DMS_SHARE_IMAGE = 2;

	final static private String DMSCLIENT_TAG = "DMSClient";
	private static String DMSSERVICE_INTENT_NAME_STRING = "com.easydlna.dlna.service.DMSService";

	private Context mContext = null;
	private IDMSService dmsService = null;
	private DMSCallback outterCallback = null;

	private ServiceConnection serviceConnection = new ServiceConnection() {
		// @Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			Log.i(DMSCLIENT_TAG, "DMS Service connected");
			if (dmsService == null)
				dmsService = IDMSService.Stub.asInterface(service);
			if (dmsService == null) {
				Log.i(DMSCLIENT_TAG, "DMR Service fail");
				if (outterCallback != null)
					outterCallback.DMSServiceStatusNotify(DMS_SERVICE_ERROR);
				return;
			}
			if (outterCallback != null)
				outterCallback.DMSServiceStatusNotify(DMS_SERVICE_UP);
		}

		// @Override
		public void onServiceDisconnected(ComponentName name) {
			if (outterCallback != null)
				outterCallback.DMSServiceStatusNotify(DMS_SERVICE_DOWN);
			dmsService = null;

			Log.i(DMSCLIENT_TAG, "DMS Service disconnected");
		}

	};

	public DMSClient(Context context) {
		mContext = context;
	}

	public int startDMS(DMSCallback callback) {

		outterCallback = callback;
		if (mContext != null)
			mContext.bindService(new Intent(DMSSERVICE_INTENT_NAME_STRING),
					serviceConnection, Context.BIND_AUTO_CREATE);

		return 0;
	}

	public int stopDMS() {
		outterCallback = null;
		if (mContext != null) {
			if (dmsService != null)
				mContext.unbindService(serviceConnection);
			dmsService = null;
		}
		return 0;
	}

	public String getDMSName() {
		try {
			if (dmsService != null)
				return dmsService.getServerFriendlyName();
		} catch (RemoteException e) {
			return null;
		}
		return null;
	}

	public int setDMSName(String name, boolean bRestart) {
		try {
			if (dmsService != null)
				return dmsService.setServerFriendlyName(name, bRestart);
		} catch (RemoteException e) {
			return -1;
		}
		return -1;
	}

	public int setSharedContents(int type, boolean bShared) {
		if (dmsService == null)
			return -1;
		if (type != DMS_SHARE_VIDEO && type != DMS_SHARE_AUDIO
				&& type != DMS_SHARE_IMAGE)
			return -1;
		try {
			if (bShared)
				return dmsService.addSharedContents(type);
			else
				return dmsService.removeSharedContents(type);
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		return -1;
	}

	public boolean setDMSAutoStartup(boolean bAutoStartup) {
		try {
			if (dmsService != null)
				return dmsService.setAutoStartup(bAutoStartup);
		} catch (RemoteException e) {
			return false;
		}
		return false;
	}

	public interface DMSCallback {
		void DMSServiceStatusNotify(int status);
	}

}

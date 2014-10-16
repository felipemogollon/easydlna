package com.easydlna.dlna.dlnaclient;

import com.easydlna.dlna.service.ContentInfo;
import com.easydlna.dlna.service.DlnaDevice;
import com.easydlna.dlna.service.IDMCCallback;
import com.easydlna.dlna.service.IDMCService;
import com.easydlna.dlna.service.RenderStatus;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.RemoteException;
import android.os.IBinder;
import android.util.Log;
import java.util.List;

public class DMCClient {

	final static public int DMC_SERVICE_UP = 0;
	final static public int DMC_SERVICE_DOWN = 1;
	final static public int DMC_SERVICE_ERROR = 2;

	public final static int AVT_SETDATASOURCE = 0;
	public final static int AVT_PLAY = 1;
	public final static int AVT_STOP = 2;
	public final static int AVT_PAUSE = 3;
	public final static int AVT_SEEK = 4;
	public final static int AVT_GETPOS = 5;

	final static private String DMCCLIENT_TAG = "DMCClient";

	volatile private IDMCService dmcService = null;
	private Context mContext = null;
	final public static String DMCSERVICE_INTENT_NAME_STRING = "com.easydlna.dlna.service.DMCService";
	private DMCCallback outterCallback = null;
	private DMCServiceListener dmcSeriveListener = null;

	private class DMCServiceListener extends IDMCCallback.Stub {

		public void DeviceNotify(DlnaDevice device) throws RemoteException {
			if (outterCallback == null)
				return;
			outterCallback.DlnaDeviceStatusNotify(device);
		}

		public void ContentNotify(String udn, int VideoCounts, int AudioCounts,
				int ImageCounts) throws RemoteException {
			if (outterCallback == null)
				return;
			outterCallback.DlnaFilesNotify(udn, VideoCounts, AudioCounts,
					ImageCounts);
		}

		public void PlayStatusNotify(String udn, RenderStatus renderStatus)
				throws RemoteException {
			if (outterCallback == null)
				return;

			outterCallback.DlnaPlayStatusNotify(udn, renderStatus);
		}
	}

	private ServiceConnection serviceConnection = new ServiceConnection() {

		public void onServiceConnected(ComponentName name, IBinder service) {
			Log.i("DMCClient", "DMC Service connected");
			if (dmcService == null)
				dmcService = IDMCService.Stub.asInterface(service);
			if (dmcService == null) {
				Log.i("DMCClient", "DMC Service fail");
				if (outterCallback != null)
					outterCallback.DMCServiceStatusNotify(DMC_SERVICE_ERROR);
				return;
			}

			if (dmcSeriveListener == null)
				dmcSeriveListener = new DMCServiceListener();
			try {
				dmcService.addDMCCallback(dmcSeriveListener);
				if (outterCallback != null)
					outterCallback.DMCServiceStatusNotify(DMC_SERVICE_UP);

			} catch (RemoteException e) {
				e.printStackTrace();
				if (outterCallback != null)
					outterCallback.DMCServiceStatusNotify(DMC_SERVICE_ERROR);
			}

		}

		public void onServiceDisconnected(ComponentName name) {
			if (outterCallback != null)
				outterCallback.DMCServiceStatusNotify(DMC_SERVICE_DOWN);
			dmcService = null;
			Log.i(DMCCLIENT_TAG, "DMC Service disconnected");
		}
	};

	public DMCClient(Context context) {
		mContext = context;
	}

	public int startDMC(DMCCallback callback) {
		outterCallback = callback;
		if (mContext != null)
			mContext.bindService(new Intent(DMCSERVICE_INTENT_NAME_STRING),
					serviceConnection, Context.BIND_AUTO_CREATE);
		return 0;
	}

	public int stopDMC() {
		outterCallback = null;
		if (mContext != null) {
			if (dmcService != null) {
				try {
					dmcService.removeDMCCallback(dmcSeriveListener);

				} catch (RemoteException e) {
					Log.e(DMCCLIENT_TAG,
							"call removeDMCCallback fails" + e.getMessage());
				}
			}

			mContext.unbindService(serviceConnection);
			dmcService = null;
		}
		return 0;
	}

	public void searchDevice(int type) {
		if (dmcService == null)
			return;
		try {
			dmcService.searchDevice(type);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void refreshContents(String deviceUDN) {
		if (dmcService == null)
			return;
		try {
			dmcService.refreshContents(deviceUDN);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void getOnLineDevices(List<DlnaDevice> onLineDevicesList, int type) {
		if (dmcService == null)
			return;
		try {
			dmcService.getOnLineDevices(onLineDevicesList, type);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public int getContents(String deviceUDN, int type,
			List<ContentInfo> filesList, int StartingIndex, int Count) {
		if (dmcService == null)
			return -1;
		try {
			return dmcService.getContents(deviceUDN, type, filesList,
					StartingIndex, Count);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return -1;
	}

	public int playControl(String deviceUDN, int control_type, String param) {
		try {
			if (dmcService != null)
				return dmcService.playControl(deviceUDN, control_type, param);
		} catch (RemoteException e) {
			return -1;
		}
		return -1;
	}

	public boolean setDMCAutoStartup(boolean bAutoStartup) {
		try {
			if (dmcService != null)
				return dmcService.setAutoStartup(bAutoStartup);
		} catch (RemoteException e) {
			return false;
		}
		return false;
	}

	public interface DMCCallback {
		void DMCServiceStatusNotify(int status);

		void DlnaDeviceStatusNotify(DlnaDevice device);

		void DlnaFilesNotify(String udn, int VideoCounts, int AudioCounts,
				int ImageCounts);

		void DlnaPlayStatusNotify(String udn, RenderStatus renderStatus);
	}
}

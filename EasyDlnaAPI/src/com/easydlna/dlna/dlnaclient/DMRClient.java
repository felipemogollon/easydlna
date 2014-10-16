package com.easydlna.dlna.dlnaclient;

import com.easydlna.dlna.service.IDMRCallback;
import com.easydlna.dlna.service.IDMRService;
import com.easydlna.dlna.service.RenderStatus;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.RemoteException;
import android.os.IBinder;
import android.util.Log;

public class DMRClient {
	final static private String DMRCLIENT_TAG = "DMRClient";
	final public static String DMRSERVICE_INTENT_NAME_STRING = "com.easydlna.dlna.service.DMRService";

	final static public int DMR_SERVICE_UP = 0;
	final static public int DMR_SERVICE_DOWN = 1;
	final static public int DMR_SERVICE_ERROR = 2;

	private Context mContext = null;
	volatile private IDMRService dmrService = null;
	private RenderStatus renderStatus = new RenderStatus();
	private DMRCallback outterCallback = null;
	private DMRServiceListener dmrSeriveListener = null;

	private class DMRServiceListener extends IDMRCallback.Stub {

		public int pause() throws RemoteException {
			if (outterCallback != null)
				return outterCallback.Pause();
			return -1;
		}

		public int play() throws RemoteException {
			if (outterCallback != null)
				return outterCallback.Play();
			return -1;
		}

		public int seek(int seekTo) throws RemoteException {
			if (outterCallback != null)
				return outterCallback.Seek(seekTo);
			return -1;
		}

		public int setDataSource(String urlSource) throws RemoteException {
			if (outterCallback != null)
				return outterCallback.SetDataSource(urlSource);
			return -1;
		}

		public int stop() throws RemoteException {
			if (outterCallback != null)
				return outterCallback.Stop();
			return -1;
		}

		public int getPosition() throws RemoteException {
			if (outterCallback != null)
				return outterCallback.GetPosition();
			return -1;
		}
	}

	private ServiceConnection serviceConnection = new ServiceConnection() {

		public void onServiceConnected(ComponentName name, IBinder service) {
			Log.i(DMRCLIENT_TAG, "DMR Service connected");
			if (dmrService == null) {
				dmrService = IDMRService.Stub.asInterface(service);
			}
			if (dmrService == null) {
				Log.i(DMRCLIENT_TAG, "DMR Service fail");
				if (outterCallback != null) {
					outterCallback.DMRServiceStatusNotify(DMR_SERVICE_ERROR);
				}
				return;
			}
			if (dmrSeriveListener == null) {
				dmrSeriveListener = new DMRServiceListener();
			}
			try {
				dmrService.addDMRCallback(dmrSeriveListener);
				if (outterCallback != null) {
					outterCallback.DMRServiceStatusNotify(DMR_SERVICE_UP);
				}
			} catch (RemoteException e) {
				e.printStackTrace();
				if (outterCallback != null) {
					outterCallback.DMRServiceStatusNotify(DMR_SERVICE_ERROR);
				}
			}
		}

		public void onServiceDisconnected(ComponentName name) {
			if (outterCallback != null) {
				outterCallback.DMRServiceStatusNotify(DMR_SERVICE_DOWN);
			}
			dmrService = null;
			Log.i(DMRCLIENT_TAG, "DMR Service disconnected");
		}
	};

	private void convertPlayingState(PlayingState playingState) {
		renderStatus.urlString = playingState.avtUri;
		renderStatus.stateString = playingState.state;
		renderStatus.statusString = playingState.status;
		renderStatus.durationString = playingState.duration;
	}

	public DMRClient(Context context) {
		mContext = context;
	}

	public int startDMR(DMRCallback callback) {
		outterCallback = callback;
		if (mContext != null) {
			// Start DMR Service and bind. When connected, get IDMRService and
			// set DMRServiceListener to it.
			// There are two binders between DMRClient and DMRService:
			// 1. IDMRService DMRClient -> DMRService
			// 2. DMRServiceListener DMRService -> DMRClient
			// And provide DMRCallback to upper APP.
			// (by tkboy)
			mContext.startService(new Intent(DMRSERVICE_INTENT_NAME_STRING));
			mContext.bindService(new Intent(DMRSERVICE_INTENT_NAME_STRING),
					serviceConnection, Context.BIND_AUTO_CREATE);
		}
		return 0;
	}

	public int stopDMR() {
		outterCallback = null;
		if (mContext != null) {
			if (dmrService != null) {
				try {
					dmrService.removeDMRCallback(dmrSeriveListener);
				} catch (RemoteException e) {
					Log.e(DMRCLIENT_TAG,
							"call removeDMCCallback fails" + e.getMessage());
				}
			}
			mContext.unbindService(serviceConnection);
			mContext.stopService(new Intent(DMRSERVICE_INTENT_NAME_STRING));
			dmrService = null;
		}
		return 0;
	}

	public String getDMRName() {
		try {
			if (dmrService != null) {
				return dmrService.getRenderFriendlyName();
			}
		} catch (RemoteException e) {
			return null;
		}
		return null;
	}

	public int setDMRName(String name, boolean bRestart) {
		try {
			if (dmrService != null) {
				return dmrService.setRenderFriendlyName(name, bRestart);
			}
		} catch (RemoteException e) {
			return -1;
		}
		return -1;
	}

	public int setStatus(PlayingState playingStatus) {
		try {
			if (dmrService != null) {
				convertPlayingState(playingStatus);
				return dmrService.setRenderStatus(renderStatus);
			}
		} catch (RemoteException e) {
			return -1;
		}
		return -1;
	}

	public boolean setDMRAutoStartup(boolean bAutoStartup) {
		try {
			if (dmrService != null) {
				return dmrService.setAutoStartup(bAutoStartup);
			}
		} catch (RemoteException e) {
			return false;
		}
		return false;
	}

	public void sendDMRBroadcast() {
		try {
			if (dmrService != null)
				dmrService.sendBroadcast(1800);
		} catch (RemoteException e) {
		}
	}

	public interface DMRCallback {
		void DMRServiceStatusNotify(int status);

		int SetDataSource(String urlSource);

		int GetPosition();

		int Stop();

		int Play();

		int Pause();

		int Seek(int seekTo);
	}
}

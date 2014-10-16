package com.easydlna.dlna.service;

import com.easydlna.dlna.service.IDMCCallback;
import com.easydlna.dlna.service.DlnaDevice;
import com.easydlna.dlna.service.ContentInfo;

interface IDMCService{
	
	void start_stop_DMC(boolean bStart);
	void addDMCCallback(in IDMCCallback callback);
	void removeDMCCallback(in IDMCCallback callback);
	void getOnLineDevices(out List<DlnaDevice> onLineDevicesList,int type);
	void searchDevice(int type);
	
	int playControl(String deviceUDN,int CONTORL_TYPE,String param);
	int getContents(in String deviceUDN, int type, 
	                out List<ContentInfo> filesList,
					int StartingIndex,int Count);
	int refreshContents(in String deviceUDN);
	boolean setAutoStartup(in boolean bAutoStartup);					

}
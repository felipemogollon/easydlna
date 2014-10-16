package com.easydlna.dlna.service;

import com.easydlna.dlna.service.IDMRCallback;
import com.easydlna.dlna.service.RenderStatus;

interface IDMRService{
	void start_stop_DMR(boolean bStart);
	void addDMRCallback(in IDMRCallback callback);
	void removeDMRCallback(in IDMRCallback callback);
	int setRenderStatus(in RenderStatus renderStatus);
	int setRenderFriendlyName(in String name,in boolean reStartRender);
	String getRenderFriendlyName();
	boolean setAutoStartup(in boolean bAutoStartup);
	void sendBroadcast(int timeout);

}
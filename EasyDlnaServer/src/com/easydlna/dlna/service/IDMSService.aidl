package com.easydlna.dlna.service;

interface IDMSService{
	int start_stop_DMS(boolean bStart);
	int setServerFriendlyName(in String name,in boolean reStartServer);
	String getServerFriendlyName();
	int getSharedType();
	int addSharedContents(in int type);
	int removeSharedContents(in int type);
	void setSharedType(in int type);
	boolean setAutoStartup(in boolean bAutoStartup);
	boolean getAutoStartup();
	boolean isStarted();
	void sendBroadcast(in int timeout);

}
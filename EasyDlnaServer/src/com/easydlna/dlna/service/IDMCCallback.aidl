package com.easydlna.dlna.service;

import com.easydlna.dlna.service.RenderStatus;
import com.easydlna.dlna.service.DlnaDevice;

interface IDMCCallback{
	
	void DeviceNotify(in DlnaDevice device);
	void ContentNotify(in String udn, int VideoCounts,int AudioCounts,int ImageCounts);
	void PlayStatusNotify(in String udn,in RenderStatus renderStatus);
}
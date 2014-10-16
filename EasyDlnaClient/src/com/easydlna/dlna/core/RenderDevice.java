package com.easydlna.dlna.core;

import com.easydlna.dlna.service.DlnaDevice;

public class RenderDevice extends DlnaDevice {
    public static final DeviceCollection<RenderDevice> sDevices = new DeviceCollection<RenderDevice>();

    public static boolean isRenderDevice(DlnaDevice device) {
        return device.type == DlnaDevice.DEVICE_TYPE_DMR;
    }

    public RenderDevice(DlnaDevice device) {
        super(device);
    }
}

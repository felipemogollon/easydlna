package com.easydlna.dlna.core;

import java.util.ArrayList;

import android.util.Log;

import com.easydlna.dlna.service.DlnaDevice;

public class DeviceCollection<TDevice extends DlnaDevice> {

    private static final String TAG = "Dlna:DmcApp.DeviceCollection";

    private final ArrayList<TDevice> sDevices = new ArrayList<TDevice>();

    public int getDeviceCount() {
        return sDevices.size();
    }

    public String[] getOnlineDeviceNames() {
        ArrayList<String> nameList = new ArrayList<String>();
        for (TDevice device : sDevices) {
            nameList.add(device.nameString);
        }
        return nameList.toArray(new String[0]);
    }

    public void addDevice(TDevice device) {
        Log.d(TAG, "zdw adding device : name=" + device.nameString + " type="
                + device.type);
        int existsIndex = -1;
        for (int i = 0; i < sDevices.size(); i++) {
            if (sDevices.get(i).udnString.equals(device.udnString)) {
                existsIndex = i;
                break;
            }
        }
        if (existsIndex == -1) {
            sDevices.add(device);
        } else {
            sDevices.set(existsIndex, device);
        }
    }

    public TDevice findDeviceByUdn(String udn) {
        for (TDevice device : sDevices) {
            if (device.udnString.equals(udn)) {
                return device;
            }
        }
        return null;
    }

    public TDevice findDeviceByName(String name) {
        for (TDevice device : sDevices) {
            if (device.nameString.equals(name)) {
                return device;
            }
        }
        return null;
    }

    public void removeDevice(String udn) {
        for (int i = 0; i < sDevices.size(); i++) {
            if (sDevices.get(i).udnString.equals(udn)) {
                sDevices.remove(i);
            }
        }
    }
}

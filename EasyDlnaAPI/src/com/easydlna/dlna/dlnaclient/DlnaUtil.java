package com.easydlna.dlna.dlnaclient;

import android.util.Log;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;

public class DlnaUtil {
	static public String TAG = "DlnaUtil";

	static public boolean isNetAccesible() {
		if (getHostIp() == null)
			return false;
		return true;
	}

	static public String getHostIp() {
		try {
			Enumeration<NetworkInterface> netInterfaces = NetworkInterface
					.getNetworkInterfaces();
			InetAddress ip = null;
			while (netInterfaces.hasMoreElements()) {
				NetworkInterface ni = (NetworkInterface) netInterfaces
						.nextElement();
                if (!ni.getName().contains("eth") && !ni.getName().contains("wlan"))
                    continue;
				Enumeration<InetAddress> iaddrEnumeration = ni
						.getInetAddresses();
				ip = null;
				while (iaddrEnumeration.hasMoreElements()) {
					ip = (InetAddress) iaddrEnumeration.nextElement();
					if (ip instanceof Inet4Address) {
						break;
					} else {
						ip = null;
					}
				}
				if (ip == null) {
					continue;
				} else if (!ip.isLoopbackAddress()
						&& ip.getHostAddress().indexOf(":") == -1) {
					Log.e(TAG, "getHostip = " + ip.getHostAddress());
					return ip.getHostAddress();
				}
			}
			return null;
		} catch (Exception e) {
			Log.e(TAG, "fail to get host ip\n");
			return null;
		}
	}
}

package com.easydlna.application;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;

public class EasyDlnaUtil {
    final static public String DMSSERVICE_INTENT_NAME_STRING = "com.easydlna.dlna.service.DMSService";
    final static public String DMPSERVICE_INTENT_NAME_STRING = "com.easydlna.dlna.service.DMPService";

    final public static String TAG_STRING = "EasydlnaServer";
    private static int status = -1;
    static private boolean bDlnaStartd = false;
    static public String dlnaIp = null;
    static private int refCount = 0;
    static private int globalNotificationId = -1;
    static {
        try {
            System.loadLibrary("easydlna");
            System.loadLibrary("easydlna_dlna_plugin");
            status = 0;
        } catch (UnsatisfiedLinkError linkError) {
            Log.e(TAG_STRING, linkError.getMessage());
        }
    }

    static public void killEasyDlna() {
        android.os.Process.killProcess(android.os.Process.myPid());
        System.exit(10);
    }

    static public int startDLNA(Context context) {

        if (status != 0) {
            Log.e(TAG_STRING, "EasyDlna [startDLNA] is not init correctly");
            return -1;
        }

        if (bDlnaStartd) {
            ++refCount;
            return 0;
        }

        String thisIpString = getHostIp();
        if (thisIpString == null) {
            Log.e(TAG_STRING, "Ip is unavailable,startDLNA fails");
            return -1;
        }
        if (dlnaIp == null) {
            dlnaIp = thisIpString;
        }

        int nret = native_startDLNA(dlnaIp, 0);
        if (nret == -1) {
            Log.e(TAG_STRING, "native startDlna fails");
            return -1;
        }

        bDlnaStartd = true;
        ++refCount;
        return 0;
    }

    static public void stopDLNA(Context context) {

        if (status != 0) {
            Log.e(TAG_STRING, "EasyDlna [stopDLNA] is not init correctly");
            return;
        }

        if (!bDlnaStartd)
            return;

        --refCount;
        if (refCount == 0) {
            native_stopDLNA();
            bDlnaStartd = false;
            Log.e(TAG_STRING, "EasyDlna stopDlna");
        }

        Log.e(TAG_STRING, "native stopDlna");
    }

    static public int netStateChange(Context context) {
        if (status != 0) {
            Log.e(TAG_STRING,
                    "DlnaService [netStateChange] is not init correctly");
            return -1;
        }

        String newIpString = getHostIp();
        if (newIpString == null) {
            Log.e(TAG_STRING, "EasyDlna [netStateChange] ip is not available\n");
            if (bDlnaStartd) {
                stopDLNA(context);
            }
            return -1;
        }
        if (dlnaIp == null) {
            if (!bDlnaStartd) {
                startDLNA(context);
                return 0;
            } else {
                Log.e(TAG_STRING, "EasyDlna sth is wrong here");
                return 0;
            }

        }
        if (!dlnaIp.equals(newIpString)) {
            Log.e(TAG_STRING, "EasyDlna [netStateChange] ip changed to "
                    + newIpString);
            dlnaIp = newIpString;
            stopDLNA(context);
            startDLNA(context);
            return 0;
        }
        return 0;
    }

    static private native int native_startDLNA(String ip, int port);

    static private native int native_stopDLNA();

    static public String getHostIp() {
        try {// maybe add wifi p2p support
            Enumeration<NetworkInterface> netInterfaces = NetworkInterface
                    .getNetworkInterfaces();
            InetAddress ip = null;

            while (netInterfaces.hasMoreElements()) {
                NetworkInterface ni = (NetworkInterface) netInterfaces
                        .nextElement();
                if (!ni.getName().contains("eth")
                        && !ni.getName().contains("wlan"))
                    continue;
                Enumeration<InetAddress> iaddrEnumeration = ni
                        .getInetAddresses();

                ip = null;
                while (iaddrEnumeration.hasMoreElements()) {

                    ip = (InetAddress) iaddrEnumeration.nextElement();
                    if (ip instanceof Inet4Address)
                        break;
                    else
                        ip = null;
                }

                if (ip == null)
                    continue;

                if (!ip.isLoopbackAddress()
                        && ip.getHostAddress().indexOf(":") == -1) {
                    Log.e(TAG_STRING, "getHostip = " + ip.getHostAddress());
                    return ip.getHostAddress();
                }
            }
            return null;
        } catch (Exception e) {
            Log.e(TAG_STRING, "fail to get host ip :\n");
            return null;
        }

    }

    static public void putPreference(Context context, String key, Boolean value) {
        if (context != null) {
            SharedPreferences preferences = context.getSharedPreferences(
                    EasyDlnaUtil.TAG_STRING, 0);
            Editor editor = preferences.edit();
            editor.putBoolean(key, value);
            editor.commit();
        }
    }

    static public void putPreference(Context context, String key, String value) {
        if (context != null) {
            SharedPreferences preferences = context.getSharedPreferences(
                    EasyDlnaUtil.TAG_STRING, 0);
            Editor editor = preferences.edit();
            editor.putString(key, value);
            editor.commit();
        }
    }

    static public int getNotificationId() {
        globalNotificationId++;
        return globalNotificationId;
    }
}

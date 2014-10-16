package com.easydlna.application;

import com.easydlna.dlna.service.DMCService;
import com.easydlna.dlna.service.DMRService;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.wifi.WifiManager;
import android.util.Log;

public class DmpBroadcastReceiver extends BroadcastReceiver {
    final private static String DLNABR_TAG_STRING = "DmpBroadcastReceiver";

    public void onReceive(Context context, Intent intent) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(
                EasyDlnaUtil.TAG_STRING, 0);
        if (sharedPreferences == null)
            return;
        boolean bDMRAutoStart = sharedPreferences.getBoolean(
                DMRService.DMR_AUTOSTART_KEY, false/* true */);
        boolean bDMCAutoStart = sharedPreferences.getBoolean(
                DMCService.DMC_AUTOSTART_KEY, false/* true */);

        String actionName = intent.getAction();
        if (actionName.equals(Intent.ACTION_BOOT_COMPLETED)) {
            if (bDMRAutoStart) {
                context.startService(new Intent(
                        "com.easydlna.dlna.service.DMRService"));
            }
            if (bDMCAutoStart) {
                context.startService(new Intent(
                        "com.easydlna.dlna.service.DMCService"));
            }
        } else if (actionName.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
            Log.i(DLNABR_TAG_STRING, "Receive broadcast to Dmpservice : "
                    + WifiManager.NETWORK_STATE_CHANGED_ACTION);
            EasyDlnaUtil.netStateChange(context);
        } else if (actionName
                .equals(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION)) {
            Log.i(DLNABR_TAG_STRING, "Receive broadcast to Dmpservice : "
                    + WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION);
            EasyDlnaUtil.netStateChange(context);
        } else if (actionName.equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
            Log.i(DLNABR_TAG_STRING, "Receive broadcast to Dmpservice : "
                    + WifiManager.WIFI_STATE_CHANGED_ACTION);
            EasyDlnaUtil.netStateChange(context);
        } else if (actionName.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED)) {
            Log.i(DLNABR_TAG_STRING, "Receive broadcast to Dmpservice : "
                    + Intent.ACTION_AIRPLANE_MODE_CHANGED);
            EasyDlnaUtil.netStateChange(context);
        }
    }
}

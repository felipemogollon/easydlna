package com.easydlna.application;

import com.easydlna.dlna.service.IDMRService;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.os.Message;
import android.os.RemoteException;
import android.widget.Toast;

public class ActivityDMR extends Activity {
    private String TAG = "DMP_UI";
    private static String DMRSERVICE_INTENT_NAME_STRING = "com.easydlna.dlna.service.DMRService";
    private volatile IDMRService dmrService = null;
    private ServiceConnection serviceConnection = null;
    final private int DMR_DOWN = 0;
    final private int DMR_UP = 1;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dmr);
        Button BtnDmr = (Button) findViewById(R.id.BtnDmr);
        BtnDmr.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                StartDmr();
            }
        });
    }

    protected void onDestroy() {
        super.onDestroy();
    }

    private void StartDmr() {
        if (checkNetwork(getString(R.string.msg_network_dmr))) {
            serviceConnection = new ServiceConnection() {
                public void onServiceConnected(ComponentName name,
                        IBinder service) {
                    Log.i(TAG, "DMR Service connected");
                    if (dmrService == null) {
                        dmrService = IDMRService.Stub.asInterface(service);
                        handler.sendEmptyMessage(DMR_UP);
                    }
                    internalExecuteDMR();
                }

                public void onServiceDisconnected(ComponentName name) {
                    Log.i(TAG, "DMS Service disconnected");
                    dmrService = null;
                    handler.sendEmptyMessage(DMR_DOWN);
                }
            };
            bindService(new Intent(DMRSERVICE_INTENT_NAME_STRING),
                    serviceConnection, Context.BIND_AUTO_CREATE);
        }
    }

    protected int internalExecuteDMR() {
        try {
            dmrService.start_stop_DMR(true);
        } catch (RemoteException e) {
            e.printStackTrace();
            return -1;
        }
        return 0;
    }

    private boolean checkNetwork(String msg) {
        ConnectivityManager cnManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);

        NetworkInfo availeInfo = cnManager.getActiveNetworkInfo();
        if (availeInfo == null
                || (availeInfo != null && !availeInfo.isAvailable())) {
            new AlertDialog.Builder(this).setPositiveButton(R.string.ok, null)
                    .setMessage(msg).show();
            return false;
        }
        return true;
    }

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case DMR_DOWN:
                Toast.makeText(ActivityDMR.this, R.string.dmr_down,
                        Toast.LENGTH_SHORT).show();
                break;
            case DMR_UP:
                Toast.makeText(ActivityDMR.this, R.string.dmr_start,
                        Toast.LENGTH_SHORT).show();
                break;
            default:
                break;
            }
            super.handleMessage(msg);
        }
    };
}

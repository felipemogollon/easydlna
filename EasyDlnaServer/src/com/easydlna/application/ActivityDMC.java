package com.easydlna.application;

import com.easydlna.dlna.service.IDMCService;

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

public class ActivityDMC extends Activity {
    private String TAG = "DMC_UI";
    private static String DMCSERVICE_INTENT_NAME_STRING = "com.easydlna.dlna.service.DMCService";
    private volatile IDMCService dmcService = null;
    private ServiceConnection serviceConnection = null;
    final private int DMC_DOWN = 0;
    final private int DMC_UP = 1;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dmc);
        Button BtnDmc = (Button) findViewById(R.id.BtnDmc);
        BtnDmc.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                StartDmc();
            }
        });
    }

    protected void onDestroy() {
        super.onDestroy();
    }

    private void StartDmc() {
        if (checkNetwork(getString(R.string.msg_network_dmc))) {
            serviceConnection = new ServiceConnection() {
                public void onServiceConnected(ComponentName name,
                        IBinder service) {
                    Log.i(TAG, "DMC Service connected");
                    if (dmcService == null) {
                        dmcService = IDMCService.Stub.asInterface(service);
                        handler.sendEmptyMessage(DMC_UP);
                    }
                    internalExecuteDMC();
                }

                public void onServiceDisconnected(ComponentName name) {
                    Log.i(TAG, "DMS Service disconnected");
                    dmcService = null;
                    handler.sendEmptyMessage(DMC_DOWN);
                }
            };
            bindService(new Intent(DMCSERVICE_INTENT_NAME_STRING),
                    serviceConnection, Context.BIND_AUTO_CREATE);
        }
    }

    protected int internalExecuteDMC() {
        try {
            dmcService.start_stop_DMC(true);
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
            case DMC_DOWN:
                Toast.makeText(ActivityDMC.this, R.string.dmc_down,
                        Toast.LENGTH_SHORT).show();
                break;
            case DMC_UP:
                Toast.makeText(ActivityDMC.this, R.string.dmc_start,
                        Toast.LENGTH_SHORT).show();
                break;
            default:
                break;
            }
            super.handleMessage(msg);
        }
    };
}

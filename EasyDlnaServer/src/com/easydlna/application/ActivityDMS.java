package com.easydlna.application;

import com.easydlna.application.Command.ICommandCompleted;
import com.easydlna.dlna.service.DMSService;
import com.easydlna.dlna.service.IDMSService;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.Notification.Builder;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.CompoundButton.OnCheckedChangeListener;

public class ActivityDMS extends Activity implements OnClickListener,
        OnCheckedChangeListener, ICommandCompleted {
    private static final String TAG = "DMS_UI";

    private EditText mDmsName;
    private TextView videoTextView;
    private TextView audioTextView;
    private TextView imageTextView;
    private TextView autoStartTextView;

    private Button mDmsOpen;
    private Button mDmsClose;
    private Button mDmsBroadcast;

    private CheckBox mTriggerAutoStart;

    private CheckBox mTriggerVideo;
    private CheckBox mTriggerAudio;
    private CheckBox mTriggerImage;

    private LinearLayout mDmsRunLayout;

    private volatile IDMSService mServices = null;
    private ServiceConnection serviceConnection = null;
    private static int notifycationId = 0;
    final public static int DMS_DOWN = 0;
    final public static int DMS_UP = 1;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dms);
        setTitle(null);

        mDmsOpen = (Button) findViewById(R.id.TriggetDMS);
        mDmsOpen.setOnClickListener(this);

        mDmsRunLayout = (LinearLayout) findViewById(R.id.TwoButtons);

        mDmsClose = (Button) findViewById(R.id.BtnCloseDMS);
        mDmsClose.setOnClickListener(this);

        mDmsBroadcast = (Button) findViewById(R.id.BtnBroadcast);
        mDmsBroadcast.setOnClickListener(this);

        mDmsName = (EditText) findViewById(R.id.EditDMSName);
        videoTextView = (TextView) findViewById(R.id.video);
        audioTextView = (TextView) findViewById(R.id.audio);
        imageTextView = (TextView) findViewById(R.id.image);
        autoStartTextView = (TextView) findViewById(R.id.auto_start);

        mTriggerAutoStart = (CheckBox) findViewById(R.id.TriggerAutoStart);
        mTriggerAutoStart.setOnCheckedChangeListener(this);

        mTriggerVideo = (CheckBox) findViewById(R.id.TriggerVideo);
        mTriggerVideo.setOnCheckedChangeListener(this);

        mTriggerAudio = (CheckBox) findViewById(R.id.TriggerAudio);
        mTriggerAudio.setOnCheckedChangeListener(this);

        mTriggerImage = (CheckBox) findViewById(R.id.TriggerImage);
        mTriggerImage.setOnCheckedChangeListener(this);

        startDMS();
    }

    private void updateUiStatus(boolean bEnabled) {
        boolean servicesNotStarted = !bEnabled;
        if (mServices != null) {
            try {
                servicesNotStarted = !mServices.isStarted();
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        Log.d(TAG, servicesNotStarted ? "DMS not ready" : "DMS is running");
        mTriggerAutoStart.setEnabled(servicesNotStarted);
        mTriggerVideo.setEnabled(servicesNotStarted);
        mTriggerAudio.setEnabled(servicesNotStarted);
        mTriggerImage.setEnabled(servicesNotStarted);

        videoTextView.setEnabled(servicesNotStarted);
        audioTextView.setEnabled(servicesNotStarted);
        imageTextView.setEnabled(servicesNotStarted);

        autoStartTextView.setEnabled(servicesNotStarted);

        mDmsName.setEnabled(servicesNotStarted);
        mDmsOpen.setEnabled(mServices != null);
        if (mServices != null) {
            mDmsOpen.setVisibility(servicesNotStarted ? View.VISIBLE
                    : View.GONE);
            mDmsRunLayout.setVisibility(servicesNotStarted ? View.GONE
                    : View.VISIBLE);
        } else {
            mDmsOpen.setVisibility(View.VISIBLE);
            mDmsRunLayout.setVisibility(View.GONE);
        }

        // Set Checking
        if (mServices != null) {
            try {
                mDmsName.setText(mServices.getServerFriendlyName());
                int t = mServices.getSharedType();
                mTriggerVideo
                        .setChecked((t & DMSService.SHARED_TYPE_VIDEO) != 0);
                mTriggerAudio
                        .setChecked((t & DMSService.SHARED_TYPE_AUDIO) != 0);
                mTriggerImage
                        .setChecked((t & DMSService.SHARED_TYPE_IMAGE) != 0);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        SharedPreferences preferences = this.getSharedPreferences(
                EasyDlnaUtil.TAG_STRING, 0);
        mTriggerAutoStart.setChecked(preferences.getBoolean(
                DMSService.DMS_AUTOSTART_KEY, true));
    }

    public Handler handler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case DMS_DOWN:
                Toast.makeText(ActivityDMS.this, R.string.dms_down,
                        Toast.LENGTH_SHORT).show();
                updateUiStatus(false);
                break;
            case DMS_UP:
                Toast.makeText(ActivityDMS.this, R.string.dms_start,
                        Toast.LENGTH_SHORT).show();
                updateUiStatus(true);
                break;
            default:
                break;
            }
            super.handleMessage(msg);
        }
    };

    private void updateSharedType() {
        int sharedType = 0;
        if (mTriggerVideo.isChecked())
            sharedType |= DMSService.SHARED_TYPE_VIDEO;
        if (mTriggerAudio.isChecked())
            sharedType |= DMSService.SHARED_TYPE_AUDIO;
        if (mTriggerImage.isChecked())
            sharedType |= DMSService.SHARED_TYPE_IMAGE;
        int oldType;
        try {
            if (mServices != null) {
                oldType = mServices.getSharedType();
                if (oldType == sharedType)
                    return;
                CMDSetContents cmdAddContents = new CMDSetContents(sharedType,
                        this, mServices);
                cmdAddContents.execute();
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    private boolean networkAvailable() {
        ConnectivityManager cnManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo availeInfo = cnManager.getActiveNetworkInfo();
        if (availeInfo == null
                || (availeInfo != null && !availeInfo.isAvailable())) {
            return false;
        }
        return true;
    }

    private void startDMS() {
        if (!networkAvailable()) {
            new AlertDialog.Builder(this).setPositiveButton("OK", null)
                    .setMessage(R.string.network_warning).show();
            updateUiStatus(true);
            return;
        }
        // EnableUIComponents(true);
        if (serviceConnection == null) {
            serviceConnection = new ServiceConnection() {

                public void onServiceConnected(ComponentName name,
                        IBinder service) {
                    Log.i(TAG, "DMS Service connected");
                    if (mServices == null) {
                        mServices = IDMSService.Stub.asInterface(service);
                        handler.sendEmptyMessage(DMS_UP);
                    }
                }

                public void onServiceDisconnected(ComponentName name) {
                    Log.i(TAG, "DMS Service disconnected");
                    mServices = null;
                    handler.sendEmptyMessage(DMS_DOWN);
                }
            };

            startService(new Intent(EasyDlnaUtil.DMSSERVICE_INTENT_NAME_STRING));
            bindService(new Intent(EasyDlnaUtil.DMSSERVICE_INTENT_NAME_STRING),
                    serviceConnection, Context.BIND_AUTO_CREATE);
        }
    }

    private void stopDMS() {
        if (serviceConnection != null) {
            unbindService(serviceConnection);
            serviceConnection = null;
            stopService(new Intent(EasyDlnaUtil.DMSSERVICE_INTENT_NAME_STRING));
        }
    }

    protected void onDestroy() {
        super.onDestroy();
        if (serviceConnection != null) {
            unbindService(serviceConnection);
            serviceConnection = null;
        }
        try {
            if (mServices != null && !mServices.isStarted()) {
                stopService(new Intent(
                        EasyDlnaUtil.DMSSERVICE_INTENT_NAME_STRING));
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void onClick(View v) {
        int id = v.getId();
        Command cmdExecute = null;
        if (id == mDmsOpen.getId()) {
            new CMDSetName(mDmsName.getText().toString(), false, this,
                    mServices).execute();
            cmdExecute = new CMDTriggerDms(true, this, mServices);
        } else if (id == mDmsBroadcast.getId()) {
            cmdExecute = new CMDBroadcast(1800, this, mServices);
        } else if (id == mDmsClose.getId()) {
            cmdExecute = new CMDTriggerDms(false, this, mServices);
        }

        if (cmdExecute != null) {
            cmdExecute.execute();
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        int id = buttonView.getId();
        Command cmdExcute = null;
        if (id == mTriggerAutoStart.getId()) {
            cmdExcute = new CMDSetAutoStart(isChecked, this, mServices);
        } else if (id == mTriggerVideo.getId() || id == mTriggerAudio.getId()
                || id == mTriggerImage.getId()) {
            updateSharedType();
        }
        if (cmdExcute != null) {
            cmdExcute.execute();
        }
    }

    @Override
    public boolean commandCompleted(Command command, int result) {

        String toast = null;
        switch (command.cmdType) {
        case Command.CMD_TYPE_SETAUTOSTART:
            toast = "set auto start";
            break;
        case Command.CMD_TYPE_SETCONTENTS:
            toast = "shared-dir set";
            break;
        case Command.CMD_TYPE_START_STOP_DMS:
            CMDTriggerDms dmsCmd = (CMDTriggerDms) command;
            boolean bStart = dmsCmd.bStarted;
            toast = bStart ? "start DMS" : "stop DMS";
            if (bStart) {
                showNotification();
            } else {
                removeNotification(getApplicationContext());
            }

            updateUiStatus(!bStart);
            break;
        case Command.CMD_TYPE_SETNAME:
            toast = "set DMS name";
            break;
        default:
            break;
        }
        if (toast != null)
            Toast.makeText(this, toast + (result == 0 ? " succee" : " fail"),
                    Toast.LENGTH_SHORT).show();
        return false;
    }

    private void showNotification() {
        NotificationManager notiManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        Builder builder = new Notification.Builder(this);
        Intent intent = new Intent(this, ActivityDMS.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP
                | Intent.FLAG_ACTIVITY_NEW_TASK);
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                intent, PendingIntent.FLAG_UPDATE_CURRENT);
        builder.setSmallIcon(R.drawable.status_bar_icon).setContentTitle("DMS")
                .setContentText("DMS is running")
                .setWhen(System.currentTimeMillis())
                .setContentIntent(contentIntent);
        Notification notification = builder.getNotification();
        notification.flags = Notification.FLAG_NO_CLEAR;
        notifycationId = EasyDlnaUtil.getNotificationId();
        notiManager.notify(notifycationId, notification);
    }

    static public void removeNotification(Context context) {
        NotificationManager notiManager = (NotificationManager) context
                .getSystemService(NOTIFICATION_SERVICE);
        notiManager.cancel(notifycationId);
    }
}

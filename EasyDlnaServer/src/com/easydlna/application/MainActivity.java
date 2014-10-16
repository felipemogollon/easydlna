package com.easydlna.application;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.StrictMode;
import android.view.Menu;

import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;

import android.widget.ImageButton;

import android.widget.Toast;

public class MainActivity extends Activity implements OnClickListener {

    private ImageButton mDMS;

    // private Button mDMR;
    // private Button mDMC;
    // private TextView mIp;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent i = getIntent();
        if (i != null) {
            Bundle bundle = i.getExtras();
            if (bundle != null && bundle.getInt("exit") == 1) {
                EasyDlnaUtil.killEasyDlna();
                finish();
                return;
            }
        }

        setContentView(R.layout.main_activity);
        setTitle(null);
        mDMS = (ImageButton) findViewById(R.id.button_goto_dms);
        mDMS.setOnClickListener(this);
        // mDMR = (Button) findViewById(R.id.button_goto_dmr);
        // mDMR.setOnClickListener(this);
        // mDMC = (Button) findViewById(R.id.button_goto_dmc);
        // mDMC.setOnClickListener(this);
        // mIp = (TextView) findViewById(R.id.ip_address);
        // mIp.setText(getIpAddress());
        StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder()
                .detectDiskReads().detectDiskWrites().detectNetwork() // or
                                                                      // .detectAll()
                                                                      // for all
                                                                      // detectable
                                                                      // problems
                .penaltyLog().build());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add("Exit");
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        ActivityDMS.removeNotification(getApplicationContext());
        EasyDlnaUtil.killEasyDlna();
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == mDMS.getId()) {
            goDMS();
            // } else if (id == mDMR.getId()) {
            // goDMR();
            // } else if (id == mDMC.getId()) {
            // goDMC();
        }
    }

    private void goDMS() {
        Intent intent = new Intent(this,
                com.easydlna.application.ActivityDMS.class);
        startActivity(intent);
    }

    private void goDMR() {
        Intent intent = new Intent(this,
                com.easydlna.application.ActivityDMR.class);
        startActivity(intent);
    }

    private void goDMC() {
        Intent intent = new Intent(this,
                com.easydlna.application.ActivityDMC.class);
        startActivity(intent);
    }

    public void showToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

}

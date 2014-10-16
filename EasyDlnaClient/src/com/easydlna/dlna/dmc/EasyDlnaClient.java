package com.easydlna.dlna.dmc;

import java.util.ArrayList;
import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

import com.easydlna.dlna.core.CacheManager;
import com.easydlna.dlna.core.ContentDevice;
import com.easydlna.dlna.core.ContentInfoEx;
import com.easydlna.dlna.core.DmcClientWraper;
import com.easydlna.dlna.core.RenderDevice;
import com.easydlna.dlna.dlnaclient.DMCClient;
import com.easydlna.dlna.service.DlnaDevice;
import com.easydlna.dlna.dmc.R;

public class EasyDlnaClient extends ListActivity implements
        DmcClientWraper.IDmcBrowserClient {

    private static final String TAG = "Dlna:DmcApp.DlnaBrowser";

    private static final int DEVICE_STATE_ONLINE = 1;

    private static final int DEVICE_STATE_OFFLINE = 0;

    private static final int MSG_UPDATE_LIST = 1;

    private static final int MSG_SET_UI_ENABLE = 2;

    private static final String KEY_IS_ENABLED = "is_enabled";

    private ArrayAdapter<String> devices_adapter;

    private PopupWindow popupWindow;

    private static String REMOTE_DESKTOP_ACTIVITY = "com.easydlna.RemoteDesktopActivity";

    private static String SERVER_COMPONENT = "com.easydlna.application";
    private static String SERVER_MAINACTIVITY = "com.easydlna.application.MainActivity";

    private int[] image_ids = { R.drawable.video_normal,
            R.drawable.music_normal, R.drawable.image_normal};
    private int[] image_focus_ids = { R.drawable.video_focus,
            R.drawable.music_focus, R.drawable.image_focus};

    private class OnMediaTypeSelectorClickedListener implements
            View.OnClickListener {
        public void onClick(View v) {
            int type = Integer.parseInt(v.getTag().toString());
            mCurrentMediaType = type;
            updateUiForMediaType(mCurrentMediaType);
            onMediaTypeChanged(mCurrentMediaType);
        }
    }

    private ContentInfoAdapter mMediaListAdapter = null;

    private ArrayList<ImageButton> mMediaTypeSelectors = new ArrayList<ImageButton>();

    private TextView mServerListSelector;

    private ContentDevice mCurrentContentDevice = null;

    private int mCurrentMediaType = ContentInfoEx.MEDIA_TYPE_AUDIO;

    private Bitmap[] mDefaultThumbnails = new Bitmap[3];

    private OnMediaTypeSelectorClickedListener mOnMediaTypeSelectorClickedListener = new OnMediaTypeSelectorClickedListener();

    private Handler mMainHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MSG_UPDATE_LIST:
                if (mCurrentContentDevice != null) {
                    mServerListSelector
                            .setText(mCurrentContentDevice.nameString);
                    mMediaListAdapter = new ContentInfoAdapter(
                            EasyDlnaClient.this,
                            mCurrentContentDevice
                                    .getContents(mCurrentMediaType),
                            mDefaultThumbnails[mCurrentMediaType]);
                    mMediaListAdapter.setNotifyOnChange(true);
                    setUiEnable(true);
                } else {
                    mMediaListAdapter = null;
                    setUiEnable(false);
                }

                getListView().setAdapter(mMediaListAdapter);
                break;
            case MSG_SET_UI_ENABLE:
                boolean enable = msg.getData().getBoolean(KEY_IS_ENABLED);
                for (ImageButton mediaTypeSelector : mMediaTypeSelectors) {
                    mediaTypeSelector.setEnabled(enable);
                    mediaTypeSelector.setClickable(enable);

                    // if(!enable) {
                    mediaTypeSelector.setBackgroundColor(0xEE000000);
                    // // mediaTypeSelector.setTextColor(0xFF555555);
                    // } else {
                    // updateUiForMediaType(mCurrentMediaType);
                    // }
                }
                updateUiForMediaType(mCurrentMediaType);
                getListView().setEnabled(enable);
                getListView().setClickable(enable);
                // getListView().setBackgroundColor(enable ? Color.WHITE :
                // Color.DKGRAY);
                // mServerListSelector.setEnabled(enable);
                // mServerListSelector.setClickable(enable);
                break;
            default:
                break;
            }
        }
    };

    private void setUiEnable(boolean enabled) {
        Message msg = mMainHandler.obtainMessage(MSG_SET_UI_ENABLE);
        Bundle data = new Bundle();
        data.putBoolean(KEY_IS_ENABLED, enabled);
        msg.setData(data);
        mMainHandler.sendMessage(msg);
    }

    private void updateUiForMediaType(int mediaType) {
        for (int i = 0; i < mMediaTypeSelectors.size(); i++) {
            ImageButton button = mMediaTypeSelectors.get(i);
            if (i == mediaType) {
                button.setImageDrawable(getResources().getDrawable(
                        image_focus_ids[i]));
            } else {
                button.setImageDrawable(getResources()
                        .getDrawable(image_ids[i]));
            }
        }
    }

    private void initRefs() {
        mMediaTypeSelectors.clear();
        mMediaTypeSelectors
                .add((ImageButton) findViewById(R.id.media_type_selector_video));
        mMediaTypeSelectors.get(ContentInfoEx.MEDIA_TYPE_VIDEO).setTag(
                "" + ContentInfoEx.MEDIA_TYPE_VIDEO);
        mMediaTypeSelectors.get(ContentInfoEx.MEDIA_TYPE_VIDEO)
                .setOnClickListener(mOnMediaTypeSelectorClickedListener);

        mMediaTypeSelectors
                .add((ImageButton) findViewById(R.id.media_type_selector_audio));
        mMediaTypeSelectors.get(ContentInfoEx.MEDIA_TYPE_AUDIO).setTag(
                "" + ContentInfoEx.MEDIA_TYPE_AUDIO);
        mMediaTypeSelectors.get(ContentInfoEx.MEDIA_TYPE_AUDIO)
                .setOnClickListener(mOnMediaTypeSelectorClickedListener);

        mMediaTypeSelectors
                .add((ImageButton) findViewById(R.id.media_type_selector_image));
        mMediaTypeSelectors.get(ContentInfoEx.MEDIA_TYPE_IMAGE).setTag(
                "" + ContentInfoEx.MEDIA_TYPE_IMAGE);
        mMediaTypeSelectors.get(ContentInfoEx.MEDIA_TYPE_IMAGE)
                .setOnClickListener(mOnMediaTypeSelectorClickedListener);

        mServerListSelector = (TextView) findViewById(R.id.media_server_selector);
        mServerListSelector.setText(R.string.text_server_select_button_default);
        mServerListSelector.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                showWindow(arg0);
            }
        });

        mDefaultThumbnails[ContentInfoEx.MEDIA_TYPE_AUDIO] = BitmapFactory
                .decodeResource(getResources(), R.drawable.musics);
        mDefaultThumbnails[ContentInfoEx.MEDIA_TYPE_IMAGE] = BitmapFactory
                .decodeResource(getResources(), R.drawable.images);
        mDefaultThumbnails[ContentInfoEx.MEDIA_TYPE_VIDEO] = BitmapFactory
                .decodeResource(getResources(), R.drawable.videos);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.main);
        initRefs();
        updateUiForMediaType(mCurrentMediaType);
        mMainHandler.sendEmptyMessage(MSG_UPDATE_LIST);
        DmcClientWraper.setBrowserClient(this);
        DmcClientWraper.startDMC(this);
        setupLocalRender();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        DmcClientWraper.stopDMC();
        Log.d(TAG, "shutting down thread pool");
        Toast.makeText(this,
                getResources().getString(R.string.message_closing_thread_pool),
                Toast.LENGTH_SHORT);
        // DownloadManager.shutDown();
        Toast.makeText(this,
                getResources().getString(R.string.message_thread_pool_closed),
                Toast.LENGTH_SHORT);
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    private void setupLocalRender() {
        DlnaDevice device = new DlnaDevice();
        device.nameString = "Local Render";
        device.udnString = DlnaController.UDN_LOCAL_RENDER;
        RenderDevice.sDevices.addDevice(new RenderDevice(device));
    }

    private void onMediaTypeChanged(int mediaType) {
        mCurrentMediaType = mediaType;
        mMainHandler.sendEmptyMessage(MSG_UPDATE_LIST);
        Log.d(TAG, "Media Type changed to: " + mediaType);
    }

    @Override
    public void DMCServiceStatusNotify(int status) {
        Log.d(TAG, "DMCServiceStatusNotify:" + status);
        if (status == DMCClient.DMC_SERVICE_DOWN
                || status == DMCClient.DMC_SERVICE_ERROR) {
            Toast.makeText(this,
                    getResources()
                            .getString(R.string.message_dmcservice_closed),
                    Toast.LENGTH_SHORT);
            finish();
        } else {
            Toast.makeText(this,
                    getResources()
                            .getString(R.string.message_dmcservice_opened),
                    Toast.LENGTH_LONG);
        }
    }

    @Override
    public void DlnaDeviceStatusNotify(DlnaDevice device) {
        Log.d(TAG, device.nameString + " " + device.stateNow);
        if (ContentDevice.isContentDevice(device)) {
            switch (device.stateNow) {
            case DEVICE_STATE_ONLINE:
                if (device.nameString.contains("EasyShare")
                        || device.nameString.contains("Easyshare"))
                    device.bSupportRemoteDesktop = 1;
                ContentDevice contentDevice = new ContentDevice(device);
                ContentDevice.sDevices.addDevice(contentDevice);
                if (mCurrentContentDevice == null) {
                    mCurrentContentDevice = contentDevice;
                    CacheManager.sImage.reset();
                    CacheManager.sImageThumbnail.reset();
                    CacheManager.sVideoThumbnail.reset();
                    mMainHandler.sendEmptyMessage(MSG_UPDATE_LIST);
                }
                break;
            case DEVICE_STATE_OFFLINE:
                ContentDevice.sDevices.removeDevice(device.udnString);
                if (mCurrentContentDevice != null
                        && device.udnString
                                .equals(mCurrentContentDevice.udnString)) {
                    if (ContentDevice.sDevices.getDeviceCount() != 0) {
                        mMainHandler.post(new Runnable() {
                            public void run() {
                                // showServerSelectDialog();
                                showWindow(mServerListSelector);
                            }
                        });
                    } else {
                        mCurrentContentDevice = null;
                        mMainHandler.sendEmptyMessage(MSG_UPDATE_LIST);
                    }
                }
                break;
            default:
                Log.e(TAG, "unkown device state : " + device.stateNow);
                break;
            }
        } else if (RenderDevice.isRenderDevice(device)) {
            switch (device.stateNow) {
            case DEVICE_STATE_ONLINE:
                RenderDevice.sDevices.addDevice(new RenderDevice(device));
                break;
            case DEVICE_STATE_OFFLINE:
                RenderDevice.sDevices.removeDevice(device.udnString);
                break;
            default:
                Log.e(TAG, "unkown device state : " + device.stateNow);
                break;
            }
        }
    }

    @Override
    public void DlnaFilesNotify(String udn, int videoCount, int audioCount,
            int imageCount) {
        final ContentDevice device = ContentDevice.sDevices
                .findDeviceByUdn(udn);
        Log.d(TAG, " " + videoCount + " " + audioCount + " " + imageCount);
        if (device != null) {
            final int fAudioCount = audioCount;
            final int fVideoCount = videoCount;
            final int fImageCount = imageCount;
            mMainHandler.post(new Runnable() {
                public void run() {
                    device.updateContent(DmcClientWraper.sClient, fAudioCount,
                            fImageCount, fVideoCount);
                    if (fImageCount != 0) {
                        Log.d(TAG, "log");
                    }
                    if (mCurrentContentDevice != null
                            && device.udnString
                                    .equals(mCurrentContentDevice.udnString)
                            && mMediaListAdapter != null) {
                        CacheManager.updateImageKeyList(device
                                .getContents(ContentInfoEx.MEDIA_TYPE_IMAGE));
                        CacheManager.updateImageThumbnailKeyList(device
                                .getContents(ContentInfoEx.MEDIA_TYPE_IMAGE));
                        CacheManager.updateVideoThumbnailKeyList(device
                                .getContents(ContentInfoEx.MEDIA_TYPE_VIDEO));
                        mMediaListAdapter.notifyDataSetChanged();
                    }
                }
            });
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case R.id.rescan_devices:

            // first refresh dms that is already online
            ArrayList<DlnaDevice> dmsList = new ArrayList<DlnaDevice>();
            DmcClientWraper.sClient.getOnLineDevices(dmsList,
                    DlnaDevice.DEVICE_TYPE_DMS);
            for (DlnaDevice device : dmsList) {
                DlnaDeviceStatusNotify(device);
            }

            DmcClientWraper.sClient.searchDevice(DlnaDevice.DEVICE_TYPE_DMS);
            break;
        case R.id.rescan_contents:
            if (mCurrentContentDevice != null) {
                DmcClientWraper.sClient
                        .refreshContents(mCurrentContentDevice.udnString);
            }
            break;
        case R.id.rescan_renders:

            // first refresh dmr that is already online
            ArrayList<DlnaDevice> dmrList = new ArrayList<DlnaDevice>();
            DmcClientWraper.sClient.getOnLineDevices(dmrList,
                    DlnaDevice.DEVICE_TYPE_DMR);
            for (DlnaDevice device : dmrList) {
                DlnaDeviceStatusNotify(device);
            }

            DmcClientWraper.sClient.searchDevice(DlnaDevice.DEVICE_TYPE_DMR);
            break;
        case R.id.exit_app:
            // forcelly exit dlna server if we run in the same phone
            try {
                ComponentName temp = new ComponentName(SERVER_COMPONENT,
                        SERVER_MAINACTIVITY);
                Intent intent = new Intent();
                intent.setComponent(temp);
                intent.putExtra("exit", 1);
                startActivityForResult(intent, 100);
            } catch (Exception e) {
                // TODO: handle exception
            }

        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        // kill myself to get a chance to restart freshly
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    private void startRemoteDesktop() {
        if (mCurrentContentDevice == null) {
            Toast.makeText(this, "Cann't find the server", Toast.LENGTH_SHORT)
                    .show();
            return;
        }
        if (mCurrentContentDevice.bSupportRemoteDesktop == 0) {
            Toast.makeText(this, "Remote Server does not support DesktopShare",
                    Toast.LENGTH_SHORT).show();
            return;
        }
        Intent intent = new Intent(REMOTE_DESKTOP_ACTIVITY);
        // Intent intent = new Intent("com.easydlna.RDNativeActivity");
        intent.putExtra("ip", mCurrentContentDevice.baseUrlString);
        startActivity(intent);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        return super.onPrepareOptionsMenu(menu);
    }

    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
        super.onListItemClick(l, v, position, id);
        ContentInfoEx contentInfo = mMediaListAdapter.getItem(position);
        showRenderDialog(contentInfo);
    }

    private void showRenderDialog(final ContentInfoEx contentToRender) {
        final int index = mCurrentContentDevice.getContents(mCurrentMediaType)
                .indexOf(contentToRender);
        if (index < 0) {
            return;
        }
        final Context thisContext = this;
        new AlertDialog.Builder(this)
                .setItems(RenderDevice.sDevices.getOnlineDeviceNames(),
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                    int which) {
                                AlertDialog thisDialog = (AlertDialog) dialog;
                                String deviceName = (String) thisDialog
                                        .getListView().getAdapter()
                                        .getItem(which);
                                RenderDevice renderDevice = RenderDevice.sDevices
                                        .findDeviceByName(deviceName);
                                Intent intent = new Intent(thisContext,
                                        DlnaController.class);
                                intent.putExtra(
                                        DlnaController.EXTRA_CONTENT_DEVICE_UDN,
                                        mCurrentContentDevice.udnString);
                                intent.putExtra(
                                        DlnaController.EXTRA_RENDER_DEVICE_UDN,
                                        renderDevice.udnString);
                                intent.putExtra(
                                        DlnaController.EXTRA_CONTENT_TYPE,
                                        mCurrentMediaType);
                                intent.putExtra(
                                        DlnaController.EXTRA_CONTENT_INDEX,
                                        index);
                                thisContext.startActivity(intent);
                                thisDialog.dismiss();
                            }
                        }).setTitle(R.string.text_render_select_dialog_title)
                .create().show();
    }

    private void showWindow(View parent) {
        ListView listView = null;
        devices_adapter = new ArrayAdapter<String>(this,
                R.layout.device_list_item,
                ContentDevice.sDevices.getOnlineDeviceNames());

        LayoutInflater layoutInflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        View view = (View) layoutInflater.inflate(R.layout.device_list, null);
        listView = (ListView) view.findViewById(R.id.list);

        listView.setAdapter(devices_adapter);
        if (popupWindow == null) {

            popupWindow = new PopupWindow(view, 120, LayoutParams.WRAP_CONTENT);

        }

        popupWindow.setFocusable(true);
        popupWindow.setOutsideTouchable(true);
        popupWindow.setBackgroundDrawable(new BitmapDrawable());
        WindowManager windowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        int xPos = windowManager.getDefaultDisplay().getWidth()
                - popupWindow.getWidth();

        popupWindow.showAsDropDown(parent, xPos, 0);

        listView.setOnItemClickListener(new OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> adapterView, View view,
                    int position, long id) {
                String deviceName = (String) ((TextView) view).getText();
                ContentDevice selectedDevice = ContentDevice.sDevices
                        .findDeviceByName(deviceName);
                if (mCurrentContentDevice == null
                        || !mCurrentContentDevice.udnString
                                .equals(selectedDevice.udnString)) {
                    mCurrentContentDevice = selectedDevice;

                    CacheManager.sImage.reset();
                    CacheManager.sImageThumbnail.reset();
                    CacheManager.sVideoThumbnail.reset();

                    mMainHandler.sendEmptyMessage(MSG_UPDATE_LIST);
                }
                if (popupWindow != null) {
                    popupWindow.dismiss();
                }
            }
        });
    }

}
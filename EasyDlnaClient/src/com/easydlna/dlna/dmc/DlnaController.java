package com.easydlna.dlna.dmc;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.AlphaAnimation;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import com.easydlna.dlna.core.CacheManager;
import com.easydlna.dlna.core.ContentDevice;
import com.easydlna.dlna.core.ContentInfoEx;
import com.easydlna.dlna.core.ContentRenderBase;
import com.easydlna.dlna.core.LocalAVRender;
import com.easydlna.dlna.core.LocalImageRender;
import com.easydlna.dlna.core.RemoteContentRender;
import com.easydlna.dlna.core.RenderDevice;
import com.easydlna.dlna.dmc.VerticalSeekbar.OnSeekBarChangeListener;
import com.easydlna.dlna.dmc.R;
import com.easydlna.dlna.service.ContentInfo;
import com.easydlna.dlna.service.RenderStatus;

public class DlnaController extends Activity implements
        ContentRenderBase.IStatusChangedListener, View.OnClickListener {

    public static final String UDN_LOCAL_RENDER = "<LOCAL_RENDER>";

    static class RenderStatusEx extends RenderStatus {
        public String titleString;
        public int progress;
        public int bufferingPercent;
    }

    private static final String TAG = "Dlna:DmcController";

    public static final String EXTRA_CONTENT_DEVICE_UDN = "content_device_udn";
    public static final String EXTRA_CONTENT_INDEX = "content_index";
    public static final String EXTRA_CONTENT_TYPE = "content_type";
    public static final String EXTRA_RENDER_DEVICE_UDN = "render_device_udn";

    private Timer timer = null;
    public static final int MSG_UPDATE_RENDERER_STATUS = 0;
    public static final int MSG_RENDER_CONTENT = 1;

    public static final int MSG_SHOW_PROGRESS_DIALOG = 2;
    public static final int MSG_DISMISS_PROGRESS_DIALOG = 3;

    private boolean mPreparingDataSource = false;
    private ContentDevice contentDevice;

    Handler mMainHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MSG_UPDATE_RENDERER_STATUS:
                updateUiForRenderStatus(mRendererStatus);
                break;
            case MSG_RENDER_CONTENT:
                renderContent(mCurrentIndex);
                break;
            case MSG_SHOW_PROGRESS_DIALOG:
                showProgressDialog();
                break;
            case MSG_DISMISS_PROGRESS_DIALOG:
                mMainHandler.removeMessages(MSG_SHOW_PROGRESS_DIALOG);
                if (mProgressDialog != null) {
                    mProgressDialog.dismiss();
                    mProgressDialog = null;
                }
            default:
                break;
            }
        }
    };

    private void showProgressDialog() {
        if (mProgressDialog != null)
            return;
        mProgressDialog = new ProgressDialog(DlnaController.this);
        mProgressDialog.setMessage(DlnaController.this.getResources()
                .getString(R.string.message_buffering));
        mProgressDialog
                .setOnDismissListener(new DialogInterface.OnDismissListener() {
                    @SuppressWarnings("deprecation")
                    public void onDismiss(DialogInterface dialog) {
                        if (mPreparingDataSource) {
                            if (mPrepareThread != null) {
                                try {
                                    mPrepareThread.stop();
                                } catch (Exception e) {
                                }
                            }
                            mPreparingDataSource = false;
                            finish();
                        }
                        mPrepareThread = null;
                    }
                });
        mProgressDialog.show();
    }

    ArrayList<ContentInfo> mContentList = new ArrayList<ContentInfo>();
    int mCurrentIndex = 0;
    int mMediaType = -1;

    RenderStatusEx mRendererStatus = null;
    String mRenderUdn = "";
    ContentRenderBase mRender = null;

    Thread mPrepareThread = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.dlna_controller);
        initRefs();
        Intent intent = getIntent();

        contentDevice = ContentDevice.sDevices.findDeviceByUdn(intent
                .getStringExtra(EXTRA_CONTENT_DEVICE_UDN));
        RenderDevice renderDevice = RenderDevice.sDevices
                .findDeviceByUdn(intent.getStringExtra(EXTRA_RENDER_DEVICE_UDN));

        if (contentDevice == null || renderDevice == null) {
            Log.e(TAG, "controller start failed! contentDevice="
                    + contentDevice + " renderDevice=" + renderDevice);
            finish();
            return;
        }

        mCurrentIndex = intent.getIntExtra(EXTRA_CONTENT_INDEX, 0);

        mMediaType = intent.getIntExtra(EXTRA_CONTENT_TYPE, -1);
        if (mMediaType == -1) {
            Log.e(TAG, "controller start failed! Must give a media type");
            finish();
            return;
        }

        mContentList.clear();

        synchronized (contentDevice.mUpdateLock) {
            mContentList.addAll(contentDevice.getContents(mMediaType));
        }

        mRenderUdn = renderDevice.udnString;

        mRender = getContentRender(mRenderUdn, mMediaType);
        mRender.setStatusChangedListener(this);
    }

    ProgressDialog mProgressDialog = null;

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mRender != null) {
            mRender.stop();
            mRender.setStatusChangedListener(null);
        }
    }

    private void renderContent(final int contentIndex) {

        if (mContentList.size() == 0 || contentIndex < 0
                || contentIndex > mContentList.size() - 1) {
            finish();
        }

        mRendererStatus = new RenderStatusEx();
        mRendererStatus.duration = 0;
        mRendererStatus.errcode = 0;
        mRendererStatus.urlString = null;
        mRendererStatus.state = RenderStatus.STATE_STOPPED;
        mRendererStatus.progress = 0;
        mRendererStatus.bufferingPercent = 0;
        mRendererStatus.titleString = mContentList.get(contentIndex).title;
        updateUiForRenderStatus(mRendererStatus);
        mPreparingDataSource = true;

        mPrepareThread = new Thread() {
            @Override
            public void run() {
                mRender.stop();
                try {
                    Thread.sleep(300);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                mMainHandler.sendEmptyMessageDelayed(MSG_SHOW_PROGRESS_DIALOG,
                        300);
                mRender.setDataSource(mContentList.get(contentIndex).resourceUrl);
                mRender.play();
                mPreparingDataSource = false;
                mMainHandler.sendEmptyMessage(MSG_DISMISS_PROGRESS_DIALOG);
            }
        };
        mPrepareThread.start();
    }

    private TextView mTitleTextView;
    private TextView start_time;
    private TextView end_time;
    private SeekBar progressBar;
    private VerticalSeekbar soundSeekbar;
    private ImageButton mNextButton;
    private ImageButton mPrevButton;
    private ImageButton mRenderPauseButton;
    private ImageButton mStopButton;
    private RelativeLayout statusLayout;
    private RemoteImageView mSurfaceView;
    private ImageView sound;
    private ImageView sound_open;
    private AudioManager audiomanage;
    private RelativeLayout time_pane;

    private void initRefs() {
        mTitleTextView = (TextView) findViewById(R.id.info_title);
        start_time = (TextView) findViewById(R.id.start_time);
        end_time = (TextView) findViewById(R.id.end_time);
        progressBar = (SeekBar) findViewById(R.id.seek);
        statusLayout = (RelativeLayout) findViewById(R.id.status_layout);
        time_pane = (RelativeLayout) findViewById(R.id.time_pane);
        mNextButton = (ImageButton) findViewById(R.id.play_control_next);
        mNextButton.setOnClickListener(this);

        mPrevButton = (ImageButton) findViewById(R.id.play_control_prev);
        mPrevButton.setOnClickListener(this);

        mRenderPauseButton = (ImageButton) findViewById(R.id.play_control_play_pause);
        mRenderPauseButton.setOnClickListener(this);

        mStopButton = (ImageButton) findViewById(R.id.play_control_stop);
        mStopButton.setOnClickListener(this);

        sound = (ImageView) findViewById(R.id.sound);
        audiomanage = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        soundSeekbar = (VerticalSeekbar) findViewById(R.id.sound_controll);
        soundSeekbar.setMax(audiomanage
                .getStreamMaxVolume(AudioManager.STREAM_MUSIC));
        sound_open = (ImageView) findViewById(R.id.sound_open);
        sound.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                if (!soundSeekbar.isShown()) {
                    soundSeekbar.setVisibility(View.VISIBLE);
                    sound_open.setVisibility(View.VISIBLE);
                } else {
                    soundSeekbar.setVisibility(View.INVISIBLE);
                    sound_open.setVisibility(View.INVISIBLE);
                }
            }
        });

        soundSeekbar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(VerticalSeekbar VerticalSeekBar,
                    int progress, boolean fromUser) {
                // TODO Auto-generated method stub
                audiomanage.setStreamVolume(AudioManager.STREAM_MUSIC,
                        progress, 0);
                int currentVolume = audiomanage
                        .getStreamVolume(AudioManager.STREAM_MUSIC);
                soundSeekbar.setProgress(currentVolume);

            }

            @Override
            public void onStartTrackingTouch(VerticalSeekbar VerticalSeekBar) {
                // TODO Auto-generated method stub

            }

            @Override
            public void onStopTrackingTouch(VerticalSeekbar VerticalSeekBar) {
                // TODO Auto-generated method stub

            }
        });

        mSurfaceView = (RemoteImageView) findViewById(R.id.surface_view);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            public void surfaceDestroyed(SurfaceHolder holder) {
            }

            public void surfaceCreated(SurfaceHolder holder) {
                mMainHandler.sendEmptyMessage(MSG_RENDER_CONTENT);
            }

            public void surfaceChanged(SurfaceHolder holder, int format,
                    int width, int height) {
            }
        });
    }

    @Override
    public void onClick(View v) {

        if (mPreparingDataSource) {
            return;
        }

        int id = v.getId();
        switch (id) {
        case R.id.play_control_next:
            mCurrentIndex = (mCurrentIndex + 1) % mContentList.size();
            mRenderPauseButton.setVisibility(View.VISIBLE);
            mStopButton.setVisibility(View.GONE);
            renderContent(mCurrentIndex);
            break;
        case R.id.play_control_prev:
            mCurrentIndex = (mCurrentIndex - 1 + mContentList.size())
                    % mContentList.size();
            mRenderPauseButton.setVisibility(View.VISIBLE);
            mStopButton.setVisibility(View.GONE);
            renderContent(mCurrentIndex);
            break;
        case R.id.play_control_stop:
            mRenderPauseButton.setVisibility(View.VISIBLE);
            mStopButton.setVisibility(View.GONE);
            mRender.play();
            // mRender.stop();
            break;
        case R.id.play_control_play_pause:
            switch (mRendererStatus.state) {
            case RenderStatus.STATE_STOPPED:

            case RenderStatus.STATE_PAUSE_PLAYBACK:
                mRender.play();
                break;
            case RenderStatus.STATE_PLAYING:
                mRenderPauseButton.setVisibility(View.GONE);
                mStopButton.setVisibility(View.VISIBLE);
                mRender.pause();
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    private void updateUiForRenderStatus(RenderStatusEx renderStatus) {
        if (mMediaType != ContentInfoEx.MEDIA_TYPE_IMAGE) {
            start_time.setText(getTimeString(renderStatus.progress));
            end_time.setText(getTimeString(renderStatus.duration));
            progressBar.setProgress(renderStatus.progress);
            progressBar.setMax((int) renderStatus.duration);
        }
        mTitleTextView.setText(renderStatus.titleString);
    }

    private String getTimeString(long timeInSec) {
        return String.format("%02d:%02d:%02d", timeInSec / 3600,
                (timeInSec % 3600) / 60, timeInSec % 60);
    }

    private Runnable mUpdateProgressRunnable = new Runnable() {
        public void run() {
            mMainHandler.removeCallbacks(this);
            if (mRendererStatus.state == RenderStatus.STATE_PLAYING) {
                mRendererStatus.progress = mRender.getPos();
                mRendererStatus.bufferingPercent = mRender
                        .getBufferingPercents();
                updateUiForRenderStatus(mRendererStatus);
                mMainHandler.postDelayed(this, 1000);
            }
        }
    };

    @Override
    public void onStatusChanged(RenderStatus status) {
        if (mRendererStatus.duration == 0)
            mRendererStatus.duration = status.duration;
        mRendererStatus.state = status.state;
        if (mRendererStatus.urlString == null)
            mRendererStatus.urlString = status.urlString;
        mRendererStatus.errcode = status.errcode;
        mMainHandler.sendEmptyMessage(MSG_UPDATE_RENDERER_STATUS);

        if (mRendererStatus.state == RenderStatus.STATE_PLAYING) {
            mMainHandler.post(mUpdateProgressRunnable);
        } else {
            mMainHandler.removeCallbacks(mUpdateProgressRunnable);
        }
    }

    public ContentRenderBase getContentRender(String udn, int mediaType) {
        if (udn.equals(UDN_LOCAL_RENDER)) {
            if (mediaType == ContentInfoEx.MEDIA_TYPE_IMAGE) {
                statusLayout.setVisibility(View.INVISIBLE);
                LocalImageRender render = new LocalImageRender(mRenderUdn);
                mSurfaceView.setCache(CacheManager.sImage);
                mSurfaceView.setOnClickListener(new OnClickListener() {

                    @Override
                    public void onClick(View arg0) {
                        // TODO Auto-generated method stub
                        statusLayout.setVisibility(View.VISIBLE);
                        progressBar.setVisibility(View.INVISIBLE);
                        sound.setVisibility(View.INVISIBLE);
                        time_pane.setVisibility(View.INVISIBLE);
                        timer = new Timer();
                        timer.schedule(new MyTimeTask(), 5000);
                    }
                });
                render.setRemoteImageView(mSurfaceView);
                return render;
            } else {
                LocalAVRender render = new LocalAVRender(mRenderUdn);
                if (mMediaType == ContentInfoEx.MEDIA_TYPE_AUDIO) {
                    mSurfaceView.setBackgroundDrawable(getResources()
                            .getDrawable(R.drawable.music_image));
                    ViewGroup.LayoutParams lp = mSurfaceView.getLayoutParams();
                    lp.width = getWindowManager().getDefaultDisplay()
                            .getWidth();
                    lp.height = lp.width;
                    mSurfaceView.setLayoutParams(lp);
                } else if (mMediaType == ContentInfoEx.MEDIA_TYPE_VIDEO) {
                    statusLayout.setVisibility(View.INVISIBLE);
                    mSurfaceView.setOnClickListener(new OnClickListener() {

                        @Override
                        public void onClick(View arg0) {
                            // TODO Auto-generated method stub
                            statusLayout.setVisibility(View.VISIBLE);
                            timer = new Timer();
                            timer.schedule(new MyTimeTask(), 5000);
                        }
                    });
                    ViewGroup.LayoutParams lp = mSurfaceView.getLayoutParams();
                    lp.width = getWindowManager().getDefaultDisplay()
                            .getWidth();
                    lp.height = (int) (0.75 * (float) lp.width);
                    mSurfaceView.setLayoutParams(lp);
                }
                mSurfaceView.setPushMode();

                render.setDisplay(mSurfaceView.getHolder());
                return render;
            }
        } else {
            return new RemoteContentRender(mRenderUdn);
        }
    }

    private Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case 1:
                AlphaAnimation alphaAnimation = new AlphaAnimation(1.0f, 0.0f);
                alphaAnimation.setDuration(1000);
                statusLayout.startAnimation(alphaAnimation);
                statusLayout.setVisibility(View.GONE);
                timer.cancel();
                break;
            }
            super.handleMessage(msg);
        }

    };;

    private class MyTimeTask extends TimerTask {
        @Override
        public void run() {
            Message message = new Message();
            message.what = 1;
            handler.sendMessage(message);
        }

    }
}

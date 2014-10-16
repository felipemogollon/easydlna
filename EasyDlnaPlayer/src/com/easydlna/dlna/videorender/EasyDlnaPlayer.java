package com.easydlna.dlna.videorender;

import com.easydlna.dlna.dlnaclient.DMRClient;
import com.easydlna.dlna.dlnaclient.DlnaUtil;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.MediaController;
import android.widget.RelativeLayout;
import android.widget.Toast;

public class EasyDlnaPlayer extends Activity implements SurfaceHolder.Callback {

    static final String TAG = "VideoRender";

    private RenderPlayer renderPlayer = null;
    private SurfaceView videoSurfaceView = null;
    private SurfaceHolder videoSurfceHolder = null;
    private RelativeLayout videoLayout = null;
    private HeadUI titleLayout = null;

    private WindowManager.LayoutParams ttLayoutParams = null;

    private MediaController playController = null;
    private Handler playHandler = null;
    private Runnable showRunnable = null;

    private boolean bNetworkAvailable = false;
    public final static int SHOW_DEFAULT_TIMEOUT = 10000;

    public final static int MSG_SET_TITLE = 0;
    public final static int MSG_SET_PROGRESS = 1;
    public final static int MSG_SET_PLAYINFO = 2;
    public final static int MSG_SET_POSITION = 3;
    public final static int MSG_POP_INFO = 4;
    public final static int MSG_SET_DMRSTATUS = 5;
    public final static int MSG_BACK = 6;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        videoLayout = (RelativeLayout) getLayoutInflater().inflate(
                R.layout.video, null);
        setContentView(videoLayout);

        if (DlnaUtil.isNetAccesible() == false) {
            new AlertDialog.Builder(this)
                    .setMessage("Network is not availbale, Click OK to quit")
                    .setPositiveButton("OK",
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog,
                                        int which) {
                                    EasyDlnaPlayer.this.finish();
                                }
                            }).create().show();
        } else {
            createPlayers();
            bNetworkAvailable = true;
        }
    }

    public int createPlayers() {
        renderPlayer = new RenderPlayer();

        // MediaController is a View, used for playing control.
        playController = new MediaController(this);
        playController.setMediaPlayer(renderPlayer);
        playController.setAnchorView(videoLayout);

        createViews();

        // Start DMR via DMRClient.
        renderPlayer.startRender(this);

        // playHandler call back by renderPlayer.
        playHandler = new Handler() {
            public void handleMessage(Message msg) {
                switch (msg.what) {
                case MSG_SET_TITLE:
                    if (renderPlayer.playingUrl == null) {
                        Toast.makeText(EasyDlnaPlayer.this,
                                getString(R.string.no_file_play),
                                Toast.LENGTH_SHORT).show();
                        return;
                    } else {
                        Toast.makeText(EasyDlnaPlayer.this,
                                renderPlayer.playingUrl, Toast.LENGTH_SHORT)
                                .show();
                    }
                    titleLayout.setTitle(renderPlayer.playingUrl);
                    playController.show(0);
                    break;
                case MSG_SET_PROGRESS:
                    playHandler.removeCallbacks(showRunnable);
                    playHandler.postDelayed(showRunnable, SHOW_DEFAULT_TIMEOUT);
                    break;
                case MSG_SET_PLAYINFO:
                    renderPlayer.setPlayInfo();
                    break;
                case MSG_SET_POSITION:
                    renderPlayer.setProgress();
                    break;
                case MSG_POP_INFO:
                    playController.show(0);
                    break;
                case MSG_SET_DMRSTATUS:
                    int status = msg.arg1;
                    if (status == DMRClient.DMR_SERVICE_UP) {
                        Toast.makeText(EasyDlnaPlayer.this,
                                getString(R.string.dmr_start_success),
                                Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(EasyDlnaPlayer.this,
                                getString(R.string.dmr_service_fail),
                                Toast.LENGTH_SHORT).show();
                    }
                    break;
                case MSG_BACK:
                    onBackPressed();
                default:
                    super.handleMessage(msg);
                }
            }
        };
        renderPlayer.videoMainHandler = playHandler;
        return 0;
    }

    public int createViews() {
        // videoLayout = (RelativeLayout)
        // findViewById(R.id.videoRelativeLayout);
        videoSurfaceView = (SurfaceView) videoLayout
                .findViewById(R.id.videoSurfaceView);
        videoSurfceHolder = videoSurfaceView.getHolder();
        videoSurfceHolder.setFixedSize(120, 120);
        videoSurfceHolder.addCallback(this);
        videoSurfceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        videoLayout.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (titleLayout == null) {
                    return;
                }
                boolean bShwoing = titleLayout.getVisibility() == View.VISIBLE;
                if (bShwoing) {
                    titleLayout.setVisibility(View.INVISIBLE);
                    playController.hide();
                } else {
                    titleLayout.setVisibility(View.VISIBLE);
                    playController.show(0);
                }
            }
        });
        return 0;
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (!bNetworkAvailable) {
            return;
        }
        if (showRunnable == null) {
            showRunnable = new Runnable() {
                public void run() {
                    if (titleLayout == null) {
                        initTitleLayout();
                    }
                    // If playing, hide title and play controller.
                    boolean bShwoing = playController.isShowing();
                    if (bShwoing) {
                        titleLayout.setVisibility(View.INVISIBLE);
                        playController.hide();
                    } else {
                        titleLayout.setVisibility(View.VISIBLE);
                        playController.show(0);
                    }
                }
            };
        }
        playHandler.postDelayed(showRunnable, 500);
    }

    public void initTitleLayout() {
        WindowManager mWm = (WindowManager) getSystemService(WINDOW_SERVICE);
        int[] myLocations = new int[2];
        videoLayout.getLocationOnScreen(myLocations);

        titleLayout = (HeadUI) getLayoutInflater()
                .inflate(R.layout.title, null);

        if (ttLayoutParams == null) {
            ttLayoutParams = new WindowManager.LayoutParams();
        }
        ttLayoutParams.gravity = Gravity.TOP;
        ttLayoutParams.width = videoLayout.getWidth();
        ttLayoutParams.height = LayoutParams.WRAP_CONTENT;
        ttLayoutParams.x = 0;
        ttLayoutParams.y = myLocations[1] / 2;
        ttLayoutParams.format = PixelFormat.TRANSLUCENT;
        ttLayoutParams.type = WindowManager.LayoutParams.TYPE_APPLICATION;
        ttLayoutParams.flags |= WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
        ttLayoutParams.token = null;
        ttLayoutParams.windowAnimations = 0;

        mWm.addView(titleLayout, ttLayoutParams);
    }

    protected void onStop() {
        if (renderPlayer != null) {
            renderPlayer.release();
            renderPlayer = null;
        }
        WindowManager mWm = (WindowManager) getSystemService(WINDOW_SERVICE);
        if (titleLayout != null) {
            mWm.removeView(titleLayout);
            titleLayout = null;
        }
        if (showRunnable != null) {
            playHandler.removeCallbacks(showRunnable);
        }
        Log.e(TAG, "onStop");
        finish();
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        if (renderPlayer != null) {
            renderPlayer.release();
            renderPlayer = null;
        }
        WindowManager mWm = (WindowManager) getSystemService(WINDOW_SERVICE);
        if (titleLayout != null) {
            mWm.removeView(titleLayout);
            titleLayout = null;
        }
        if (showRunnable != null)
            playHandler.removeCallbacks(showRunnable);
        Log.e(TAG, "onDestroy");
        super.onDestroy();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
            int height) {
        Log.i(TAG, "surfaceChanged");
        if (renderPlayer != null)
            renderPlayer.setSurfaceHolder(holder);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "surfaceChanged");
        if (renderPlayer != null)
            renderPlayer.setSurfaceHolder(holder);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "surfaceChanged");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add("Broadcast DMR to Network");
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (renderPlayer != null) {
            renderPlayer.sendBroadcast();
        }
        return super.onOptionsItemSelected(item);
    }
}

package com.easydlna.dlna.videorender;

import com.easydlna.dlna.dlnaclient.*;
import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnSeekCompleteListener;
import android.media.MediaPlayer.OnVideoSizeChangedListener;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.SeekBar;
import android.widget.MediaController.MediaPlayerControl;
import android.widget.SeekBar.OnSeekBarChangeListener;

/**
 * RenderPlayer
 * <p>
 * i do not know what to say...
 */
class RenderPlayer implements Runnable, OnBufferingUpdateListener,
        OnErrorListener, OnInfoListener, OnPreparedListener,
        OnSeekCompleteListener, OnCompletionListener,
        OnVideoSizeChangedListener, OnSeekBarChangeListener,
        MediaPlayerControl, DMRClient.DMRCallback {

    private SurfaceHolder videoSurfceHolder = null;
    public Handler videoMainHandler = null;
    private volatile MediaPlayer mediaPlayer = null;

    private int videoHeight = 0, videoWidth = 0;

    private PlayingState playingState = null;
    public String playingUrl = null;
    private int DEFAULT_VIDEO_SIZE = 120;
    volatile private int DMR_STATUS = DMRClient.DMR_SERVICE_DOWN;

    private static int ONE_SECOND = 1000;
    private static int ONE_HOUR = 3600;
    private static int ONE_MINUTE = 60;

    private static int PREPARED = 0;
    private static int STARTED = 1;
    private static int PAUSED = 2;
    private static int STOP = 3;
    private int renderstatus = STOP;

    private DMRClient dmrClient = null;

    private int bufferPercent = 0;
    private final static String TAG = EasyDlnaPlayer.TAG;

    private PowerManager.WakeLock wakelock = null;

    public void startRender(Context context) {
        dmrClient = new DMRClient(context);
        dmrClient.startDMR(this);
        PowerManager pm = (PowerManager) context
                .getSystemService(Context.POWER_SERVICE);
        wakelock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK,
                "My Tag");
    }

    public void stopRender() {
        if (dmrClient != null) {
            dmrClient.stopDMR();
        }
        dmrClient = null;
        wakelock.release();
    }

    public void sendBroadcast() {
        if (dmrClient != null) {
            dmrClient.sendDMRBroadcast();
        }
    }

    private Handler notifyHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (msg.what == 0 && dmrClient != null)
                dmrClient.setStatus(playingState);
        }
    };

    private int notifystateChange() {
        if (dmrClient == null)
            return -1;
        notifyHandler.sendEmptyMessage(0);
        return 0;
    }

    private int getpos() {
        if (mediaPlayer == null) {
            Log.d(TAG, "zdw going to return 0");
            return 0;
        }
        return mediaPlayer.getCurrentPosition();
    }

    public synchronized int Stop() {
        try {
            renderstatus = STOP;
            if (mediaPlayer == null) {
                return -1;
            }
            if (mediaPlayer.isPlaying()) {
                mediaPlayer.stop();
                videoHeight = 0;
                videoWidth = 0;
            }

            mediaPlayer.stop();

            // playingState.reset();
            playingState.state = "STOPPED";
            notifystateChange();

            videoMainHandler.removeCallbacks(this);
            videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_POP_INFO);
            Reset();
            return 0;
        } catch (Exception e) {
            if (e instanceof IllegalStateException) {
                Reset();
            }
            videoMainHandler.removeCallbacks(this);
            return -1;
        }
    }

    public void setPlayInfo() {
        // int totallen = mediaPlayer.getDuration();
    }

    private void createMediaPlayer() {
        if (mediaPlayer == null) {
            mediaPlayer = new MediaPlayer();
            mediaPlayer.setDisplay(videoSurfceHolder);
            mediaPlayer.setOnBufferingUpdateListener(this);
            mediaPlayer.setOnCompletionListener(this);
            mediaPlayer.setOnErrorListener(this);
            mediaPlayer.setOnInfoListener(this);
            mediaPlayer.setOnPreparedListener(this);
            mediaPlayer.setOnSeekCompleteListener(this);
            mediaPlayer.setOnVideoSizeChangedListener(this);
            mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
        }
    }

    public RenderPlayer() {
        mediaPlayer = new MediaPlayer();
        playingState = new PlayingState();
        Log.e(EasyDlnaPlayer.TAG, "RenderPlayer constructed");
    }

    public void setSurfaceHolder(SurfaceHolder screenHolder) {
        mediaPlayer.setDisplay(screenHolder);
        videoSurfceHolder = screenHolder;
    }

    public synchronized void release() {
        Log.i(EasyDlnaPlayer.TAG, "RenderPlayer release\n");
        try {
            if (mediaPlayer.isPlaying()) {
                mediaPlayer.stop();
            }
            mediaPlayer.reset();
            mediaPlayer.release();
            if (dmrClient != null)
                dmrClient.stopDMR();
        } catch (Exception e) {
            Log.e(EasyDlnaPlayer.TAG, "RenderPlayer release error\n");
        } finally {
            playingState.reset();
            playingState.state = "STOPPED";
            notifystateChange();
            videoMainHandler.removeCallbacks(this);
        }
    }

    public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
        if (width == 0 || height == 0) {
            return;
        }
        videoHeight = height;
        videoWidth = width;
        if (videoSurfceHolder != null)
            videoSurfceHolder.setFixedSize(videoWidth, videoHeight);

    }

    public static String formatTime(int time) {
        time = time / ONE_SECOND;
        int hour = time / ONE_HOUR;
        int minute = (time - hour * ONE_HOUR) / ONE_MINUTE;
        if (minute < 0) {
            minute = 0;
        }
        int seconds = time - minute * ONE_MINUTE - hour * ONE_HOUR;
        if (seconds < 0) {
            seconds = 0;
        }
        return String.format("%2d:%2d:%2d", hour, minute, seconds);
    }

    public void run() {
        /*
         * try { if (mediaPlayer.isPlaying()) { Log.e(TAG,"getPosition=" +
         * mediaPlayer.getCurrentPosition()); videoMainHandler.postDelayed(this,
         * 1000); } else { videoMainHandler.removeCallbacks(this); } } catch
         * (Exception e) { }
         */
    }

    public void onBufferingUpdate(MediaPlayer mp, int percent) {
        bufferPercent = percent;
    }

    public void onCompletion(MediaPlayer mp) {
        Log.i(EasyDlnaPlayer.TAG, "onCompletion");
        Stop();

    }

    public boolean onError(MediaPlayer arg0, int arg1, int arg2) {
        String bug = "onError ( " + arg1 + " " + arg2 + ")";
        Log.i(EasyDlnaPlayer.TAG, bug);

        playingState.reset();
        playingState.status = "ERROR_OCCURRED";
        notifystateChange();
        // Reset();

        return true;
    }

    public boolean onInfo(MediaPlayer mp, int what, int extra) {
        String bug = "onInfo ( " + what + " " + extra + ")";
        Log.i(EasyDlnaPlayer.TAG, bug);
        return true;
    }

    public void onPrepared(MediaPlayer mp) {
        String bug = "onPrepared";
        Log.i(EasyDlnaPlayer.TAG, bug);

        Play();

    }

    public void setProgress() {
    }

    public void onSeekComplete(MediaPlayer mp) {
        String bug = "onSeekComplete";
        Log.i(EasyDlnaPlayer.TAG, bug);
        videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_SET_POSITION);

    }

    public void onProgressChanged(SeekBar seekBar, int progress,
            boolean fromUser) {
        if (!fromUser) {
            return;
        }

        if (mediaPlayer.isPlaying()) {
            mediaPlayer.seekTo(progress);
        }
        String bug = "onProgressChanged : " + progress;
        Log.i(EasyDlnaPlayer.TAG, bug);
        videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_SET_PROGRESS);
    }

    public void onStartTrackingTouch(SeekBar seekBar) {
        return;
    }

    public void onStopTrackingTouch(SeekBar seekBar) {
        return;
    }

    @Override
    public void DMRServiceStatusNotify(int arg0) {
        DMR_STATUS = arg0;
        if (arg0 == DMRClient.DMR_SERVICE_ERROR) {
            Log.e(EasyDlnaPlayer.TAG, "DMR service error");
        } else if (arg0 == DMRClient.DMR_SERVICE_DOWN) {
            Log.e(EasyDlnaPlayer.TAG, "DMR service down");
        } else if (arg0 == DMRClient.DMR_SERVICE_UP) {
            Log.e(EasyDlnaPlayer.TAG, "DMR service up");
        }
        if (videoMainHandler != null) {
            Message dmrstatusMsg = videoMainHandler.obtainMessage(
                    EasyDlnaPlayer.MSG_SET_DMRSTATUS, DMR_STATUS);
            videoMainHandler.sendMessage(dmrstatusMsg);
        }

    }

    private boolean checkDMRStatus() {
        return DMR_STATUS == DMRClient.DMR_SERVICE_UP ? true : false;
    }

    public int GetPosition() {
        if (renderstatus == STARTED || renderstatus == PAUSED) {
            return mediaPlayer.getCurrentPosition();
        }
        return 0;
    }

    public synchronized int Seek(int time) {
        try {
            if (mediaPlayer == null) {
                return -1;
            }
            mediaPlayer.seekTo(time);
            return 0;
        } catch (Exception e) {
            if (e instanceof IllegalStateException) {
                Reset();
            }
            return -1;
        }
    }

    public synchronized int SetDataSource(String uri) {
        int nret = 0;
        boolean bsetOk = false;
        Log.e(TAG, "setDataSource = " + uri);
        try {
            if (renderstatus != STOP) {
                nret = Stop();
            }
            createMediaPlayer();
            if (nret == 0) {

                mediaPlayer.setDataSource(uri);
                bsetOk = true;
                mediaPlayer.prepare();
                playingState.reset();
                playingState.avtUri = uri;
                playingUrl = uri;
                notifystateChange();
                videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_SET_TITLE);
                videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_POP_INFO);
                renderstatus = PREPARED;

            }
            return 0;

        } catch (IllegalStateException e) {
            Reset();
            return -1;
        } catch (Exception e) {
            if (bsetOk) {
                Reset();
            }
            return -1;
        }
    }

    public synchronized int Play() {
        try {
            if (mediaPlayer == null) {
                createMediaPlayer();
            }
            if (renderstatus == STOP && playingUrl != null) {
                SetDataSource(playingUrl);
            }
            if (renderstatus == PREPARED || renderstatus == PAUSED) {
                if (videoHeight == 0) {
                    videoHeight = DEFAULT_VIDEO_SIZE;
                }
                if (videoWidth == 0) {
                    videoWidth = DEFAULT_VIDEO_SIZE;
                }

                videoSurfceHolder.setFixedSize(videoWidth, videoHeight);
                mediaPlayer.start();
                renderstatus = STARTED;
                videoMainHandler
                        .sendEmptyMessage(EasyDlnaPlayer.MSG_SET_PLAYINFO);

                videoMainHandler.postDelayed(this, ONE_SECOND);

                int totallen = mediaPlayer.getDuration();
                Log.e(TAG, "getDuration = " + totallen);
                // playingState.reset();
                playingState.state = "PLAYING";
                playingState.duration = formatTime(totallen);
                notifystateChange();
                videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_POP_INFO);

            }
            return 0;
        } catch (Exception e) {
            if (e instanceof IllegalStateException) {
                Reset();
            }
            return -1;
        }
    }

    public synchronized int Pause() {
        try {
            if (mediaPlayer == null) {
                return -1;
            }
            if (mediaPlayer.isPlaying()) {
                mediaPlayer.pause();
            }
            renderstatus = PAUSED;
            // playingState.reset();
            playingState.state = "PAUSED_PLAYBACK";
            notifystateChange();
            videoMainHandler.removeCallbacks(this);
            videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_POP_INFO);
            Log.e(TAG, "pause");
            return 0;
        } catch (Exception e) {
            return -1;
        }
    }

    private void Reset() {
        if (mediaPlayer != null) {
            mediaPlayer.reset();
            mediaPlayer.release();
            mediaPlayer = null;
        }
        playingState.reset();
        videoMainHandler.sendEmptyMessage(EasyDlnaPlayer.MSG_POP_INFO);
        playingState.state = "NO_MEDIA_PRESENT";
        // playingUrl = "NO MEDIA PRESENT";
        renderstatus = STOP;
        notifystateChange();
        Log.e(TAG, "reset");
    }

    @Override
    public boolean canPause() {
        return true;
    }

    @Override
    public boolean canSeekBackward() {
        return false;
    }

    @Override
    public boolean canSeekForward() {
        return false;
    }

    @Override
    public int getBufferPercentage() {
        return bufferPercent;
    }

    @Override
    public int getCurrentPosition() {
        try {
            if (playingUrl.equals("NO MEDIA PRESENT")
                    || !mediaPlayer.isPlaying())
                return 0;
            return mediaPlayer.getCurrentPosition();
        } catch (Exception e) {
        }
        return 0;
    }

    @Override
    public int getDuration() {
        try {
            if (playingUrl.equals("NO MEDIA PRESENT")
                    || !mediaPlayer.isPlaying())
                return 0;
            return mediaPlayer.getDuration();
        } catch (Exception e) {
        }
        return 0;
    }

    @Override
    public synchronized boolean isPlaying() {
        try {
            return mediaPlayer.isPlaying();
        } catch (Exception e) {
        }
        return false;
    }

    @Override
    public void seekTo(int pos) {
        Seek(pos);
    }

    @Override
    public void start() {
        Play();
    }

    @Override
    public void pause() {
        Pause();
    }
}

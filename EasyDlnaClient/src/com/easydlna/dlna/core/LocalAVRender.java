package com.easydlna.dlna.core;

import java.io.IOException;

import com.easydlna.dlna.service.RenderStatus;

import android.media.MediaPlayer;
import android.view.SurfaceHolder;

public class LocalAVRender extends ContentRenderBase implements
        MediaPlayer.OnBufferingUpdateListener, MediaPlayer.OnErrorListener,
        MediaPlayer.OnCompletionListener, MediaPlayer.OnPreparedListener,
        MediaPlayer.OnSeekCompleteListener,
        MediaPlayer.OnVideoSizeChangedListener {
    MediaPlayer mMediaPlayer = null;
    SurfaceHolder mHolder = null;

    public LocalAVRender(String renderUdn) {
        super(renderUdn);
    }

    public void setDisplay(SurfaceHolder holder) {
        mHolder = holder;
    }

    @Override
    public void play() {
        if (mStatus.urlString == null) {
            return;
        }
        if (mStatus.state == RenderStatus.STATE_STOPPED && mMediaPlayer == null) {
            try {
                setDataSource(mStatus.urlString);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        mMediaPlayer.start();
        mStatus.state = RenderStatus.STATE_PLAYING;
        notifyStatusChanged();
    }

    @Override
    public void stop() {
        if (mMediaPlayer != null) {
            mMediaPlayer.stop();
            mMediaPlayer.reset();
            mMediaPlayer.release();
            mMediaPlayer = null;
            mStatus.state = RenderStatus.STATE_STOPPED;
            notifyStatusChanged();
        }
    }

    @Override
    public void pause() {
        if (mMediaPlayer != null) {
            mMediaPlayer.pause();
            mStatus.state = RenderStatus.STATE_PAUSE_PLAYBACK;
            notifyStatusChanged();
        }
    }

    @Override
    public int getPos() {
        if (mMediaPlayer != null) {
            return mMediaPlayer.getCurrentPosition() / 1000;
        } else {
            return 0;
        }
    }

    @Override
    public void seek(int posInMSec) {
        if (mMediaPlayer != null)
            mMediaPlayer.seekTo(posInMSec);
    }

    @Override
    public void setDataSource(String url) {

        stop();

        mMediaPlayer = new MediaPlayer();
        mMediaPlayer.setOnBufferingUpdateListener(this);
        mMediaPlayer.setOnCompletionListener(this);
        mMediaPlayer.setOnErrorListener(this);
        mMediaPlayer.setOnPreparedListener(this);
        mMediaPlayer.setOnSeekCompleteListener(this);
        mMediaPlayer.setOnVideoSizeChangedListener(this);
        try {
            mMediaPlayer.setDisplay(mHolder);
            mMediaPlayer.setDataSource(url);
            mMediaPlayer.prepare();
            mStatus.duration = mMediaPlayer.getDuration() / 1000;
            mStatus.errcode = 0;
            mStatus.urlString = url;
            mBufferingPercents = 0;
        } catch (IllegalArgumentException e) {
            mStatus.errcode = 1;
            e.printStackTrace();
        } catch (IllegalStateException e) {
            mStatus.errcode = 1;
            e.printStackTrace();
        } catch (IOException e) {
            mStatus.errcode = 1;
            e.printStackTrace();
        }
        notifyStatusChanged();
    }

    @Override
    public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
        // mHolder.setFixedSize(width, height);
    }

    @Override
    public void onSeekComplete(MediaPlayer mp) {
    }

    @Override
    public void onPrepared(MediaPlayer mp) {
    }

    @Override
    public void onCompletion(MediaPlayer mp) {
        mStatus.state = RenderStatus.STATE_STOPPED;
        notifyStatusChanged();
    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        mStatus.errcode = 1;
        notifyStatusChanged();
        return true;
    }

    @Override
    public void onBufferingUpdate(MediaPlayer mp, int percent) {
        if (percent >= 0)
            mBufferingPercents = percent;
    }
}

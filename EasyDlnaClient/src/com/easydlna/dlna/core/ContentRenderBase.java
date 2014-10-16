package com.easydlna.dlna.core;

import com.easydlna.dlna.service.RenderStatus;

public abstract class ContentRenderBase {

    public interface IStatusChangedListener {
        public void onStatusChanged(RenderStatus status);
    }

    protected IStatusChangedListener mListener = null;

    public void setStatusChangedListener(IStatusChangedListener listener) {
        mListener = listener;
    }

    protected void notifyStatusChanged() {
        if (mListener != null) {
            mListener.onStatusChanged(mStatus);
        }
    }

    protected RenderStatus mStatus = new RenderStatus();
    protected String mRenderUdn = null;

    public ContentRenderBase(String renderUdn) {
        mRenderUdn = renderUdn;
    }

    protected int mBufferingPercents = 0;

    public int getBufferingPercents() {
        return mBufferingPercents;
    }

    public abstract void play();

    public abstract void stop();

    public abstract void pause();

    public abstract int getPos();

    public abstract void seek(int pos);

    public abstract void setDataSource(String url);
}

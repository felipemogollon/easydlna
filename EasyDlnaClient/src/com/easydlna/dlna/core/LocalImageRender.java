package com.easydlna.dlna.core;

import com.easydlna.dlna.dmc.RemoteImageView;

public class LocalImageRender extends ContentRenderBase {

    public LocalImageRender(String renderUdn) {
        super(renderUdn);
    }

    public void play() {
    }

    public void stop() {
    }

    @Override
    public void pause() {
    }

    @Override
    public int getPos() {
        return 0;
    }

    @Override
    public void seek(int pos) {

    }

    RemoteImageView mImageView = null;

    public void setRemoteImageView(RemoteImageView view) {
        mImageView = view;
    }

    @Override
    public void setDataSource(String url) {
        if (mImageView != null) {
            mImageView.setImageUrl(url);
        }
    }
}

package com.easydlna.dlna.core;

import com.easydlna.dlna.dlnaclient.DMCClient;
import com.easydlna.dlna.service.RenderStatus;

public class RemoteContentRender extends ContentRenderBase implements
        DmcClientWraper.IDmcControllerClient {

    public RemoteContentRender(String udn) {
        super(udn);
        DmcClientWraper.setControllerClient(this);
    }

    @Override
    public void play() {
        DmcClientWraper.sClient.playControl(mRenderUdn, DMCClient.AVT_PLAY,
                null);
    }

    @Override
    public void stop() {
        DmcClientWraper.sClient.playControl(mRenderUdn, DMCClient.AVT_STOP,
                null);
    }

    @Override
    public void pause() {
        DmcClientWraper.sClient.playControl(mRenderUdn, DMCClient.AVT_PAUSE,
                null);
    }

    @Override
    public int getPos() {
        return DmcClientWraper.sClient.playControl(mRenderUdn,
                DMCClient.AVT_GETPOS, null);
    }

    @Override
    public void seek(int pos) {
        DmcClientWraper.sClient.playControl(mRenderUdn, DMCClient.AVT_SEEK, ""
                + pos);
    }

    @Override
    public void setDataSource(String url) {
        DmcClientWraper.sClient.playControl(mRenderUdn,
                DMCClient.AVT_SETDATASOURCE, url);
    }

    @Override
    public void DlnaPlayStatusNotify(String udn, RenderStatus status) {
        if (udn.equals(mRenderUdn)) {
            mStatus = status;
            notifyStatusChanged();
        }
    }

}

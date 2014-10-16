package com.easydlna.dlna.core;

import com.easydlna.dlna.service.ContentInfo;

public class ContentInfoEx extends ContentInfo {

    public static final int MEDIA_TYPE_VIDEO = 0;
    public static final int MEDIA_TYPE_AUDIO = 1;
    public static final int MEDIA_TYPE_IMAGE = 2;

    int mMediaType = -1;

    public int getMediaType() {
        return mMediaType;
    }

    public ContentInfoEx(ContentInfo base, int mediaType) {
        id = base.id;
        parentId = base.parentId;
        childCount = base.childCount;
        title = base.title;
        protocalInfo = base.protocalInfo;
        resourceUrl = base.resourceUrl;
        thumbnailUrl = base.thumbnailUrl;
        mediaClasString = base.mediaClasString;
        size = base.size;
        mMediaType = mediaType;
    }
}

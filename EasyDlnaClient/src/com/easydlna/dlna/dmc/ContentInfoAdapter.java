package com.easydlna.dlna.dmc;

import java.util.List;

import com.easydlna.dlna.core.CacheManager;
import com.easydlna.dlna.core.ContentInfoEx;
import com.easydlna.dlna.dmc.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class ContentInfoAdapter extends ArrayAdapter<ContentInfoEx> {

    private Bitmap mDefaultTumbnail = null;

    LayoutInflater mInflater = null;

    public ContentInfoAdapter(Context context, List<ContentInfoEx> array,
            Bitmap defaultThumb) {
        super(context, 0, array);
        mDefaultTumbnail = defaultThumb;
        mInflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    public Bitmap getDefaultTumbnail() {
        return mDefaultTumbnail;
    }

    public void setDefaultThumbnail(Bitmap defaultThumb) {
        mDefaultTumbnail = defaultThumb;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = mInflater.inflate(R.layout.media_list_item, null);
        }
        ContentInfoEx infoEx = getItem(position);
        ((TextView) convertView.findViewById(R.id.plain_media_item_title))
                .setText(infoEx.title);
        RemoteThumbnailView remoteImageView = (RemoteThumbnailView) convertView
                .findViewById(R.id.plain_media_item_tumbnail);
        remoteImageView.setDefaultBitmap(mDefaultTumbnail);

        String thumbUri = null;
        switch (infoEx.getMediaType()) {
        case ContentInfoEx.MEDIA_TYPE_AUDIO:
            remoteImageView.setCache(null);
            thumbUri = null;
            break;
        case ContentInfoEx.MEDIA_TYPE_IMAGE:
            remoteImageView.setCache(CacheManager.sImageThumbnail);
            thumbUri = infoEx.thumbnailUrl;
            if (thumbUri == null || thumbUri.equals("")) {
                thumbUri = infoEx.resourceUrl;
            }
            break;
        case ContentInfoEx.MEDIA_TYPE_VIDEO:
            remoteImageView.setCache(CacheManager.sVideoThumbnail);
            thumbUri = infoEx.thumbnailUrl;
            if (thumbUri != null && thumbUri.equals("")) {
                thumbUri = null;
            }
        default:
            break;
        }
        remoteImageView.setImageUrl(thumbUri);
        return convertView;
    }

}

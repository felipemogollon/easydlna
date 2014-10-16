package com.easydlna.dlna.dmc;

import com.easydlna.dlna.core.DownloadManager;
import com.easydlna.dlna.core.CacheManager.WindowedObjectCache;
import com.easydlna.dlna.core.DownloadManager.DownloadTask;
import com.easydlna.dlna.core.DownloadManager.ImageDownloadTask;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.ImageView;

public class RemoteThumbnailView extends ImageView implements
        DownloadManager.DownloadStatusListener {

    public WindowedObjectCache<String, Bitmap> mCache = null;

    public String mUrl = null;

    public ImageDownloadTask mDownloadTask = null;

    public Bitmap mBitmap = null;

    private Bitmap mDefaultBitmap = null;

    public void setDefaultBitmap(Bitmap defaultBitmap) {
        mDefaultBitmap = defaultBitmap;
    }

    public int mDownloadPercent = -1;

    public Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            setImageBitmap(mBitmap);
        }
    };

    public RemoteThumbnailView(Context context) {
        super(context);
    }

    public RemoteThumbnailView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public RemoteThumbnailView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    public void setCache(WindowedObjectCache<String, Bitmap> cache) {
        mCache = cache;
    }

    public void setImageUrl(String url) {
        if ((mUrl != null && mUrl.equals(url)))
            return;

        Bitmap oldOne = mBitmap;

        if (mDownloadTask != null) {
            mDownloadTask.cancelDownload();
        }

        mUrl = url;

        if (mUrl != null) {
            mBitmap = mCache.get(mUrl);
            if (mBitmap == null) {
                mDownloadPercent = 0;
                Log.d("zdw", "submiting task");
                mDownloadTask = new ImageDownloadTask(mUrl);
                mDownloadTask.setDownloadStatusListener(this);
                DownloadManager.submitTask(mDownloadTask);
                mBitmap = mDefaultBitmap;
            }
        } else {
            mBitmap = mDefaultBitmap;
        }
        if (mBitmap != oldOne)
            updateView();
    }

    @Override
    public void onDownloadFinished(DownloadTask task, boolean success) {
        mDownloadTask = null;
        Bitmap image = ((ImageDownloadTask) task).getDownloadedBitmap();
        if (image != null) {
            mDownloadPercent = 100;
            mBitmap = createThumb(image);
            mCache.put(mUrl, mBitmap);
            image = null;
        } else { // download failed, use default Bitmap
            mDownloadPercent = 100;
            mBitmap = mDefaultBitmap;
            mCache.put(mUrl, mBitmap);
        }

        updateView();
    }

    @Override
    public void onProcessingDownload(DownloadTask task, int bytesDownloaded,
            int totalSize) {
        mDownloadPercent = bytesDownloaded * 100 / totalSize;
        updateView();
    }

    private void updateView() {
        mHandler.sendEmptyMessage(0);
    }

    private Bitmap createThumb(Bitmap source) {
        float imageHWRatio = source.getHeight() / (float) source.getWidth();
        float canvasHWRatio = this.getHeight() / (float) this.getWidth();

        int w, h;
        if (imageHWRatio > canvasHWRatio) {
            h = this.getHeight();
            w = (int) (h / imageHWRatio);
        } else {
            w = this.getWidth();
            h = (int) (w * imageHWRatio);
        }

        int width = source.getWidth();
        int height = source.getHeight();
        Matrix matrix = new Matrix();
        float scaleWidht = ((float) w / width);
        float scaleHeight = ((float) h / height);
        matrix.postScale(scaleWidht, scaleHeight);
        Bitmap thumb = Bitmap.createBitmap(source, 0, 0, width, height, matrix,
                true);
        return thumb;
    }
}

package com.easydlna.dlna.dmc;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.easydlna.dlna.core.DownloadManager;
import com.easydlna.dlna.core.CacheManager.WindowedObjectCache;
import com.easydlna.dlna.core.DownloadManager.DownloadStatusListener;
import com.easydlna.dlna.core.DownloadManager.DownloadTask;
import com.easydlna.dlna.core.DownloadManager.ImageDownloadTask;

public class RemoteImageView extends SurfaceView implements
        DownloadStatusListener, SurfaceHolder.Callback {

    public static final String TAG = "Dlna:RemoteImageView";
    public boolean mSurfaceValid = false;

    public WindowedObjectCache<String, Bitmap> mCache = null;

    public String mUrl = null;

    public ImageDownloadTask mDownloadTask = null;

    public Bitmap mBitmap = null;

    private Bitmap mDefaultBitmap = null;

    public void setDefaultBitmap(Bitmap defaultBitmap) {
        mDefaultBitmap = defaultBitmap;
    }

    public String mMessage = null;

    public int mDownloadPercent = -1;
    public boolean mIsInPushMode = false;

    public void setPushMode() {
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mIsInPushMode = true;
    }

    public RemoteImageView(Context context) {
        super(context);
        getHolder().addCallback(this);
    }

    public RemoteImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
        getHolder().addCallback(this);
    }

    public RemoteImageView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        getHolder().addCallback(this);
    }

    public void setCache(WindowedObjectCache<String, Bitmap> cache) {
        mCache = cache;
    }

    public void setImageUrl(String url) {
        Log.d(TAG, "setting url");
        if (mUrl == url || url == null || (mUrl != null && mUrl.equals(url)))
            return;

        if (mDownloadTask != null) {
            mDownloadTask.cancelDownload();
        }

        mUrl = url;

        if (mUrl != null) {
            mBitmap = mCache.get(mUrl);
            if (mBitmap == null) {
                mMessage = "waiting ...";
                mDownloadPercent = 0;
                mDownloadTask = new ImageDownloadTask(mUrl);
                mDownloadTask.setDownloadStatusListener(this);
                DownloadManager.submitTask(mDownloadTask);
            }
        } else {
            mBitmap = mDefaultBitmap;
        }

        updateView();
    }

    @Override
    public void onDownloadFinished(DownloadTask task, boolean success) {
        Bitmap image = ((ImageDownloadTask) task).getDownloadedBitmap();
        mDownloadTask = null;
        if (image != null) {
            mCache.put(mUrl, image);
            mMessage = "finished.";
            mDownloadPercent = 100;
            mBitmap = image;
        } else {
            mBitmap = null;
            mMessage = "download failed!";
            mDownloadPercent = 0;
        }
        updateView();
    }

    @Override
    public void onProcessingDownload(DownloadTask task, int bytesDownloaded,
            int totalSize) {
        mDownloadPercent = (bytesDownloaded * 100 / totalSize);
        mMessage = "" + mDownloadPercent + "%";
        updateView();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
            int height) {
        mSurfaceValid = true;
        Log.d(TAG, "surfaceChagned: true");
        updateView();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mSurfaceValid = true;
        updateView();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mSurfaceValid = false;
    }

    private void updateView() {
        if (!mSurfaceValid || mIsInPushMode)
            return;
        Canvas canvas = getHolder().lockCanvas();
        canvas.drawRGB(0x00, 0x00, 0x00);
        Paint paint = new Paint();
        if (mBitmap != null) {

            float imageHWRatio = mBitmap.getHeight()
                    / (float) mBitmap.getWidth();
            float canvasHWRatio = this.getHeight() / (float) this.getWidth();

            RectF destRectF = new RectF();
            if (imageHWRatio > canvasHWRatio) {
                destRectF.top = 0;
                destRectF.bottom = this.getHeight();
                float width = (destRectF.bottom - destRectF.top) / imageHWRatio;
                destRectF.left = (this.getWidth() - width) / 2;
                destRectF.right = destRectF.left + width;
            } else {
                destRectF.left = 0;
                destRectF.right = this.getWidth();
                float height = (destRectF.right - destRectF.left)
                        * imageHWRatio;
                destRectF.top = (this.getHeight() - height) / 2;
                destRectF.bottom = destRectF.top + height;
            }

            canvas.drawBitmap(mBitmap, new Rect(0, 0, mBitmap.getWidth(),
                    mBitmap.getHeight()), destRectF, paint);

        } else if (mMessage != null) {

            float fontWidth = paint.measureText(mMessage);
            float fontHeight = paint.getTextSize();
            paint.setColor(Color.WHITE);
            canvas.drawText(mMessage, (canvas.getWidth() - fontWidth) / 2,
                    (canvas.getHeight() - fontHeight) / 2, paint);
            paint.setColor(Color.DKGRAY);
            float barLeft = canvas.getWidth() * 0.2f;
            float barWidth = canvas.getWidth() - barLeft * 2;
            float barDownloadedWidth = barWidth * mDownloadPercent / 100f;
            float barTop = (canvas.getHeight() - fontHeight) / 2 + fontHeight;
            float barHeight = fontHeight / 2;
            canvas.drawRect(new RectF(barLeft, barTop, barLeft + barWidth,
                    barTop + barHeight), paint);
            paint.setColor(Color.WHITE);
            canvas.drawRect(new RectF(barLeft, barTop, barLeft
                    + barDownloadedWidth, barTop + barHeight), paint);
        }
        getHolder().unlockCanvasAndPost(canvas);
    }
}

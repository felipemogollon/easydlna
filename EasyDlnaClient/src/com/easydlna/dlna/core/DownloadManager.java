package com.easydlna.dlna.core;

import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

public class DownloadManager {

    public static final String TAG = "Dlna:DmcApp.DownloadManager";

    private static ExecutorService mThreadPool = new ThreadPoolExecutor(5, 10,
            60, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>());

    public static void submitTask(DownloadTask task) {
        mThreadPool.submit(task);
    }

    public static void shutDown() {
        mThreadPool.shutdown(); // Disable new tasks from being submitted
        try {
            if (!mThreadPool.awaitTermination(60, TimeUnit.SECONDS)) {
                mThreadPool.shutdownNow(); // Cancel currently executing tasks
                if (!mThreadPool.awaitTermination(60, TimeUnit.SECONDS))
                    System.err.println("Pool did not terminate");
            }
        } catch (InterruptedException ie) {
            mThreadPool.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }

    public interface DownloadStatusListener {
        void onDownloadFinished(DownloadTask task, boolean success);

        void onProcessingDownload(DownloadTask task, int bytesDownloaded,
                int totalSize);
    }

    public static abstract class DownloadTask implements Runnable {

        private static final int DOWNLOAD_BUFFER_SIZE = 10 * 1024;

        private String mDownloadUrl;

        public String getDownloadUrl() {
            return mDownloadUrl;
        }

        private int mBytesDownloaded;

        public int getBytesDownloaded() {
            return mBytesDownloaded;
        }

        private int mTotalSize;

        public int getTotalSize() {
            return mTotalSize;
        }

        private boolean mExitPending = false;

        public void cancelDownload() {
            mExitPending = true;
        }

        private DownloadStatusListener mListener = null;

        public void setDownloadStatusListener(DownloadStatusListener listener) {
            mListener = listener;
        }

        public DownloadTask(String url) {
            mDownloadUrl = url;
        }

        protected abstract boolean onDownloadStarting(int totalSize,
                InputStream inStream);

        protected abstract boolean onDownloadFinished(boolean success);

        protected abstract void onProcessingDownload(byte[] buffer,
                int bytesRead, int bytesDownloaded);

        public void run() {
            if (mExitPending) {
                Log.d(TAG, "exit for mExitPending at the beginning");
                return;
            }

            try {
                URL url = new URL(mDownloadUrl);
                Log.d(TAG, "opening connection!");
                URLConnection conn = url.openConnection();
                Log.d(TAG, "getting content length!");
                mTotalSize = conn.getContentLength();

                byte[] buffer = new byte[DOWNLOAD_BUFFER_SIZE];
                int bytesRead = 0;
                InputStream inStream = conn.getInputStream();

                Log.d(TAG, "before starting");
                if (onDownloadStarting(mTotalSize, inStream)) {
                    inStream.close();
                    Log.d(TAG, "notify completed");
                    mListener.onDownloadFinished(this, true);
                    return;
                }
                try {
                    while (!mExitPending
                            && (bytesRead = inStream.read(buffer, 0,
                                    DOWNLOAD_BUFFER_SIZE)) > 0) {
                        mBytesDownloaded += bytesRead;
                        Log.d(TAG, "process downloading");
                        onProcessingDownload(buffer, bytesRead,
                                mBytesDownloaded);
                        if (mListener != null) {
                            mListener.onProcessingDownload(this,
                                    mBytesDownloaded, mTotalSize);
                        }
                    }
                } catch (Exception e) {
                    throw e;
                } finally {
                    inStream.close();
                }
            } catch (Exception e) {
                Log.e(TAG, "download failed: " + e.getMessage());
                e.printStackTrace();
                mExitPending = true;
            }

            boolean ok = onDownloadFinished(!mExitPending);

            if (mListener != null)
                mListener.onDownloadFinished(this, ok);
        }
    }

    public static class ImageDownloadTask extends DownloadTask {

        Bitmap mBitmap = null;

        public Bitmap getDownloadedBitmap() {
            return mBitmap;
        }

        byte[] mImageBuffer = null;

        public ImageDownloadTask(String url) {
            super(url);
        }

        protected boolean onDownloadStarting(int totalSize, InputStream inStream) {
            if (totalSize == -1) {
                mBitmap = BitmapFactory.decodeStream(inStream);
                return true;
            } else {
                mImageBuffer = new byte[totalSize];
                return false;
            }
        }

        protected boolean onDownloadFinished(boolean success) {
            if (success) {
                mBitmap = BitmapFactory.decodeByteArray(mImageBuffer, 0,
                        mImageBuffer.length);
                return mBitmap != null;
            } else {
                mBitmap = null;
                return false;
            }
        }

        protected void onProcessingDownload(byte[] buffer, int bytesRead,
                int bytesDownloaded) {
            for (int i = 0; i < bytesRead; i++) {
                mImageBuffer[i + bytesDownloaded - bytesRead] = buffer[i];
            }
        }
    }
}

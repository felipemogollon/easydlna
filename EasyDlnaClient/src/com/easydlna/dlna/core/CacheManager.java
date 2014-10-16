package com.easydlna.dlna.core;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Hashtable;
import java.util.List;

import android.graphics.Bitmap;
import android.util.Log;

public class CacheManager {

    public final static int IMAGE_THUMBNAIL_CACHE_SIZE = 100;
    public final static int VIDEO_THUMBNAIL_CACHE_SIZE = 100;
    public final static int IMAGE_CACHE_SIZE = 5;

    public final static WindowedObjectCache<String, Bitmap> sVideoThumbnail = new WindowedObjectCache<String, Bitmap>(
            VIDEO_THUMBNAIL_CACHE_SIZE);
    public final static WindowedObjectCache<String, Bitmap> sImageThumbnail = new WindowedObjectCache<String, Bitmap>(
            IMAGE_THUMBNAIL_CACHE_SIZE);
    public final static WindowedObjectCache<String, Bitmap> sImage = new WindowedObjectCache<String, Bitmap>(
            IMAGE_CACHE_SIZE);

    public static void updateVideoThumbnailKeyList(
            List<ContentInfoEx> contentList) {
        ArrayList<String> urlList = new ArrayList<String>();
        for (ContentInfoEx info : contentList) {
            urlList.add(info.thumbnailUrl);
        }
        sVideoThumbnail.setKeyList(urlList);
    }

    public static void updateImageThumbnailKeyList(
            List<ContentInfoEx> contentList) {
        ArrayList<String> urlList = new ArrayList<String>();
        for (ContentInfoEx info : contentList) {
            if (info.thumbnailUrl != null && !info.thumbnailUrl.equals(""))
                urlList.add(info.thumbnailUrl);
            else
                urlList.add(info.resourceUrl);
        }
        sImageThumbnail.setKeyList(urlList);
    }

    public static void updateImageKeyList(List<ContentInfoEx> contentList) {
        ArrayList<String> urlList = new ArrayList<String>();
        for (ContentInfoEx info : contentList) {
            urlList.add(info.resourceUrl);
        }
        sImage.setKeyList(urlList);
    }

    public static class WindowedObjectCache<TKey, TObj> {

        public static final String TAG = "Dlna:WindowedObjectCache";

        private int mCacheSize;

        public int getCacheSize() {
            return mCacheSize;
        }

        private ArrayList<TKey> mKeyList = new ArrayList<TKey>();

        private Hashtable<TKey, TObj> mHashtable = new Hashtable<TKey, TObj>();

        public WindowedObjectCache(int size) {
            mCacheSize = size;
        }

        public synchronized void reset() {
            mKeyList.clear();
            mHashtable.clear();
        }

        public synchronized void setKeyList(Collection<TKey> keyList) {
            mKeyList.clear();
            mKeyList.addAll(keyList);
        }

        public synchronized void put(TKey key, TObj object) {
            if (!mKeyList.contains(key)) {
                Log.d(TAG, "key must be in mKeyList");
                return;
            }

            if (mHashtable.containsKey(key)) {
                mHashtable.put(key, object);
                return;
            }

            mHashtable.put(key, object);

            int indexOfKeyInserted = mKeyList.indexOf(key);

            if (mHashtable.keySet().size() > mCacheSize) {
                int maxIndexInCache = getMaxIndexOfBufferedKey();
                int minIndexInCache = getMinIndexOfBufferedKey();
                int indexOfKeyToRemove = (Math.abs(indexOfKeyInserted
                        - maxIndexInCache) > Math.abs(indexOfKeyInserted
                        - minIndexInCache)) ? maxIndexInCache : minIndexInCache;
                mHashtable.remove(mKeyList.get(indexOfKeyToRemove));
            }
            Log.d("zdw", "cache status:" + mHashtable.size() + "/"
                    + getCacheSize());
        }

        public synchronized TObj get(TKey key) {
            return mHashtable.get(key);
        }

        public int getMinIndexOfBufferedKey() {
            int minIndex = Integer.MAX_VALUE;
            for (TKey key : mHashtable.keySet()) {
                if (minIndex > mKeyList.indexOf(key))
                    minIndex = mKeyList.indexOf(key);
            }
            return minIndex;
        }

        public int getMaxIndexOfBufferedKey() {
            int maxIndex = Integer.MIN_VALUE;
            for (TKey key : mHashtable.keySet()) {
                if (maxIndex < mKeyList.indexOf(key))
                    maxIndex = mKeyList.indexOf(key);
            }
            return maxIndex;
        }
    }
}
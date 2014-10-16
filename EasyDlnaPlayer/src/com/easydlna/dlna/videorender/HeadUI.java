package com.easydlna.dlna.videorender;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class HeadUI extends RelativeLayout {
    private int[] myLocations = null;
    private int width = 0, height = 0;
    private TextView txtView = null;

    public void setTitle(String newTitle) {
        if (txtView == null) {
            txtView = (TextView) findViewById(R.id.title_videoName);
        }
        newTitle = newTitle.replace("http://", "");
        newTitle = newTitle.replace("/", "_");
        txtView.setText(newTitle);
        Log.i(EasyDlnaPlayer.TAG, newTitle);
        txtView.requestFocus();
        txtView.setSelected(true);
    }

    public boolean hitTest(int x, int y) {
        if (myLocations == null) {
            myLocations = new int[2];
            width = getWidth();
            height = getHeight();
        }
        getLocationOnScreen(myLocations);
        if (myLocations[0] < x && x < myLocations[0] + width
                && myLocations[1] < y && y < myLocations[1] + height) {
            return true;
        }
        return false;
    }

    public HeadUI(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
}

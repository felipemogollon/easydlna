package com.easydlna.dlna.dlnaclient;

public class PlayingState {

	final public static String STATE_STOPPED = "STOPPED";
	final public static String STATE_PLAYING = "PLAYING";
	final public static String STATE_PAUSED = "PAUSED_PLAYBACK";
	final public static String STATE_NO_MEDIA = "NO_MEDIA_PRESENT";

	final public static String STATUS_ERROR = "ERROR_OCCURRED";
	final public static String STATUS_OK = "OK";

	private static int ONE_SECOND = 1000;
	private static int ONE_HOUR = 3600;
	private static int ONE_MINUTE = 60;

	public static String formatTime(int time) {
		time = time / ONE_SECOND;
		int hour = time / ONE_HOUR;
		int minute = (time - hour * ONE_HOUR) / ONE_MINUTE;
		if (minute < 0) {
			minute = 0;
		}
		int seconds = time - minute * ONE_MINUTE - hour * ONE_HOUR;
		if (seconds < 0) {
			seconds = 0;
		}
		return String.format("%2d:%2d:%2d", hour, minute, seconds);
	}

	public String avtUri = "";
	public String state = "";
	public String status = "";
	public String duration = "";

	public PlayingState() {
	}

	public void reset() {
		avtUri = "";
		state = "";
		status = "";
		duration = "";
	}
}

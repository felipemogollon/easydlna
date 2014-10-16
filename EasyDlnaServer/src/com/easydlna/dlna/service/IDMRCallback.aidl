package com.easydlna.dlna.service;

interface IDMRCallback{
	
	int setDataSource(in String urlSource);
	int stop();
	int play();
	int pause();
	int seek(int seekTo);
	int getPosition();
}
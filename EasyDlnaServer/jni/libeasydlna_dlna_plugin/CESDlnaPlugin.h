#ifndef __CESDLNAPLUGIN_H__
#define  __CESDLNAPLUGIN_H__

#include "IDlnaFrame.h"
extern "C"
{
		
	int	GetSupportedDlnaComponents(const char* modelName,char** & pComponentNameList);

	IDMController* GetDMController(const char* modelName);

	IDMPlayer* GetDMPlayer(const char* modelName);

	IDMRender* GetDMRender(const char* modelName);

	IDMServer* GetDMServer(const char* modelName);


};












#endif

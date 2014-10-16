#ifndef __CDLNAFRAME_H__
#define __CDLNAFRAME_H__

#include "IUpnpDefine.h"

#define DLNA_ALL ""

#define DLNA_TYPE_DMCONTROLLER  "urn:easydlna:device:MediaController:1"   // not a device, just fake name;
#define DLNA_TYPE_DMPLAYER		"urn:schemas-upnp-org:device:MediaPlayer:1" //unsure about this;
#define DLNA_TYPE_DMRENDER		"urn:schemas-upnp-org:device:MediaRenderer:1"
#define DLNA_TYPE_DMSERVER		"urn:schemas-upnp-org:device:MediaServer:1"

class IDMController;
// class IDMPlayer;
class IDMRender;
class IDMServer;

typedef void IDMPlayer;


#define FUNC_GetSupportedDlnaComponents "GetSupportedDlnaComponents"
typedef int 	(*GetSupportedDlnaComponentsPtr)(const char* modelName,
												 char** & pComponentNameList);
#define FUNC_GetDMController	"GetDMController"
typedef IDMController* (*GetDMControllerPtr)(const char* modelName);

#define FUNC_GetDMPlayer	"GetDMPlayer"
typedef IDMPlayer* (*GetDMPlayerPtr)(const char* modelName);

#define FUNC_GetDMRender	"GetDMRender"
typedef IDMRender* (*GetDMRenderPtr)(const char* modelName);

#define FUNC_GetDMServer	"GetDMServer"
typedef IDMServer* (*GetDMServerPtr)(const char* modelName);





class IDlnaFrame
{

public:
	virtual ~IDlnaFrame(){};
	static  IDlnaFrame* Instantiate(const char* pPluginDirName,const char*pIp,unsigned short port);
	static  ERROR_TYPE  DestroyDlnaFrame(IDlnaFrame* ptrDlnaFrame);

	virtual int	GetSupportedDlnaComponnents(const char* modelName,char** & pComponentNameList) = 0;
	
	virtual IDMController* GetDMController(const char* modelName) = 0;
	
	virtual IDMPlayer* GetDMPlayer(const char* modelName) = 0;

	virtual IDMRender* GetDMRender(const char* modelName) = 0;

	virtual IDMServer* GetDMServer(const char* modelName) = 0;
};









#endif

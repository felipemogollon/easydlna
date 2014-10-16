#include "CESDlnaPlugin.h"

#include "IDMController.h"
#include "IDMServer.h"
#include "IDMRender.h"
#include "CESDMController.h"
#include "CESDMRender.h"
#include "CESDMServer.h"

#define MODULE_TAG "easydlna"


int	GetSupportedDlnaComponents(const char* modelName,char** & pComponentNameList)
{
	if(strcasecmp(MODULE_TAG,modelName) != 0)
		return 0;
	
	pComponentNameList = (char**)(malloc(3*sizeof(char*)));
	pComponentNameList[0] = strdup(DLNA_TYPE_DMCONTROLLER);
	pComponentNameList[1] = strdup(DLNA_TYPE_DMRENDER);
	pComponentNameList[2] = strdup(DLNA_TYPE_DMSERVER);

	return 3;
}

IDMController* GetDMController(const char* modelName)
{
	if(strcasecmp(MODULE_TAG,modelName) != 0)
		return NULL;
	return new CESDMController();
}

IDMPlayer* GetDMPlayer(const char* modelName)
{
	if(strcasecmp(MODULE_TAG,modelName) != 0)
		return NULL;
	return NULL;
}

IDMRender* GetDMRender(const char* modelName)
{
	if(strcasecmp(MODULE_TAG,modelName) != 0)
		return NULL;
	return new CESDMRender();
}

IDMServer* GetDMServer(const char* modelName)
{
	if(strcasecmp(MODULE_TAG,modelName) != 0)
		return NULL;
	//#ifndef OS_ANDROID
	return new CESDMServer();
	//#else
	//return NULL;
	//#endif
}


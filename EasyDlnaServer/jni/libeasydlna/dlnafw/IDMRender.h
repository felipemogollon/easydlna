#ifndef __IDMRENDER_H__
#define __IDMRENDER_H__
#include "IUpnpDefine.h"
class IDlnaService;
class IUpnp;
class IDlnaConnetionManager;
class IDlnaAVTransport;
class IDlnaRenderingControl;
class IDlnaDeviceNortifyHandle;

#define DLNA_WEB_RENDER_ROOTDIR "/dlna/Render"

class IDMRender
{
public:
	IDMRender(){};
	virtual ~IDMRender(){};

public:	
	
	//following three functions should be called before calling RegisterDMRender;
	virtual int SetDlnaDeviceNortifyHandler(IDlnaDeviceNortifyHandle* pNortifyHandler) = 0;
	virtual int SetAVTransport(IDlnaAVTransport* pIAVTransport) = 0;
	virtual int SetRenderingControl(IDlnaRenderingControl* pIRenderingControl) = 0;
	virtual int SetConnectionManager(IDlnaConnetionManager* pIConnectionManager) = 0;
	
	virtual int RegisterDMRender(const char* content,BOOL bFromLocalFile,const char* friendlyName = NULL) = 0;
	virtual int Broadcast(int TimeOut) = 0;
	virtual int Release() = 0;

	virtual IUpnp*      QueryUpnp() = 0;
};










#endif

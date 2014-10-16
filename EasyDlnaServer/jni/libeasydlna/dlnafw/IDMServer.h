#ifndef __IDMSERVER_H__
#define  __IDMSERVER_H__

#include "IUpnpDefine.h"
class IDlnaService;
class IUpnp;
class IDlnaConnetionManager;
class IDlnaAVTransport;
class IDlnaContentDirectory;
class IWebVFSCallbacks;
class IDlnaDeviceNortifyHandle;


#define DLNA_WEB_SERVER_ROOTDIR  "/dlna/Server"

class IDMServer
{
public:
	IDMServer(){};
	virtual ~IDMServer(){};

public:
	virtual int SetDlnaDeviceNortifyHandler(IDlnaDeviceNortifyHandle* pNortifyHandler) = 0;
	virtual int SetAVTransport(IDlnaAVTransport* pIAVTransport) = 0;
	virtual int SetContentDirectroy(IDlnaContentDirectory* pIContentDirectory) = 0;
	virtual int SetConnectionManager(IDlnaConnetionManager* pIConnectionManager) = 0;
	virtual int SetVFSCallbackInterface(IWebVFSCallbacks* pIWebVFSCallbacks) = 0;
	virtual IWebVFSCallbacks* QueryVFSCallbackInterface() = 0;
	virtual int AddContentDirectory(const char* dirName) = 0;
	virtual int RemoveContentDirectory(const char* dirName) = 0;

	virtual int RegisterDMServer(const char* content,BOOL bFromLocalFile,const char* friendlyName = NULL) = 0;
	virtual int Broadcast(int TimeOut) = 0;
	virtual int Release() = 0;

	virtual IUpnp*      QueryUpnp() = 0;

	

};













#endif

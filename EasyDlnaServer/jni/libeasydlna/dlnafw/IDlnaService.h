#ifndef __IDLNADEFINE_H__
#define __IDLNADEFINE_H__

#include "IUpnpUtil.h"
#include "IUpnp.h"

#define DLNA_WEB_ROOTDIR "/dlna"

class IDlnaConnetionManager;
class IDlnaAVTransport;
class IDlnaRenderingControl;
class IDlnaContentDirectory;

struct StateVarableEntry
{
	const char* VarName;
	BOOL bSendEvents;

};

#define VAR_TABLE_LENGTH(table) (sizeof(table)/sizeof(table[0]))

enum DLNA_SERVICE_TYPE
{
	DLNA_ST_UNKNOWN			 = -1,
	DLNA_ST_CONTENTDIRECTORY = 0,
	DLNA_ST_CONNECTIONMANAGER = 1,
	DLNA_ST_AVTRANSPORT		  = 2,
	DLNA_ST_RENDERINGCONTROL  = 3,
};

typedef unsigned short ui2;
typedef short i2;

typedef unsigned int ui4;
typedef int i4;

typedef const char* icstring;
typedef char* istring;

typedef long long i64;

class IUpnpObserver;
class IDlnaService;


class IDlnaServiceObserver
{
public:
	virtual ~IDlnaServiceObserver(){};
	virtual int Nortify(IDlnaService *pSource, CDevice* pSrcDevice,IXML_Document * changeValueList) = 0;
};


class IDlnaService
{
public:
	virtual ~IDlnaService(){};
	virtual int SetObserver(IDlnaServiceObserver * pObserver){return 0;};
	IDlnaService()
	{
		m_dlnaServiceType = DLNA_ST_UNKNOWN;
	};
	DLNA_SERVICE_TYPE GetServiceType()
	{
		return m_dlnaServiceType;
	};
	
	virtual const char* GetErrorMessage(int errorCode)
	{
		return IUpnpUtil::GetErrorMessage(errorCode);
	}
	virtual void NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify){return;};
	virtual int Subscribe(BOOL bSub, int Timeout = 0){return ERROR_INVALID_SERVICE;};
	virtual istring GetStatusValue(const char* ValName){return NULL;};
	virtual int SetStatusValue(const char* ValName,const char* NewValue){return ERROR_INVALID_SERVICE;};

	virtual int GetPropSet(IXML_Document*& PropSet){return ERROR_INVALID_SERVICE;};
	virtual int SetPropSet(const IXML_Document* PropSet){return ERROR_INVALID_SERVICE;};
	virtual int SetActive(const char* udn){return ERROR_SUCCESS;};
public:
	DLNA_SERVICE_TYPE m_dlnaServiceType;
};

class IDlnaDeviceNortifyHandle
{
public:
	IDlnaDeviceNortifyHandle(){};

	virtual ~IDlnaDeviceNortifyHandle(){};

public:
	virtual int SetUpnpDevice(IUpnpDevice* pUpnpDevice) = 0;
	virtual int SetAVTransport(IDlnaAVTransport* pIAVTransport) = 0;
	virtual int SetRenderingControl(IDlnaRenderingControl* pIRenderingControl) = 0;
	virtual int SetConnectionManager(IDlnaConnetionManager* pIConnectionManager)= 0;
	virtual int SetContentDirectory(IDlnaContentDirectory* pIContentDirectory) = 0;
public:
	virtual ERROR_TYPE HandleDlnaActionRequest(CEventActionRequest & actionReq)
	{
		return ERROR_INVALID_ACTION;
	};
	virtual ERROR_TYPE HandleDlnaGetVarRequest(CEventStateVarRequest & eventGetVar)
	{
		return ERROR_INVALID_ACTION;
	};
	virtual ERROR_TYPE HandleDlnaSubRequest(CEventSubscriptionRequest & eventSubsReq)
	{
		return ERROR_INVALID_ACTION;
	}

};


#endif


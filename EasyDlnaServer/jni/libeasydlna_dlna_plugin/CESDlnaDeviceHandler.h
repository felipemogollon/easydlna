#ifndef __CESDLNADEVICEHANDLER_H__
#define __CESDLNADEVICEHANDLER_H__

#include "IDlnaService.h"

class IUpnp;
class IDlnaAVTransport;
class IDlnaConnetionManager;
class IDlnaContentDirectory;
class IDlnaRenderingControl;


class CESDlnaDeviceHandler: public IDlnaDeviceNortifyHandle
{
public:
	CESDlnaDeviceHandler();
	virtual ~CESDlnaDeviceHandler();

public:
	virtual int SetUpnpDevice(IUpnpDevice* pUpnpDevice)
	{
		m_IUpnp = pUpnpDevice;
		return 0;
	}
	virtual int SetAVTransport(IDlnaAVTransport* pIAVTransport)
	{
		m_pAVTransport = pIAVTransport;
		return 0;		
	}
	virtual int SetRenderingControl(IDlnaRenderingControl* pIRenderingControl)
	{
		m_pRenderingControl = pIRenderingControl;
		return 0;		
	}
	virtual int SetConnectionManager(IDlnaConnetionManager* pIConnectionManager)
	{
		m_pConnectionManager = pIConnectionManager;
		return 0;		
	}
	virtual int SetContentDirectory(IDlnaContentDirectory* pIContentDirectory)
	{
		m_pContentDirectory = pIContentDirectory;
		return 0;		
	}
public:
	virtual ERROR_TYPE HandleDlnaActionRequest(CEventActionRequest & actionReq);
	virtual ERROR_TYPE HandleDlnaGetVarRequest(CEventStateVarRequest & eventGetVar);
	virtual ERROR_TYPE HandleDlnaSubRequest(CEventSubscriptionRequest & eventSubsReq);
private:
	ERROR_TYPE HandleCMSAction(CEventActionRequest &actionReq);
	ERROR_TYPE HandleRCSAction(CEventActionRequest &actionReq);
	ERROR_TYPE HandleAVTAction(CEventActionRequest &actionReq);
	ERROR_TYPE HandleCDSAction(CEventActionRequest &actionReq);
private:
	ERROR_TYPE HandleAVTSubRequest(CEventSubscriptionRequest & eventSubsReq);
	ERROR_TYPE HandleRCSSubRequest(CEventSubscriptionRequest & eventSubsReq);
	ERROR_TYPE HandleCMSSubRequest(CEventSubscriptionRequest & eventSubsReq);
	ERROR_TYPE HandleCDSSubRequest(CEventSubscriptionRequest & eventSubsReq);

private:
	ERROR_TYPE HandleCMSGetVar(CEventStateVarRequest &eventGetVar);
	ERROR_TYPE HandleRCSGetVar(CEventStateVarRequest &eventGetVar);
	ERROR_TYPE HandleAVTGetVar(CEventStateVarRequest &eventGetVar);
	ERROR_TYPE HandleCDSGetVar(CEventStateVarRequest &eventGetVar);
private:
	IUpnpDevice* m_IUpnp;
	IDlnaRenderingControl* m_pRenderingControl;
	IDlnaConnetionManager* m_pConnectionManager;
	IDlnaContentDirectory* m_pContentDirectory;
	IDlnaAVTransport* m_pAVTransport;

};



















#endif


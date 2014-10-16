#define LOG_TAG "easydlna-upnpdevice"
#include "CUpnpDevice.h"

CUpnpDevice::CUpnpDevice()
:m_devicehandle(-1),
m_deviceRegisterCount(0),
m_observer(NULL)
{

}

CUpnpDevice::~CUpnpDevice()
{

}

ERROR_TYPE CUpnpDevice::Register(const CRegisterType & cType)
{
	if(m_deviceRegisterCount > 1)
		return ERROR_OUTOF_CONTEXT;
	
	int nret  = 0;
	if(cType.nDeclareType == UPNP_ROOTDEVICE)
	{
		const CRegisterTypeRootDevice & registerType = 
				static_cast<const CRegisterTypeRootDevice &>(cType);
		
		nret = UpnpRegisterRootDevice(registerType.DescUrl,(Upnp_FunPtr)CUpnpDevice::DeviceCallBack,
									  this,&m_devicehandle);
		Trace("Register Root Device:%d",nret);

	}
	else if(cType.nDeclareType == UPNP_ROOTDEVICE2)
	{
		const CRegisterTypeRootDevice2 & registerType = 
			static_cast<const CRegisterTypeRootDevice2&>(cType);

		nret = UpnpRegisterRootDevice2((Upnp_DescType)registerType.descriptionType,registerType.description,
										registerType.bufferLen,registerType.config_baseURL,
										(Upnp_FunPtr)CUpnpDevice::DeviceCallBack,this,&m_devicehandle);
		Trace("Register Root Device 2:%d",nret);
	}
	else
		return ERROR_INVALID_PARAM;
	
	if(nret == UPNP_E_SUCCESS)
		++m_deviceRegisterCount;
	
	return (ERROR_TYPE)nret;
}
ERROR_TYPE CUpnpDevice::UnRegister()
{
	if(--m_deviceRegisterCount == 0)
	{
		UpnpUnRegisterRootDevice(m_devicehandle);
		m_devicehandle = -1;
	}
	
	return ERROR_SUCCESS;
}

ERROR_TYPE CUpnpDevice::AddUpnpDeviceObserver(IUpnpDeviceObserver* IObserver)
{
	QUICKWRITELOCK(&m_rwLock);
	if(m_observer)
		return ERROR_OUTOF_CONTEXT;
	
	m_observer = IObserver;
	return ERROR_SUCCESS;
}
ERROR_TYPE CUpnpDevice::RemoveUpnpDeviceObserver(IUpnpDeviceObserver* IObserver)
{
	QUICKWRITELOCK(&m_rwLock);
	if(m_observer)
		m_observer = NULL;

	return ERROR_SUCCESS;
}

void CUpnpDevice::DeviceCallBack(EventType eventType, void *Event, void *Cookie)
{
	return ((CUpnpDevice*)Cookie)->innerDeviceCallBack(eventType,Event);
}
void  CUpnpDevice::innerDeviceCallBack(EventType eventType, void *Event)
{
	switch(eventType)
	{
	case ET_CONTROL_ACTION_REQUEST:
		{
			CEventActionRequest eventActionRequest;
			eventActionRequest.Translate(Event);
			LoopCallObserver(eventType,eventActionRequest);
			eventActionRequest.SetResult(Event);
		}
		break;
	case ET_CONTROL_GET_VAR_REQUEST:
		{
			CEventStateVarRequest eventGetVar;
			eventGetVar.Translate(Event);
			LoopCallObserver(eventType,eventGetVar);
			eventGetVar.SetResult(Event);
			
		}
		break;
	case ET_EVENT_SUBSCRIPTION_REQUEST:
		{
			CEventSubscriptionRequest eventSubsReq;
			eventSubsReq.Translate(Event);
			LoopCallObserver(eventType,eventSubsReq);
		}
	default:
	    break;
	}
	
}
void CUpnpDevice::LoopCallObserver(EventType eventType,CEvent & event)
{
	//QUICKREADLOCK(&m_rwLock);
	QUICKWRITELOCK(&m_rwLock);
	if(m_observer)
		m_observer->Nortify(eventType,event);
	
}
ERROR_TYPE CUpnpDevice::ExecuteCommand(CCommand & command)
{
	if(m_devicehandle == -1)
		return ERROR_INTERNAL_ERROR;

	COMMAND_TYPE nType = command.nType;
	int nret = 0;
	switch(nType)
	{
	case CMD_TYPE_SENDADVERTISEMENT:
		{
			CCommandSendAdvertisement & cmdSendAdvertise = 
					static_cast<CCommandSendAdvertisement&>(command);

			nret = UpnpSendAdvertisement(m_devicehandle,cmdSendAdvertise.Exp);
		}
		break;
	case CMD_TYPE_ACCEPTSUBSCRIPTION:
		{
			CComandAcceptSubscription & cmdAccept = static_cast<CComandAcceptSubscription&>(command);

			nret = UpnpAcceptSubscription(m_devicehandle,cmdAccept.DevID,cmdAccept.ServID,cmdAccept.VarName,
										 cmdAccept.NewVal,cmdAccept.cVariables,cmdAccept.SubsId);
		}
		break;
	case CMD_TYPE_ACCEPTSUBSCRIPTIONEX:
		{
			CComandAcceptSubscriptionEx & cmdAcceptEx = static_cast<CComandAcceptSubscriptionEx&>(command);

			nret = UpnpAcceptSubscriptionExt(m_devicehandle,cmdAcceptEx.DevID,cmdAcceptEx.ServID,
											cmdAcceptEx.PropSet,cmdAcceptEx.SubsId);
		}
	    break;
	case CMD_TYPE_NORTIFY:
		{
			CComandNortify & cmdNortify = static_cast<CComandNortify &>(command);
			nret = UpnpNotify(m_devicehandle,cmdNortify.DevID,cmdNortify.ServID,cmdNortify.VarName,
							 cmdNortify.NewVal,cmdNortify.cVariables);
		}
	    break;
	case CMD_TYPE_NORTIFYEX:
		{
			CComandNortifyEx & cmdNortifyEx = static_cast<CComandNortifyEx &>(command);
			nret = UpnpNotifyExt(m_devicehandle,cmdNortifyEx.DevID,cmdNortifyEx.ServID,cmdNortifyEx.PropSet);
		}
		break;
	case CMD_TYPE_SETMAXSUBSCRIPTION:
		{
			CCommandSetMaxSubscription & cmdSetMaxSubs = static_cast<CCommandSetMaxSubscription & >(command);
			nret = UpnpSetMaxSubscriptions(m_devicehandle,cmdSetMaxSubs.MaxSubscriptions);
		}
		break;
	case CMD_TYPE_SETMAXSUBSCRIPTIONTIMEOUT:
		{
			CCommandSetSubscriptionTimeOut & cmdSetMaxSubsTimeOut = static_cast<CCommandSetSubscriptionTimeOut & >(command);
			nret = UpnpSetMaxSubscriptionTimeOut(m_devicehandle,cmdSetMaxSubsTimeOut.MaxSubscriptionTimeOut);
		}
	    break;
	default:
		return IUpnp::ExecuteCommand(command);
	}
	

	return (ERROR_TYPE)nret;
}

#define LOG_TAG "easydlna-esdmcontroller"
#include "IUpnpUtil.h"
#include "IUpnp.h"
#include "IDlnaFrame.h"
#include "CESDMController.h"
#include "ESResouce.h"
#include "ESDlnaHelper.h"
#include <assert.h>

enum URL_TYPE
{
	URL_TYPE_CONTROL,
	URL_TYPE_EVENT,
	URL_TYPE_SCPD,
	URL_TYPE_PRSENTATION,
	URL_TYPE_DESCRIPTION,
};
struct Service 
{
	char *pServiceId;

	char *pServiceType;

	char* pRelativeEventUrl;
	char *pEventURL; 

	char* pRelativeControlUrl;
	char *pControlURL;

	char* pRelativeSCPDUrl;
	char *pSCPDURL;

	char* pSCPDXML;

	char* pSubId;
	int   durationTime; //first intialized to one hour,60*30(seconds);UPNP minimum is 15 seconds;


	Service* pNext;

	Service()
	{
		pServiceId = pServiceType = pEventURL = pControlURL = pSCPDURL = pSCPDXML = NULL;
		pRelativeEventUrl = pRelativeControlUrl = pRelativeSCPDUrl = NULL;
		pSubId = NULL;
		pNext = NULL;
		durationTime = 1800;

	};

	int ChangeLocation(const char* plocation)
	{
		Service* pIter = this;
		while (pIter)
		{
			if(pIter->pSCPDURL)
			{
				FREE(pIter->pSCPDURL);
				IUpnpUtil::ResolveURL(plocation,pIter->pRelativeSCPDUrl,&pIter->pSCPDURL);
			}

			if(pIter->pControlURL)		
			{
				FREE(pIter->pControlURL);
				IUpnpUtil::ResolveURL(plocation,pIter->pRelativeControlUrl,&pIter->pControlURL);
			}
			if(pIter->pEventURL)
			{
				FREE(pIter->pEventURL);
				IUpnpUtil::ResolveURL(plocation,pIter->pRelativeEventUrl,&pIter->pEventURL);
			}

			pIter = pIter->pNext;
		}

		return 0;

	}
	inline icstring GetSubId(const char* type)
	{
		Service *pService = this;
		while(pService)
		{
			if(strcmp(pService->pServiceType,type) == 0)
			{
				return pService->pSubId;
			}
			pService = pService->pNext;
		}
		return NULL;
	}
	inline icstring DupSubId(const char* url,const char*  newId,int NewTimeOut)
	{
		Service *pService = this;
		while(pService)
		{
			if(strcmp(pService->pEventURL,url) == 0)
			{
				FREEARRAY(pService->pSubId);
				SAFEDUP(pService->pSubId,newId);
				pService->durationTime = NewTimeOut;
				return pService->pSubId;
			}
			pService = pService->pNext;
		}
		return NULL;
	}
	inline void UnSubscribe(IUpnp* pController)
	{
		Service  *pService = this;
		while (pService)
		{
			if(pService->pSubId)
			{
				CCommandUnSubscribe command(FALSE);
				strncpy(command.SubsId,pService->pSubId,44);
				int nret = pController->ExecuteCommand(command);
				Trace("url %s unsbscribe result = %d",pService->pEventURL,nret);
			}
			pService = pService->pNext;
		}
		
	}
	inline int FreeSubId(const char* url)
	{
		Service *pDel = this;
		while(pDel)
		{
			if(strcmp(pDel->pEventURL,url) == 0)
			{
				FREE(pDel->pSubId);
				pDel->durationTime = 1800;
				return 0;
			}
			pDel = pDel->pNext;
		}

		return -1;
	}
	inline int FreeSubId()
	{
		Service *pDel = this;
		while(pDel)
		{
			FREE(pDel->pSubId);
			pDel->durationTime = 1800;
			pDel = pDel->pNext;
		}
		return 0;
	}
	inline void FreeList()
	{
		Service *pDel = NULL;
		while(pNext)
		{
			pDel = pNext;
			pNext = pNext->pNext;
			DELETE(pDel);
		}
	}
	inline BOOL isSidMatch(const char* subid)
	{
		Service *pNode = this;
		while (pNode)
		{
			if(pNode->pSubId && strcmp(subid,pNode->pSubId) == 0)
				return TRUE;

			pNode = pNode->pNext;
		}

		return FALSE;

	}
	inline const char* getUrlByServiceType(URL_TYPE type,const char* serviceType)
	{
		Service* pNode = this;
		while(pNode)
		{
			if(type == URL_TYPE_EVENT && strcmp(serviceType,pNode->pServiceType) == 0)
				return pNode->pEventURL;
			else if(type == URL_TYPE_CONTROL && strcmp(serviceType,pNode->pServiceType) == 0)
				return pNode->pControlURL;
			pNode = pNode->pNext;
		}

		return NULL;

	}
	inline BOOL isUrlMatch(URL_TYPE type,const char* url)
	{
		Service* pNode = this;
		while(pNode)
		{
			switch(type)
			{
			case URL_TYPE_CONTROL:
				{
					if(strcmp(url,pNode->pControlURL) == 0)
						return TRUE;
				}
				break;
			case URL_TYPE_EVENT:
				{
					if(strcmp(url,pNode->pEventURL) == 0)
						return TRUE;
				}
				break;
			case URL_TYPE_SCPD:
				{
					if(strcmp(url,pNode->pSCPDURL) == 0)
						return TRUE;
				}
				break;
			default:
				return FALSE;
			}

			pNode = pNode->pNext;

		}

		return FALSE;
	}
	inline icstring GetServiceTypeBySubId(const char* subId)
	{
		Service *pNode = this;
		while (pNode)
		{
			if(pNode->pSubId && strcmp(subId,pNode->pSubId) == 0)
				return pNode->pServiceType;

			pNode = pNode->pNext;
		}

		return NULL;
	}
	~Service()
	{
		FREE(pServiceId);
		FREE(pServiceType);
		FREE(pEventURL);
		FREE(pControlURL);
		FREE(pSCPDURL);
		FREE(pSubId);
		FREE(pRelativeControlUrl);
		FREE(pRelativeSCPDUrl);
		FREE(pRelativeEventUrl);
	};
};

struct CDetailDevice:public CDevice
{

	Service   * pServiceList;
	
	inline BOOL isSidMatch(const char* subid)
	{
		return pServiceList->isSidMatch(subid);
	}
	inline icstring GetServiceTypeBySubId(const char* subId)
	{
		return pServiceList->GetServiceTypeBySubId(subId);
	}
	CDetailDevice()
	{
		pServiceList = NULL;
	}
	~CDetailDevice()
	{
		pServiceList->FreeList();
		DELETE(pServiceList);
	}
	void UnSubscribe(IUpnp* pController)
	{
		pServiceList->UnSubscribe(pController);
	}
	void FreeSubId()
	{
		pServiceList->FreeSubId();
	}
	void FreeSubId(const char* url)
	{
		pServiceList->FreeSubId(url);
	}
	icstring DupSubId(const char* url,const char* newId,int NewTimeOut)
	{
		return pServiceList->DupSubId(url,newId,NewTimeOut);
	}
	icstring GetSubId(const char* url)
	{
		return pServiceList->GetSubId(url);
	}
	int ChangeLocation(const char *location)
	{
		
		if(pstrDescDocURL)
		{
			FREE(pstrDescDocURL);
			pstrDescDocURL = strdup(location);
		}
		if(pstrPresURL)
		{
			FREE(pstrPresURL);
			IUpnpUtil::ResolveURL(location,pstrRelativePresUrl,&pstrPresURL);
		}
		
		
		
		if(pServiceList)
			pServiceList->ChangeLocation(location);

		parseToBaseUrl(location);
		return 0;
	}
	void parseToBaseUrl(const char* pLocation)
	{
		FREE(pBaseUrl);
		char URL[256]={'\0'};
		int icount  = 0;
		for(int i = 0; i < 256; i++)
		{
			if(pLocation[i] == '/' && ++icount == 3)
				break;
			URL[i] = pLocation[i];
		}
		pBaseUrl = strdup(URL);
	}
};





#define CONTROLLER_QWRITELOCK(pController) QUICKWRITELOCK(&pController->m_rwListLock)
#define CONTROLLER_QREADLOCK(pController) QUICKREADLOCK(&pController->m_rwListLock)

#define GETIUPNP(pController)  (pController->m_pIUpnp)






CESDMController::CESDMController()
:m_pIConnectionManager(NULL),
m_pIDlnaAVTransport(NULL),
m_pIDlnaContentDirtory(NULL),
m_pIDlnaRenderintControl(NULL),
m_pConnectionManager(NULL),
m_pAVTransport(NULL),
m_pContentDirectory(NULL),
m_pRenderingControl(NULL)
{

	m_pIUpnp = IUpnp::CreateUpnpControlPoint();
	
	m_pObserver = NULL;

	assert(m_pIUpnp);

	m_pIUpnp->AddUpnpControlPointObserver((IUpnpControlPointObserver*)this);
	
	CRegisterTypeClient clientRegister;
	ERROR_TYPE ret =	m_pIUpnp->Register(clientRegister);
	if(ret != ERROR_SUCCESS)
	{
		Trace( "fail to register\n");
	}
}

CESDMController::~CESDMController()
{
}
int CESDMController::SetContentDirectory(IDlnaContentDirectory* pIContentDirectory)
{
	if(m_pContentDirectory)
		return -1;
	m_pIDlnaContentDirtory = pIContentDirectory;
	return 0;
}
int CESDMController::SetAVTransport(IDlnaAVTransport* pIAVTransport)
{
	if(m_pAVTransport)
		return -1;
	m_pIDlnaAVTransport = pIAVTransport;
	return 0;
}
int CESDMController::SetRenderingControl(IDlnaRenderingControl* pIRenderingControl)
{
	if(m_pRenderingControl)
		return -1;
	m_pIDlnaRenderintControl = pIRenderingControl;
	return 0;
}
int CESDMController::SetConnectionManager(IDlnaConnetionManager* pIConnectionManager)
{
	if(m_pConnectionManager)
		return -1;
	m_pIConnectionManager = pIConnectionManager;
	return 0;
}

int CESDMController::ScanNetwork(const char* scanContent, int TimeOut)
{
	if(!m_pContentDirectory && !m_pIDlnaContentDirtory)
	{
		m_pContentDirectory = new CESContentDirectory(this);
	}


	if(!m_pAVTransport&& !m_pIDlnaAVTransport)
	{
		m_pAVTransport = new CESAVTransport(this);
	}
	if(!m_pConnectionManager && !m_pIConnectionManager)
	{
		m_pConnectionManager = new CESConnetionManager(this);
	}
	if(!m_pRenderingControl && !m_pIDlnaRenderintControl)
	{
		m_pRenderingControl = new CESRenderingControl(this);
	}
	
	
	
	
	CCommandSearch cmdSearch;
	cmdSearch.Target = scanContent;
	cmdSearch.Mx  = TimeOut;
	
	return (int)m_pIUpnp->ExecuteCommand(cmdSearch);
}

int CESDMController::SetObserver(IDMControllerObserver * observer)
{
	IUpnpUtil::CQuickRWLock quickLock(&m_rwListLock,FALSE);
	assert(m_pObserver == NULL);
	m_pObserver = observer;
	return 0;
}

IDlnaService* CESDMController::QueryService(const char* serviceType)
{
	if(strcmp(serviceType,DLNA_AVTRANSPORT) == 0)
		return m_pAVTransport==NULL?m_pIDlnaAVTransport:m_pAVTransport;
	else if(strcmp(serviceType,DLNA_CONNECTIONMANAGER) == 0)
		return m_pConnectionManager==NULL?m_pIConnectionManager:m_pConnectionManager;
	else if(strcmp(serviceType,DLNA_CONTENTDIRECTORY) == 0)
		return m_pContentDirectory==NULL?m_pIDlnaContentDirtory:m_pContentDirectory;
	else if(strcmp(serviceType,DLNA_RENDERINGCONTROL) == 0)
		return m_pRenderingControl==NULL?m_pIDlnaRenderintControl:m_pRenderingControl;
	else
	{
		Trace("Unknown service type [%s]\n",serviceType);
		return NULL;
	}
}

IUpnp*   CESDMController::QueryUpnp()
{
	return m_pIUpnp;
}
void CESDMController::UnsbscribeDevices(CListDevices* pList)
{
	CListDevices::iterator itbegin = pList->begin();
	CListDevices::iterator itend = pList->end();

	CCommandUnSubscribe ubsubs(FALSE);
	for (itbegin; itbegin != itend; ++itbegin)
	{
		CDetailDevice * pDevice = (CDetailDevice*)(*itbegin);
		if(pDevice)
			pDevice->UnSubscribe(m_pIUpnp);

		
	}
	pList->clear();
}
void CESDMController::UnsbscribeAllDevices()
{
	IUpnpUtil::CQuickRWLock rwLock(&m_rwListLock,FALSE);
	UnsbscribeDevices(&m_listRender);
	UnsbscribeDevices(&m_listServer);
	UnsbscribeDevices(&m_listPlayer);
}
int CESDMController::Release()
{
// 	if(m_pConnectionManager)
// 		m_pConnectionManager->Subscribe(FALSE);
// 	else
// 		m_pIConnectionManager->Subscribe(FALSE);
// 	
// 	if(m_pAVTransport)
// 		m_pAVTransport->Subscribe(FALSE);
// 	else
// 		m_pIDlnaAVTransport->Subscribe(FALSE);
// 	
// 	if(m_pContentDirectory)
// 		m_pContentDirectory->Subscribe(FALSE);
// 	else
// 		m_pIDlnaContentDirtory->Subscribe(FALSE);
// 
// 	if(m_pRenderingControl)
// 		m_pRenderingControl->Subscribe(FALSE);
// 	else
// 		m_pIDlnaRenderintControl->Subscribe(FALSE);
	UnsbscribeAllDevices();
	m_pIUpnp->RemoveUpnpControlPointObserver((IUpnpControlPointObserver*)this);
	m_pIUpnp->UnRegister();
	IUpnp::DestroyIUpnp(m_pIUpnp);
	DELETE(m_pConnectionManager);
	DELETE(m_pAVTransport);
	DELETE(m_pContentDirectory);
	DELETE(m_pRenderingControl);

	IUpnpUtil::CQuickRWLock rwLock(&m_rwListLock,FALSE);
	ClearDevices(&m_listRender);
	ClearDevices(&m_listServer);
	ClearDevices(&m_listPlayer);

	m_pObserver = NULL;
	
	delete(this);
	
   return 0;
}
void CESDMController::SetDeviceSubscribeResult(icstring strUDN,icstring strSubUrl,CCommandSubscribe & subcomand)
{
	CDetailDevice *pDevice = NULL;
	
	if(m_LockContext == 0)
	{	
		IUpnpUtil::CQuickRWLock rwLock(&m_rwListLock,FALSE);
	    pDevice = HitDevice(strUDN);
		if(!pDevice)
			return;
		if(strcmp(pDevice->pstrUDN,strUDN) == 0 && pDevice->pServiceList)
		{
			Trace("new subid = %s\n",(icstring)subcomand.SubsId);
			pDevice->DupSubId(strSubUrl,(icstring)subcomand.SubsId,subcomand.TimeOut);
			return ;
		}
	}
	else
	{
		pDevice = HitDevice(strUDN);
		if(!pDevice)
			return;
		if(strcmp(pDevice->pstrUDN,strUDN) == 0 && pDevice->pServiceList)
		{
			Trace("new subid = %s\n",(icstring)subcomand.SubsId);
			pDevice->DupSubId(strSubUrl,(icstring)subcomand.SubsId,subcomand.TimeOut);
			return ;
		}
	}
	return ;
}
icstring CESDMController::GetDeviceSubUrl(icstring udn,icstring serviceType)
{
	CDetailDevice *pDevice = NULL;
	if(m_LockContext == 0)
	{	
		IUpnpUtil::CQuickRWLock rwLock(&m_rwListLock,FALSE);
	    pDevice = HitDevice(udn);
		if(!pDevice)
			return NULL;
		if(strcmp(pDevice->pstrUDN,udn) == 0 && pDevice->pServiceList)
		{
			icstring strReturn =  pDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,serviceType);
			if(strReturn)
				return strdup(strReturn);
			else
				return NULL;
	    }

	}
	else
	{
	    pDevice = HitDevice(udn);
		if(!pDevice)
			return NULL;
		if(strcmp(pDevice->pstrUDN,udn) == 0 && pDevice->pServiceList)
		{
			icstring strReturn =  pDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,serviceType);
			if(strReturn)
				return strdup(strReturn);
			else
				return NULL;
	    }
	}
	return NULL;
}
icstring CESDMController::GetDeviceActionUrl(icstring udn,icstring serviceType)
{
	CDetailDevice *pDevice = NULL;
	if(m_LockContext == 0)
	{	
		IUpnpUtil::CQuickRWLock rwLock(&m_rwListLock,FALSE);
	    pDevice = HitDevice(udn);
		if(!pDevice)
			return NULL;
			if(strcmp(pDevice->pstrUDN,udn) == 0 && pDevice->pServiceList)
			{
				icstring strReturn =  pDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,serviceType);
				if(strReturn)
					return strdup(strReturn);
				else
					return NULL;
			}

	}
	else
	{
	    pDevice = HitDevice(udn);
		if(!pDevice)
			return NULL;
		if(strcmp(pDevice->pstrUDN,udn) == 0 && pDevice->pServiceList)
		{
			icstring strReturn =  pDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,serviceType);
			if(strReturn)
				return strdup(strReturn);
			else
				return NULL;
	    }
	}
	return NULL;
}
void CESDMController::ClearDevices(CListDevices* pList)
{
	CListDevices::iterator itbegin = pList->begin();
	CListDevices::iterator itend = pList->end();

	for (itbegin; itbegin != itend; ++itbegin)
	{
		CDetailDevice * pDevice = (CDetailDevice*)(*itbegin);
		DELETE(pDevice);
	}
	pList->clear();
}
int	 CESDMController::CanHandleThisType(const char* deviceType)
{
	if(strcmp(deviceType,DLNA_TYPE_DMCONTROLLER) == 0 ||
	   strcmp(deviceType,DLNA_TYPE_DMPLAYER) == 0 ||
	   strcmp(deviceType,DLNA_TYPE_DMRENDER) == 0 ||
	   strcmp(deviceType,DLNA_TYPE_DMSERVER) == 0)
	   return 0;
	
	return -1;
}
NORTIFY_HANDLE_RESULT CESDMController::Nortify(EventType type, CEvent & event)
{
		switch(type)
		{
		case ET_DISCOVERY_ADVERTISEMENT_ALIVE:
		case ET_DISCOVERY_ADVERTISEMENT_BYEBYE:
		case ET_DISCOVERY_SEARCH_RESULT:
			{
				CEventDiscovery & discoveryEvent = static_cast<CEventDiscovery &>(event);
				if(CanHandleThisType(discoveryEvent.DeviceType) != 0)
					return NHR_CONTINUE;

				HandleDeviceStatus(discoveryEvent,type);

				return NHR_BREAK;
			}
		    break;
		case ET_EVENT_TICKCOUNT:
			{
				CEventTickCount & tickCountEvent = static_cast<CEventTickCount&>(event);
				HandleDeviceTickCount(tickCountEvent);
				
				return NHR_CONTINUE;
			}
			break;
		case ET_EVENT_RECEIVED:
			{
				CEventEvent & eventNortify = static_cast<CEventEvent&>(event);
				HandleDeviceStateVar(eventNortify);
			}
			break;
		case ET_EVENT_AUTORENEWAL_FAILED:
			{
				QUICKWRITELOCK(&m_rwListLock);

				CEventSubscribe & cevent = static_cast<CEventSubscribe&>(event);
				CDetailDevice *pDevice = HitDeviceBySubId((const char*)cevent.Sid);
				if(!pDevice)
				{
					Trace(
						"RenewFailed of subid %s notification without matched device\n",(const char*)cevent.Sid);
				}
				else
					DropActiveAllInOne(pDevice,cevent.Sid);
				
			}
			break;
		default:
		    return NHR_CONTINUE;
			break;
		}

		return NHR_CONTINUE;
}
void CESDMController::HandleDeviceTickCount(CEventTickCount & tickCountEvent)
{
	int count = tickCountEvent.nTickCount;

	QUICKWRITELOCK(&m_rwListLock);
	
	m_LockContext = 1;

	UpdateTickCountOneByOne(&m_listServer,count);
	UpdateTickCountOneByOne(&m_listRender,count);
	UpdateTickCountOneByOne(&m_listPlayer,count);

	m_LockContext = 0;
	
}
void CESDMController::UpdateTickCountOneByOne(CListDevices *pList,int count)
{
	CListDevices::iterator itbegin = pList->begin();
	CListDevices::iterator itend = pList->end();
	
	int nret  = 0;

	CCommandSearch searchCommand;
	searchCommand.Mx = count;
	
	for (itbegin; itbegin != itend; ++itbegin)
	{
		CDetailDevice* pDevice = (CDetailDevice*)(*itbegin);

		pDevice->AdvrTimeOut -= count;
		
		if(pDevice->AdvrTimeOut <= 0)
		{
			pDevice->onLine = FALSE;
			
			Trace("Device [%s] dye because of tick count timeout\n",
					pDevice->pstrUDN);
			
			DropActiveAllInOne(pDevice);
			NortifyControllerObserver(pDevice);

			pList->erase(itbegin);
			DELETE(pDevice);
			itbegin = pList->begin();
			
			continue;

		}
		else if(pDevice->AdvrTimeOut < (count<<1))
		{
			searchCommand.Target = pDevice->pstrUDN;
			nret = m_pIUpnp->ExecuteCommand(searchCommand);
			if(nret != UPNP_E_SUCCESS)
			{
				Trace(
						"Search device [%s] fails--[%s]\n",
						pDevice->pstrUDN,IUpnpUtil::GetErrorMessage(nret));
			}
		}
		
// 		CCommandRenewSubscription cmdRenewSub(FALSE);
// 		
// 		Service *pService = pDevice->pServiceList;
// 		while (pService)
// 		{	
// 			if(pService->pSubId)
// 			{
// 				pService->durationTime -= count;
// 				if(pService->durationTime <= 0)
// 				{
// 					cmdRenewSub.TimeOut = 1800;
// 
// 					strncpy(cmdRenewSub.SubsId,pService->pSubId,44);
// 
// 					nret = m_pIUpnp->ExecuteCommand(cmdRenewSub);
// 					if(nret != ERROR_SUCCESS)
// 					{
// 						IUpnpUtil::TraceOut(IUpnpUtil::TRACE_ERROR,
// 							"Renew Subscribe to [%s] fails\n",pService->pEventURL);
// 
// 						DropActiveAllInOne(NULL,pService->pSubId);
// 					}
// 					else
// 					{
// 						pService->durationTime = cmdRenewSub.TimeOut;
// 					}
// 				}
// 
// 			}
// 			pService = pService->pNext;
// 		}
		
	}

}
CDetailDevice * CESDMController::HitDevice(const char* udn,BOOL bRemove/*= FALSE*/)
{
	CDetailDevice *pReturn = HitDevice(&m_listRender,udn,bRemove);
	if(!pReturn)
		pReturn = HitDevice(&m_listServer,udn,bRemove);
	if(!pReturn)
		pReturn = HitDevice(&m_listPlayer,udn,bRemove);

	return pReturn;
}
CDetailDevice * CESDMController::HitDevice(CListDevices * plistDevices,const char* udn, BOOL bRemove/*= FALSE*/)
{
	CListDevices::iterator itbegin = plistDevices->begin();
	CListDevices::iterator itend = plistDevices->end();

	for (itbegin; itbegin != itend; ++itbegin)
	{
		CDevice *pDevice = (CDevice*)(*itbegin);
		if(strcmp(pDevice->pstrUDN,udn) == 0)
		{
			if(bRemove)
				plistDevices->erase(itbegin);
			return (CDetailDevice *)pDevice;
		}
	}
	return NULL;
}
CDetailDevice*  CESDMController::HitDeviceBySubId(CListDevices* plistDevices,const char* subId)
{
	CListDevices::iterator itbegin = plistDevices->begin();
	CListDevices::iterator itend = plistDevices->end();

	for (itbegin; itbegin != itend; ++itbegin)
	{
		CDetailDevice *pDevice = (CDetailDevice*)(*itbegin);
		if(pDevice->isSidMatch(subId))
			return pDevice;
	}
	return NULL;
}
CDetailDevice*  CESDMController::HitDeviceBySubId(const char* subId)
{
	
	CDetailDevice *pRet = HitDeviceBySubId(&m_listServer,subId);
	if(!pRet)
		pRet = HitDeviceBySubId(&m_listRender,subId);
	if(!pRet)
		pRet = HitDeviceBySubId(&m_listPlayer,subId);
	
	return pRet;
}
CDevice* CESDMController::QueryDlnaDevice(const char* udn)
{
	IUpnpUtil::CQuickRWLock quickLock(&m_rwListLock,TRUE);
	CDevice *pDevice = NULL;
	CDevice *pReturn = NULL;
	pDevice = HitDevice(&m_listServer,udn);

	if(!pDevice)
		pDevice = HitDevice(&m_listRender,udn);
	
	if(!pDevice)
		pDevice = HitDevice(&m_listPlayer,udn);
	
	if(pDevice)
	{
		pReturn = new CDevice(*pDevice);
	}
	return pReturn;
}
int CESDMController::HandleDeviceStateVar(CEventEvent & eventNortify)
{
	int nret = 0;
	
	IUpnpUtil::CQuickRWLock quickLock(&m_rwListLock,FALSE/*TRUE*/);

	CDetailDevice *pDevice = HitDeviceBySubId((const char*)eventNortify.Sid);
	if(!pDevice)
	{
		Trace(
			"Received a notification with no subid %s matches\n",(const char*)eventNortify.Sid);
		
		return 0;
	}
	
	icstring strType = pDevice->GetServiceTypeBySubId((const char*)eventNortify.Sid);
	if(!strType)
	{
		Trace(
			"Received a notification of subid = [%s] with unknown type matches\n",
			(const char*)eventNortify.Sid);

		return 0;
	}
	m_LockContext = 1;
	if(strcmp(strType,DLNA_AVTRANSPORT) == 0)
	{
		if(m_pAVTransport)
			m_pAVTransport->NortifyObserver(pDevice,eventNortify);
		else if(m_pIDlnaAVTransport)
			m_pIDlnaAVTransport->NortifyObserver(pDevice,eventNortify);
	}
	else if(strcmp(strType,DLNA_CONNECTIONMANAGER) == 0)
	{
		if(m_pConnectionManager)
			m_pConnectionManager->NortifyObserver(pDevice,eventNortify);
		else if(m_pIConnectionManager)
			m_pIConnectionManager->NortifyObserver(pDevice,eventNortify);
	}
	else if(strcmp(strType,DLNA_RENDERINGCONTROL) == 0)
	{
		if(m_pRenderingControl)
			m_pRenderingControl->NortifyObserver(pDevice,eventNortify);
		else if(m_pIDlnaRenderintControl)
			m_pIDlnaRenderintControl->NortifyObserver(pDevice,eventNortify);
	}	
	else if(strcmp(strType,DLNA_CONTENTDIRECTORY) == 0)
	{
		if(m_pContentDirectory)
			m_pContentDirectory->NortifyObserver(pDevice,eventNortify);
		else if(m_pIDlnaContentDirtory)
			m_pIDlnaContentDirtory->NortifyObserver(pDevice,eventNortify);
	}

	m_LockContext = 0;
	return 0;

}
int CESDMController::HandleDeviceStatus(CEventDiscovery & discoveryEvent,EventType type)
{
	int nret = 0;
	IUpnpUtil::CQuickRWLock quickLock(&m_rwListLock,FALSE);
	
	CListDevices *pList = NULL;
	if(strcmp(discoveryEvent.DeviceType,DLNA_TYPE_DMSERVER) == 0)
	{
		pList = &m_listServer;
	}
	else if(strcmp(discoveryEvent.DeviceType,DLNA_TYPE_DMRENDER) == 0)
	{
		pList = &m_listRender;
	}
	else if(strcmp(discoveryEvent.DeviceType,DLNA_TYPE_DMPLAYER) == 0)
	{
		pList = &m_listPlayer;
	}
	BOOL bRemove = type == ET_DISCOVERY_ADVERTISEMENT_BYEBYE;
	if(pList)
	{
		CDetailDevice* pDeviceHitted = HitDevice(pList,discoveryEvent.DeviceId,bRemove);
		if(!bRemove)
		{
			if(!pDeviceHitted)
			{
				Trace(
					"Coming a new server from [%s] and timeout at[%d(s)]\n",
					discoveryEvent.Location,
					discoveryEvent.Expires);
				nret = HandleNewDevice(pDeviceHitted,discoveryEvent);
				Trace(
					"HandleNewDevice a new server from [%s] result = [%d]\n",discoveryEvent.Location,nret);
				if(nret == 0)
				{
					assert(pDeviceHitted);
					pList->push_back(pDeviceHitted);
					NortifyControllerObserver(pDeviceHitted);
				}
				
			}
			else
			{
				nret = UpdateDevice(pDeviceHitted,discoveryEvent);
				if(nret == 0 )
				{
					pDeviceHitted->onLine = 2;
					NortifyControllerObserver(pDeviceHitted);
					pDeviceHitted->onLine = 1;
				}
			}

			
		}
		else
		{
			if(!pDeviceHitted)
			{
				Trace("Unknown device dye: [%s]\n",discoveryEvent.DeviceId);
			}
			else
			{
				DropActiveAllInOne(pDeviceHitted);

				pDeviceHitted->onLine = FALSE;
				
				NortifyControllerObserver(pDeviceHitted);
				Trace("device dye: [%s]\n",pDeviceHitted->pstrDescDocURL);
				DELETE(pDeviceHitted);

			}
		}
		
	}

	

	return nret;
}
int CESDMController::UpdateDevice(CDetailDevice* pDetailDevice,CEventDiscovery & discoveryEvent)
{
	if(strcmp(pDetailDevice->pstrDescDocURL,discoveryEvent.Location) != 0)
	{
		pDetailDevice->ChangeLocation(discoveryEvent.Location);
		pDetailDevice->AdvrTimeOut = discoveryEvent.Expires;
		pDetailDevice->errorCode   = discoveryEvent.ErrCode;
		
		CCommandSubscribe cmdSub(FALSE);

		Service *pService = pDetailDevice->pServiceList;
		while (pService)
		{	
			if(pService->pSubId)
			{
				DropActiveAllInOne(pDetailDevice,pService->pSubId);
			
				cmdSub.TimeOut = 1800;
				cmdSub.PublisherUrl = pService->pEventURL;

				int nret = m_pIUpnp->ExecuteCommand(cmdSub);

				pService->durationTime = cmdSub.TimeOut;
				FREEARRAY(pService->pSubId);

				if(nret != ERROR_SUCCESS)
				{
					Trace(
						"Renew Subscribe to [%s] fails\n",pService->pEventURL);
				}
				else
				{
					SAFEDUP(pService->pSubId,(const char*)cmdSub.SubsId);
				}

			}
			pService = pService->pNext;
		}
		
		ResetActiveAllInOne(pDetailDevice);

		Trace(
						   "change location to =%s \n",discoveryEvent.Location);

		return 0;
	}
	else
		return -1;
}
int CESDMController::HandleNewDevice(CDetailDevice*& pDevice,CEventDiscovery & discoveryEvent)
{
	IXML_Document *DescDoc = NULL;

	int ret = 0;
	Trace("try to download from url=%s \n",discoveryEvent.Location);
	if((ret = IUpnp::DownloadXmlDoc(discoveryEvent.Location,&DescDoc)) !=UPNP_E_SUCCESS) 
	{
		Trace(
			"fail to download from url=%s \n",discoveryEvent.Location);
		
		Trace(
							"Error obtaining device description:[s]\n",IUpnpUtil::GetErrorMessage(ret));

		return -1;
	} 
	else 
	{

		pDevice = new CDetailDevice;
		
		ParseDevice(DescDoc,discoveryEvent,pDevice);

		IXML_NodeList* pRootServiceList = IUpnpUtil::GetFirstServiceList(DescDoc);

		unsigned long nLength = ixmlNodeList_length(pRootServiceList);

		IXML_Element *serviceXMLNode = NULL;


		Service* pIter = NULL;
		for (unsigned long i = 0; i < nLength; i++)
		{
			Service* pDlnaService = new Service;

			if(!pDevice->pServiceList)
			{
				pDevice->pServiceList = pDlnaService;
				pIter = pDlnaService;
			}	
			else
			{
				pIter->pNext = pDlnaService;				
				pIter = pDlnaService;
			}
			serviceXMLNode = (IXML_Element*)ixmlNodeList_item(pRootServiceList, i);

			ParseService(serviceXMLNode,discoveryEvent.Location,pDlnaService);
		}

		ixmlNodeList_free(pRootServiceList);
	}

	if( DescDoc )
		ixmlDocument_free(DescDoc);
		
		return 0;
}
int CESDMController::ParseDevice(IXML_Document *DescDoc,CEventDiscovery & discoveryEvent,CDetailDevice* pDevice)
{
	char* location =  discoveryEvent.Location;

	char* pudn = IUpnpUtil::GetFirstDocumentItem( DescDoc, "UDN" );

	if(!pudn)
	{
		Trace(
			"NO UDN? failed to handle new device\n");
		return -1;
	}



	pDevice->pstrUDN = pudn;
	pDevice->AdvrTimeOut = discoveryEvent.Expires;
	pDevice->errorCode = discoveryEvent.ErrCode;


	pDevice->pstrFriendlyName =IUpnpUtil::GetFirstDocumentItem( DescDoc, "friendlyName" );
	pDevice->pstrDeviceType = IUpnpUtil::GetFirstDocumentItem( DescDoc, "deviceType" );
	pDevice->pstrManufacturer = IUpnpUtil::GetFirstDocumentItem( DescDoc, "manufacturer" );

	pDevice->pstrRelativePresUrl = IUpnpUtil::GetFirstDocumentItem( DescDoc, "presentationURL");
	if(pDevice->pstrRelativePresUrl)
	{
		IUpnpUtil::ResolveURL(location,pDevice->pstrRelativePresUrl,&pDevice->pstrPresURL);
	}

	pDevice->pstrDescDocURL = strdup(location);
	pDevice->parseToBaseUrl(location);

	return 0;
}

int CESDMController::ParseService(IXML_Element* serviceXMLNode,const char* location,Service* pDlnaService)
{
	pDlnaService->pServiceType = IUpnpUtil::GetFirstElementItem((IXML_Element*)serviceXMLNode,"serviceType");
	pDlnaService->pServiceId = IUpnpUtil::GetFirstElementItem((IXML_Element*)serviceXMLNode,"serviceId");

	char* pcontrlUrl   = IUpnpUtil::GetFirstElementItem((IXML_Element*)serviceXMLNode,"controlURL");
	if(pcontrlUrl)
	{
		IUpnpUtil::ResolveURL(location,pcontrlUrl,&pDlnaService->pControlURL);
		pDlnaService->pRelativeControlUrl = pcontrlUrl;
		Trace("control url %s\n",pcontrlUrl);
	}

	char* peventSubUrl = IUpnpUtil::GetFirstElementItem((IXML_Element*)serviceXMLNode, "eventSubURL");
	if(peventSubUrl)
	{
		IUpnpUtil::ResolveURL(location,peventSubUrl,&pDlnaService->pEventURL);
		pDlnaService->pRelativeEventUrl = peventSubUrl;
		Trace("eventSubURL url %s\n",peventSubUrl);		
	}

	char* pscpdUrl = IUpnpUtil::GetFirstElementItem((IXML_Element*)serviceXMLNode, "SCPDURL");
	if(pscpdUrl)
	{
		IUpnpUtil::ResolveURL(location,pscpdUrl,&pDlnaService->pSCPDURL);
		pDlnaService->pRelativeSCPDUrl = pscpdUrl;
	}

	return 0;
}

CESDMController::CESConnetionManager::CESConnetionManager(CESDMController* pController)
	:m_pController(pController),
	 m_pActiveDevice(NULL),
	 m_controlUrl(NULL),
	 m_eventUrl(NULL),
	 m_sid(NULL),
	 m_Observer(NULL)
{
}
CESDMController::CESConnetionManager::~CESConnetionManager()
{

}
istring CESDMController::CESConnetionManager::GetStatusValue(const char* ValName)
{
	if(!HitStateValTable(ValName,CMServiceTable,VAR_TABLE_LENGTH(CMServiceTable)))
	{
		NoServiceStateValErrorPrint(ValName);
		return NULL;
	}

	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint("GetStatusValue");
		return NULL;
	}

	CCommandGetServiceVarStatus cmdGetValue(FALSE);
	cmdGetValue.VarName = ValName;
	cmdGetValue.ActionURL = m_controlUrl;

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdGetValue);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail get status value of [%s]--[%s]\n",ValName,IUpnpUtil::GetErrorMessage(nret));
		return NULL;
	}

	return cmdGetValue.StVarVal;
}
int CESDMController::CESConnetionManager::innerSubscribe(BOOL bSub, int Timeout)
{
	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(bSub?"Subscribe":"UnSubscribe");
		return ERROR_INVALID_URL;
	}
	if(m_sid)
		return ERROR_SUCCESS;
	ERROR_TYPE nret = ERROR_SUCCESS;
	if(bSub)
	{
		CCommandSubscribe cmdSub(FALSE);
		cmdSub.PublisherUrl = m_eventUrl;
		cmdSub.TimeOut = Timeout;

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail subscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

			return nret;
		}

		m_sid = 
			m_pActiveDevice->DupSubId(m_eventUrl,(const char*)cmdSub.SubsId,Timeout);

	}
	else
	{
		if(!m_sid)
		{
			Trace(
				"Never subscribe to [%s] before\n",m_eventUrl);

			return ERROR_INVALID_SID;
		}

		CCommandUnSubscribe cmdUnsub(FALSE);
		strncpy(cmdUnsub.SubsId,m_sid,44);

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdUnsub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail UnSubscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

		}

		m_pActiveDevice->FreeSubId(m_eventUrl);
		m_sid = NULL;


	}

	return ERROR_SUCCESS;
}
int CESDMController::CESConnetionManager::Subscribe(BOOL bSub, int Timeout)
{
	
	if(!m_pController->m_LockContext)
	{
		CONTROLLER_QWRITELOCK(m_pController);
		return innerSubscribe(bSub,Timeout);
	}
	else
		return innerSubscribe(bSub,Timeout);
}
void CESDMController::CESConnetionManager::ResetActive(CDetailDevice* pDevice)
{
	if(m_pActiveDevice)
		return;
	SetActive(pDevice->pstrUDN);
}
int CESDMController::CESConnetionManager::SetActive(const char* udn)
{
	if(m_pController->m_LockContext)
	{
		m_pActiveDevice = m_pController->HitDevice(udn);
		if(!m_pActiveDevice)
			return -1;
		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_CONNECTIONMANAGER);
		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_CONNECTIONMANAGER);

		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_CONNECTIONMANAGER);
		return 0;
	}
	else
	{
		CONTROLLER_QWRITELOCK(m_pController);
		m_pActiveDevice = m_pController->HitDevice(udn);
		if(!m_pActiveDevice)
			return -1;
		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_CONNECTIONMANAGER);
		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_CONNECTIONMANAGER);
		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_CONNECTIONMANAGER);
		return 0;
	}

	

}
int CESDMController::CESConnetionManager::GetProtocalInfo(istring & source,istring &sink)
{
	
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETPROTOCALINFO);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_CONNECTIONMANAGER;

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETPROTOCALINFO,DLNA_CONNECTIONMANAGER,0,NULL);

	if(cmdSendAction.Action == NULL)
	{
		Trace("fail to make action for [%s] at [%s]\n",ACTION_GETPROTOCALINFO,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace("fail to [%s] : [%s]\n",ACTION_GETPROTOCALINFO,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{

		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Source");
		if(pValue)
		{
			SAFEDUP(source,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Sink");
		if(pValue)
		{
			SAFEDUP(sink,pValue);
		}
	}
	return (int)nret;
	
	
	return 0;
}
int CESDMController::CESConnetionManager::GetCurrentConnectionIDs(istring & connectionids)
{
	
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETCONNECTIONIDS);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_CONNECTIONMANAGER;

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETCONNECTIONIDS,DLNA_CONNECTIONMANAGER,0,NULL);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETCONNECTIONIDS,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETCONNECTIONIDS,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{

		const char* pValue =IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"ConnectionIDs");
		if(pValue)
		{
			SAFEDUP(connectionids,pValue);
		}
	}
	return (int)nret;
	
	
	
	
	
	return 0;
}
int CESDMController::CESConnetionManager::GetCurrentConnectionInfo(ui4 connectionid,ui4& rcsid,ui4&avtransportid,
										istring& protocolinfo,istring& peerconnectionmanager,
										istring& peerconnectionid,istring&direction,
										istring&status)
{
	
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETCONNECTIONINFO);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_CONNECTIONMANAGER;
	
	char id[32] = {'\0'};
	snprintf(id,32,"%d",connectionid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETCONNECTIONINFO,DLNA_CONNECTIONMANAGER,1,"ConnectionID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETCONNECTIONINFO,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETCONNECTIONINFO,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{

		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RcsID");
		if(pValue)
		{
			rcsid = atoi(pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"AVTransportID");
		if(pValue)
		{
			avtransportid = atoi(pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"ProtocolInfo");
		if(pValue)
		{
			SAFEDUP(protocolinfo,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"PeerConnectionManager");
		if(pValue)
		{
			SAFEDUP(peerconnectionmanager,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"PeerConnectionID");
		if(pValue)
		{
			SAFEDUP(peerconnectionid,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Direction");
		if(pValue)
		{
			SAFEDUP(direction,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Status");
		if(pValue)
		{
			SAFEDUP(status,pValue);
		}
	}
	return (int)nret;
	
	
	
	return 0;
}


CESDMController::CESAVTransport::CESAVTransport(CESDMController* pController)
:m_pController(pController),
 m_pActiveDevice(NULL),
 m_controlUrl(NULL),
 m_eventUrl(NULL),
 m_sid(NULL),
 m_Observer(NULL)
{

}
CESDMController::CESAVTransport::~CESAVTransport()
{

}
istring CESDMController::CESAVTransport::GetStatusValue(const char* ValName)
{
	if(!HitStateValTable(ValName,AVTServiceTable,VAR_TABLE_LENGTH(AVTServiceTable)))
	{
		NoServiceStateValErrorPrint(ValName);
		return NULL;
	}
	
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint("GetStatusValue");
		return NULL;
	}
	
	CCommandGetServiceVarStatus cmdGetValue(FALSE);
	cmdGetValue.VarName = ValName;
	cmdGetValue.ActionURL = m_controlUrl;

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdGetValue);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail get status value of [%s]--[%s]\n",ValName,IUpnpUtil::GetErrorMessage(nret));
		return NULL;
	}
	
	return cmdGetValue.StVarVal;

}
int CESDMController::CESAVTransport::innerSubscribe(BOOL bSub, int Timeout)
{
	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(bSub?"Subscribe":"UnSubscribe");
		return ERROR_INVALID_URL;
	}
	if(m_sid)
		return ERROR_SUCCESS;
	ERROR_TYPE nret = ERROR_SUCCESS;
	if(bSub)
	{
		CCommandSubscribe cmdSub(FALSE);
		cmdSub.PublisherUrl = m_eventUrl;
		cmdSub.TimeOut = Timeout;

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail subscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

			return nret;
		}

		m_sid = 
			m_pActiveDevice->DupSubId(m_eventUrl,(const char*)cmdSub.SubsId,Timeout);

	}
	else
	{
		if(!m_sid)
		{
			Trace(
				"Never subscribe to [%s] before\n",m_eventUrl);

			return ERROR_INVALID_SID;
		}

		CCommandUnSubscribe cmdUnsub(FALSE);
		strncpy(cmdUnsub.SubsId,m_sid,44);

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdUnsub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail UnSubscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

		}

		m_pActiveDevice->FreeSubId(m_eventUrl);
		m_sid = NULL;


	}

	return ERROR_SUCCESS;
}
int CESDMController::CESAVTransport::Subscribe(BOOL bSub, int Timeout)
{
	
	if(!m_pController->m_LockContext)
	{
		CONTROLLER_QWRITELOCK(m_pController);
		return innerSubscribe(bSub,Timeout);
	}
	else
		return innerSubscribe(bSub,Timeout);
	

	
}
void CESDMController::CESAVTransport::ResetActive(CDetailDevice* pDevice)
{
	if(m_pActiveDevice)
		return;
	SetActive(pDevice->pstrUDN);
}
int CESDMController::CESAVTransport::SetActive(const char* udn)
{
	if(m_pController->m_LockContext)
	{
		m_pActiveDevice = m_pController->HitDevice(udn);
		if(!m_pActiveDevice)
			return -1;
		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_AVTRANSPORT);
		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_AVTRANSPORT);
		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_AVTRANSPORT);
	}
	else
	{
		CONTROLLER_QWRITELOCK(m_pController);
		m_pActiveDevice = m_pController->HitDevice(udn);

		if(!m_pActiveDevice)
			return -1;
		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_AVTRANSPORT);
		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_AVTRANSPORT);
		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_AVTRANSPORT);
	}

	
	return 0;
}
int CESDMController::CESAVTransport::SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_SETAVTRANSURI);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;
	
	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_SETAVTRANSURI,DLNA_AVTRANSPORT,
							3,"InstanceID",id,"CurrentURI",uri,"CurrentURIMetaData",urimetadata);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_SETAVTRANSURI,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}
	
	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_SETAVTRANSURI,IUpnpUtil::GetErrorMessage(nret));
	}
	return (int)nret;
}
int CESDMController::CESAVTransport::GetMediaInfo(ui4 instanceid,ui4& nrtracks,istring& mediaduration,istring& currenturi,
						istring& currenturimetadata,istring &nexturi,istring& nexturimetadata,
						istring& playmedium,istring& recordmedium,istring& writestatus)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETMEDIAINFO);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETMEDIAINFO,DLNA_AVTRANSPORT,
								1,"InstanceID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETMEDIAINFO,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETMEDIAINFO,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{

		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"NrTracks");
		if(pValue)
		{
			nrtracks = atoi(pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"MediaDuration");
		if(pValue)
		{
			SAFEDUP(mediaduration,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"CurrentURI");
		if(pValue)
		{
			SAFEDUP(currenturi,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"CurrentURIMetaData");
		if(pValue)
		{
			SAFEDUP(currenturimetadata,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"NextURI");
		if(pValue)
		{
			SAFEDUP(nexturi,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"NextURIMetaData");
		if(pValue)
		{
			SAFEDUP(nexturimetadata,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"PlayMedium");
		if(pValue)
		{
			SAFEDUP(playmedium,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RecordMedium");
		if(pValue)
		{
			SAFEDUP(recordmedium,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"WriteStatus");
		if(pValue)
		{
			SAFEDUP(writestatus,pValue);
		}
	
	}
	return (int)nret;
}
int CESDMController::CESAVTransport::GetTransportInfo(ui4 instanceid,istring& currenttransportstate,istring& currenttransportstatus,
						istring& currentspeed)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETRANSINFO);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETRANSINFO,DLNA_AVTRANSPORT,1,"InstanceID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETRANSINFO,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETRANSINFO,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{

		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"CurrentTransportState");
		if(pValue)
		{
			SAFEDUP(currenttransportstate,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"CurrentTransportStatus");
		if(pValue)
		{
			SAFEDUP(currenttransportstatus,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"CurrentSpeed");
		if(pValue)
		{
			SAFEDUP(currentspeed,pValue);
		}
	}

	return nret;
}
int CESDMController::CESAVTransport::GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
						istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETPOSINFO);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETPOSINFO,DLNA_AVTRANSPORT,1,"InstanceID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETPOSINFO,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETPOSINFO,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Track");
		if(pValue)
		{
			track = atoi(pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"TrackDuration");
		if(pValue)
		{
			SAFEDUP(trackduration,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"TrackMetaData");
		if(pValue)
		{
			SAFEDUP(trackmetadata,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"TrackURI");
		if(pValue)
		{
			SAFEDUP(trackuri,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RelTime");
		if(pValue)
		{
			SAFEDUP(reltime,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"AbsTime");
		if(pValue)
		{
			SAFEDUP(abstime,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RelCount");
		if(pValue)
		{
			relcount = atoi(pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"AbsCount");
		if(pValue)
		{
			abscount = atoi(pValue);
		}
	}

	return nret;
}
int CESDMController::CESAVTransport::GetDeviceCapabilities(ui4 instanceid,istring& playmedia,
														   istring& recmedia,istring& recqualitymodes)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETDEVICECAPS);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETDEVICECAPS,DLNA_AVTRANSPORT,1,"InstanceID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETDEVICECAPS,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETDEVICECAPS,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"PlayMedia");
		if(pValue)
		{
			SAFEDUP(playmedia,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RecMedia");
		if(pValue)
		{
			SAFEDUP(recmedia,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RecQualityModes");
		if(pValue)
		{
			SAFEDUP(recqualitymodes,pValue);
		}
	}

	return nret;

}
int CESDMController::CESAVTransport::GetTransportSettings(ui4 instanceid,istring& playmode,istring& recqualitymode)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETTRANSSETTINGS);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETTRANSSETTINGS,DLNA_AVTRANSPORT,1,"InstanceID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETTRANSSETTINGS,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETTRANSSETTINGS,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"PlayMode");
		if(pValue)
		{
			SAFEDUP(playmode,pValue);
		}
		pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RecQualityMode");
		if(pValue)
		{
			SAFEDUP(recqualitymode,pValue);
		}
	}

	return nret;
}
int   CESDMController::CESAVTransport::PlayAction(const char* pActionName,ui4 instanceid)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(pActionName);
		return ERROR_INVALID_DEVICE;
	}
	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(pActionName,DLNA_AVTRANSPORT,1,"InstanceID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",pActionName,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",pActionName,IUpnpUtil::GetErrorMessage(nret));
	}
	return nret;

}
int CESDMController::CESAVTransport::Stop(ui4 instanceid)
{
	return PlayAction(ACTION_STOP,instanceid);
}

int CESDMController::CESAVTransport::Play(ui4 instanceid,icstring speed)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_PLAY);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_PLAY,DLNA_AVTRANSPORT,2,"InstanceID",id,"Speed",speed);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_PLAY,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_PLAY,IUpnpUtil::GetErrorMessage(nret));
	}
	return nret;
}
int CESDMController::CESAVTransport::Seek(ui4 instanceid,icstring unit,icstring target)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_SEEK);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_SEEK,DLNA_AVTRANSPORT,3,"InstanceID",id,"Unit",unit,"Target",target);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_SEEK,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_SEEK,IUpnpUtil::GetErrorMessage(nret));
	}
	return nret;
}
int CESDMController::CESAVTransport::Next(ui4 instanceid)
{
	return PlayAction(ACTION_NEXT,instanceid);
}

int CESDMController::CESAVTransport::Previous(ui4 instanceid)
{
	return PlayAction(ACTION_PREVIOUS,instanceid);
}


CESDMController::CESContentDirectory::CESContentDirectory(CESDMController * pController)
:m_pController(pController),
 m_pActiveDevice(NULL),
 m_controlUrl(NULL),
 m_eventUrl(NULL),
 m_sid(NULL),
 m_Observer(NULL)
{
}
CESDMController::CESContentDirectory::~CESContentDirectory()
{

}
istring CESDMController::CESContentDirectory::GetStatusValue(const char* ValName)
{
	if(!HitStateValTable(ValName,CDServiceTable,VAR_TABLE_LENGTH(CDServiceTable)))
	{
		NoServiceStateValErrorPrint(ValName);
		return NULL;
	}

	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint("GetStatusValue");
		return NULL;
	}

	CCommandGetServiceVarStatus cmdGetValue(FALSE);
	cmdGetValue.VarName = ValName;
	cmdGetValue.ActionURL = m_controlUrl;

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdGetValue);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail get status value of [%s]--[%s]\n",ValName,IUpnpUtil::GetErrorMessage(nret));
		return NULL;
	}

	return cmdGetValue.StVarVal;
}
int CESDMController::CESContentDirectory::innerSubscribe(BOOL bSub, int Timeout)
{
	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(bSub?"Subscribe":"UnSubscribe");
		return ERROR_INVALID_URL;
	}
	if(m_sid)
		return ERROR_SUCCESS;
	ERROR_TYPE nret = ERROR_SUCCESS;
	if(bSub)
	{
		CCommandSubscribe cmdSub(FALSE);
		cmdSub.PublisherUrl = m_eventUrl;
		cmdSub.TimeOut = Timeout;

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail subscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

			return nret;
		}

		m_sid = 
			m_pActiveDevice->DupSubId(m_eventUrl,(const char*)cmdSub.SubsId,Timeout);

	}
	else
	{
		if(!m_sid)
		{
			Trace(
				"Never subscribe to [%s] before\n",m_eventUrl);

			return ERROR_INVALID_SID;
		}

		CCommandUnSubscribe cmdUnsub(FALSE);
		strncpy(cmdUnsub.SubsId,m_sid,44);

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdUnsub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail UnSubscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

		}

		m_pActiveDevice->FreeSubId(m_eventUrl);
		m_sid = NULL;


	}

	return ERROR_SUCCESS;
}
int CESDMController::CESContentDirectory::Subscribe(BOOL bSub,int Timeout)
{
	if(!m_pController->m_LockContext)
	{
		CONTROLLER_QWRITELOCK(m_pController);
		return innerSubscribe(bSub,Timeout);
	}
	else
		return innerSubscribe(bSub,Timeout);
}
void CESDMController::CESContentDirectory::ResetActive(CDetailDevice* pDevice)
{
	if(m_pActiveDevice)
		return;
	SetActive(pDevice->pstrUDN);
}
int CESDMController::CESContentDirectory::SetActive(const char* udn)
{
	if(m_pController->m_LockContext)
	{
		m_pActiveDevice = m_pController->HitDevice(udn);
		if(!m_pActiveDevice)
			return -1;

		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_CONTENTDIRECTORY);

		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_CONTENTDIRECTORY);
		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_CONTENTDIRECTORY);
	}
	else
	{
		CONTROLLER_QWRITELOCK(m_pController);
		m_pActiveDevice = m_pController->HitDevice(udn);
		if(!m_pActiveDevice)
			return -1;

		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_CONTENTDIRECTORY);

		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_CONTENTDIRECTORY);
		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_CONTENTDIRECTORY);
	}
	
	

	return 0;
}
int CESDMController::CESContentDirectory::GetSearchCapabilities(istring & searchcaps)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETSEARCHCAP);
		return ERROR_INVALID_DEVICE;
	}
	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_CONTENTDIRECTORY;

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETSEARCHCAP,DLNA_CONTENTDIRECTORY,0,NULL);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETSEARCHCAP,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}
	
	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETSEARCHCAP,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		
		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"SearchCaps");
		if(!pValue)
		{
			nret = ERROR_EXT_NOT_XML;
		}
		else
		{
			SAFEDUP(searchcaps,pValue);
		}
	}
	return (int)nret;
}

int CESDMController::CESContentDirectory::GetSortCapabilities(istring & sortcaps)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETSORTCAP);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
			return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_CONTENTDIRECTORY;

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETSORTCAP,DLNA_CONTENTDIRECTORY,0,NULL);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETSORTCAP,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETSORTCAP,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		icstring pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"SortCaps");
		if(!pValue)
		{
			nret = ERROR_EXT_NOT_XML;
		}
		else
		{
			SAFEDUP(sortcaps,pValue);

		}
	}
	return (int)nret;
}

int CESDMController::CESContentDirectory::GetSystemUpdateID(ui4 & id)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_GETSYSTEMUPDATEID);
		return ERROR_INVALID_DEVICE;
	}
	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_CONTENTDIRECTORY;
	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_GETSYSTEMUPDATEID,DLNA_CONTENTDIRECTORY,0,NULL);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_GETSYSTEMUPDATEID,
			 cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_GETSYSTEMUPDATEID,
			 IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		icstring pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Id");
		if(!pValue)
		{
			nret = ERROR_EXT_NOT_XML; 
		}
		else
		{
			id = atoi(pValue);
		}
	}
	
	
	return 0;
}

int CESDMController::CESContentDirectory::Browse(icstring id,icstring browseflag,icstring filter,
				   ui4 startingindex,ui4 requestedcount,icstring sort,
				   istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_BROWSE);
		return ERROR_INVALID_DEVICE;
	}

	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;
	
	char index[9]={'\0'},count[9]={'\0'};
	snprintf(index,9,"%d",startingindex);
	snprintf(count,9,"%d",requestedcount);		

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_CONTENTDIRECTORY;
	cmdSendAction.Action = 
		IUpnpUtil::MakeAction("Browse",DLNA_CONTENTDIRECTORY,
					6,"ObjectID",id,"BrowseFlag",browseflag,
					"Filter",filter,"StartingIndex",index,"RequestedCount",
					 count,"SortCriteria",sort);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_BROWSE,
			cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_BROWSE,
			IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		icstring pValueResult = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Result");
		if(pValueResult)
		{
			SAFEDUP(result,pValueResult);
		}
		
		pValueResult = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"NumberReturned");
		if(pValueResult)
		{
			numberreturned = atoi(pValueResult);
		}
		
		pValueResult = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"TotalMatches");
		if(pValueResult)
		{
			totalmatches = atoi(pValueResult);
		}
		pValueResult = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"UpdateID");
		if(pValueResult)
		{
			updateid = atoi(pValueResult);
		}
		


	}
	
	return 0;
}

CESDMController::CESRenderingControl::CESRenderingControl(CESDMController * pController)
:m_pController(pController),
m_pActiveDevice(NULL),
m_controlUrl(NULL),
m_eventUrl(NULL),
m_sid(NULL),
m_Observer(NULL)
{

}
CESDMController::CESRenderingControl::~CESRenderingControl()
{
	
}
istring CESDMController::CESRenderingControl::GetStatusValue(const char* ValName)
{
	if(!HitStateValTable(ValName,RCServiceTable,VAR_TABLE_LENGTH(RCServiceTable)))
	{
		NoServiceStateValErrorPrint(ValName);
		return NULL;
	}

	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint("GetStatusValue");
		return NULL;
	}


	CCommandGetServiceVarStatus cmdGetValue(FALSE);
	cmdGetValue.VarName = ValName;
	cmdGetValue.ActionURL = m_controlUrl;

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdGetValue);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail get status value of [%s]--[%s]\n",ValName,IUpnpUtil::GetErrorMessage(nret));
		return NULL;
	}

	return cmdGetValue.StVarVal;
}
int CESDMController::CESRenderingControl::innerSubscribe(BOOL bSub, int Timeout)
{
	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(bSub?"Subscribe":"UnSubscribe");
		return ERROR_INVALID_URL;
	}
	if(m_sid)
		return ERROR_SUCCESS;
	ERROR_TYPE nret = ERROR_SUCCESS;
	if(bSub)
	{
		CCommandSubscribe cmdSub(FALSE);
		cmdSub.PublisherUrl = m_eventUrl;
		cmdSub.TimeOut = Timeout;

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail subscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

			return nret;
		}

		m_sid = 
			m_pActiveDevice->DupSubId(m_eventUrl,(const char*)cmdSub.SubsId,Timeout);

	}
	else
	{
		if(!m_sid)
		{
			Trace(
				"Never subscribe to [%s] before\n",m_eventUrl);

			return ERROR_INVALID_SID;
		}

		CCommandUnSubscribe cmdUnsub(FALSE);
		strncpy(cmdUnsub.SubsId,m_sid,44);

		nret = GETIUPNP(m_pController)->ExecuteCommand(cmdUnsub);
		if(nret != ERROR_SUCCESS)
		{
			Trace(
				"fail UnSubscribe to [%s]--[%s]\n",m_eventUrl,IUpnpUtil::GetErrorMessage(nret));

		}

		m_pActiveDevice->FreeSubId(m_eventUrl);
		m_sid = NULL;


	}

	return ERROR_SUCCESS;
}
void CESDMController::CESRenderingControl::ResetActive(CDetailDevice* pDevice)
{
	if(m_pActiveDevice)
		return;
	SetActive(pDevice->pstrUDN);
}
int CESDMController::CESRenderingControl::Subscribe(BOOL bSub,int Timeout)
{
	if(!m_pController->m_LockContext)
	{
		CONTROLLER_QWRITELOCK(m_pController);
		return innerSubscribe(bSub,Timeout);
	}
	else
		return innerSubscribe(bSub,Timeout);
}
int CESDMController::CESRenderingControl::SetActive(const char* udn)
{
	if(m_pController->m_LockContext)
	{
		m_pActiveDevice = m_pController->HitDevice(udn);
		if(!m_pActiveDevice)
			return -1;

		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_RENDERINGCONTROL);
		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_RENDERINGCONTROL);
		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_RENDERINGCONTROL);
	}
	else
	{
		CONTROLLER_QWRITELOCK(m_pController);
		m_pActiveDevice = m_pController->HitDevice(udn);

		if(!m_pActiveDevice)
			return -1;

		m_controlUrl = 
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_CONTROL,DLNA_RENDERINGCONTROL);
		m_eventUrl =  
			m_pActiveDevice->pServiceList->getUrlByServiceType(URL_TYPE_EVENT,DLNA_RENDERINGCONTROL);
		m_sid = m_pActiveDevice->pServiceList->GetSubId(DLNA_RENDERINGCONTROL);
	}

	

	return 0;
}
int CESDMController::CESRenderingControl::ListPresets(ui4 instanceid,istring& currentpresetnamelist)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_LISTPRESETS);
		return ERROR_INVALID_DEVICE;
	}
	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_RENDERINGCONTROL;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_LISTPRESETS,DLNA_RENDERINGCONTROL,1,"InstanceID",id);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_LISTPRESETS,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_LISTPRESETS,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		const char* pValue = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"CurrentPresetNameList");
		SAFEDUP(currentpresetnamelist,pValue);
	}

	return nret;
}
int CESDMController::CESRenderingControl::SelectPreset(ui4 instanceid,icstring presetname)
{
	CONTROLLER_QREADLOCK(m_pController);

	if(!m_pActiveDevice)
	{
		NoActiveErrorPrint(ACTION_SELECTPRESETS);
		return ERROR_INVALID_DEVICE;
	}
	if(m_controlUrl == NULL)
		return ERROR_INVALID_SERVICE;

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = m_controlUrl;

	cmdSendAction.ServiceType = DLNA_RENDERINGCONTROL;

	char id[32] = {'\0'};
	snprintf(id,32,"%d",instanceid);

	cmdSendAction.Action = 
		IUpnpUtil::MakeAction(ACTION_SELECTPRESETS,DLNA_RENDERINGCONTROL,2,"InstanceID",id,"PresetName",presetname);

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_SELECTPRESETS,cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = GETIUPNP(m_pController)->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_SELECTPRESETS,IUpnpUtil::GetErrorMessage(nret));
	}
	return nret;
}

#define LOG_TAG "easydlna-upnpcontrol"
#include "CUpnpControl.h"


CUpnpControl::CUpnpControl()
	:m_nUpnpHandle(-1),
	m_registerCount(0),
	m_TickCountThreadId(-1),
	m_pThreadPool(NULL),
	m_bQuitTickCountThread(FALSE)
{
	pthread_cond_init(&m_quitCond,NULL);
	pthread_mutex_init(&m_quitMutex,NULL);
}
CUpnpControl::~CUpnpControl()
{
	if(m_nUpnpHandle != -1)
		UnRegister();
	Stop();

	pthread_cond_destroy(&m_quitCond);
	pthread_mutex_destroy(&m_quitMutex);
}
void CUpnpControl::Stop()
{
	LOCKWRITE(&m_rwLock);
	
	CListIUpnpObserver::iterator iterbegin = m_ListObserver.begin();
	CListIUpnpObserver::iterator iterend = m_ListObserver.end();
	for (iterbegin; iterbegin != iterend; ++iterbegin)
	{
		IUpnpControlPointObserver* pObserver = (IUpnpControlPointObserver*)(*iterbegin);
		delete pObserver;
	}

	m_ListObserver.clear();
	
	UNLOCKWRITE(&m_rwLock);

}

ERROR_TYPE CUpnpControl::Register(const CRegisterType & cType)
{
	if(cType.nDeclareType != UPNP_CLIENT)
		return ERROR_INVALID_PARAM;
	
	int nret = 0;

	if(++m_registerCount == 1)
	{
		nret = UpnpRegisterClient((Upnp_FunPtr)CUpnpControl::ClientCallBack,this,&m_nUpnpHandle);
		
		m_pThreadPool = new ThreadPool;

		ThreadPoolAttr attr;
		TPAttrInit(&attr);
		TPAttrSetMaxThreads(&attr, 2);

		int nret = ThreadPoolInit(m_pThreadPool,&attr);
		
		ThreadPoolJob job;

		nret = TPJobInit(&job,CUpnpControl::ThreadTickCountGenerator,this);
		
		nret = ThreadPoolAdd(m_pThreadPool,&job,&m_TickCountThreadId);
	}

	return (ERROR_TYPE)nret;

}
ERROR_TYPE CUpnpControl::UnRegister()
{
	int nret = 0;
	if(--m_registerCount == 0)
	{
		nret = UpnpUnRegisterClient(m_nUpnpHandle);
		m_nUpnpHandle = -1;

		pthread_mutex_lock(&m_quitMutex);
		m_bQuitTickCountThread = TRUE;
		pthread_cond_broadcast(&m_quitCond);
		pthread_mutex_unlock(&m_quitMutex);

		ThreadPoolRemove(m_pThreadPool,m_TickCountThreadId,NULL);
		ThreadPoolShutdown(m_pThreadPool);

		DELETE(m_pThreadPool);

	}

	return (ERROR_TYPE)nret;
}

ERROR_TYPE CUpnpControl::AddUpnpControlPointObserver(IUpnpControlPointObserver* IObserver)
{
	QUICKWRITELOCK(&m_rwLock);
	IObserver->SetNortifySource(this);
	m_ListObserver.push_back(IObserver);

	return ERROR_SUCCESS;
}
ERROR_TYPE CUpnpControl:: RemoveUpnpControlPointObserver(IUpnpControlPointObserver* observer)
{
	QUICKWRITELOCK(&m_rwLock);
	CListIUpnpObserver::iterator iterbegin = m_ListObserver.begin();
	CListIUpnpObserver::iterator iterend = m_ListObserver.end();
	for (iterbegin; iterbegin != iterend; ++iterbegin)
	{
		if((*iterbegin) == observer)
		{
			m_ListObserver.erase(iterbegin);
			break;
		}
	}

	return ERROR_SUCCESS;
}
ERROR_TYPE CUpnpControl::ExecuteCommand(CCommand & command)
{
	if(m_nUpnpHandle == -1)
		return ERROR_INTERNAL_ERROR;

	COMMAND_TYPE nType = command.nType;
	int nret = 0;
	switch(nType)
	{
	case CMD_TYPE_SEARCH_ASYNC:
		{
			CCommandSearch& cmdsearch = static_cast<CCommandSearch&>(command);
			cmdsearch.Cookie = this;
			nret = UpnpSearchAsync(m_nUpnpHandle,cmdsearch.Mx,cmdsearch.Target,cmdsearch.Cookie);
		}
		break;
	case CMD_TYPE_GETSERVICESTATUS:
		{
			CCommandGetServiceVarStatus & cmdGetStatus = static_cast<CCommandGetServiceVarStatus &>(command);
			if(!cmdGetStatus.bAsync)
			{
				nret = UpnpGetServiceVarStatus(m_nUpnpHandle,cmdGetStatus.ActionURL,cmdGetStatus.VarName,
											   &cmdGetStatus.StVarVal);
			}
			else
			{
				nret = UpnpGetServiceVarStatusAsync(m_nUpnpHandle,cmdGetStatus.ActionURL,cmdGetStatus.VarName,
						(Upnp_FunPtr)CUpnpControl::ClientCallBack,this);
			}
		}
		break;
	case CMD_TYPE_SENDACTION:
		{
			CCommandSendAction & cmdSendAction = static_cast<CCommandSendAction &>(command);
			if(!cmdSendAction.bAsync)
				nret = UpnpSendAction(m_nUpnpHandle,cmdSendAction.ActionURL,cmdSendAction.ServiceType,
					cmdSendAction.DevUDN,cmdSendAction.Action,&cmdSendAction.RespNode);
			else
			{
				nret = UpnpSendActionAsync(m_nUpnpHandle,cmdSendAction.ActionURL,cmdSendAction.ServiceType,
					cmdSendAction.DevUDN,cmdSendAction.Action,(Upnp_FunPtr)CUpnpControl::ClientCallBack,this);
			}
		}
		break;
	case CMD_TYPE_SENDACTIONEX:
		{
			CCommandSendActionEx & cmdSendActionEx = static_cast<CCommandSendActionEx &>(command);
			if(!cmdSendActionEx.bAsync)
			{
				nret = UpnpSendActionEx(m_nUpnpHandle,cmdSendActionEx.ActionURL,cmdSendActionEx.ServiceType,
					cmdSendActionEx.DevUDN,cmdSendActionEx.Header,cmdSendActionEx.Action,&cmdSendActionEx.RespNode);
			}
			else
			{
				nret = UpnpSendActionExAsync(m_nUpnpHandle,cmdSendActionEx.ActionURL,cmdSendActionEx.ServiceType,
					cmdSendActionEx.DevUDN,cmdSendActionEx.Header,cmdSendActionEx.Action,
					(Upnp_FunPtr)CUpnpControl::ClientCallBack,this);
			}
		}
	    break;
	case CMD_TYPE_RENEWSUBSCRIPTION:
		{
			CCommandRenewSubscription & cmdRenewSub = static_cast<CCommandRenewSubscription &>(command);
			if(!cmdRenewSub.bAsync)
			{
				nret = UpnpRenewSubscription(m_nUpnpHandle,&cmdRenewSub.TimeOut,cmdRenewSub.SubsId);
			}
			else
			{
				nret = UpnpRenewSubscriptionAsync(m_nUpnpHandle,cmdRenewSub.TimeOut,cmdRenewSub.SubsId,
							(Upnp_FunPtr)CUpnpControl::ClientCallBack,this);
			}
		}
	    break;
	case CMD_TYPE_SUBSCRIBE:
		{
			CCommandSubscribe & cmdSubscribe = static_cast<CCommandSubscribe & >(command);
			if(!cmdSubscribe.bAsync)
			{
				nret = UpnpSubscribe(m_nUpnpHandle,cmdSubscribe.PublisherUrl,&cmdSubscribe.TimeOut,
									cmdSubscribe.SubsId);
			}
			else
			{
				nret = UpnpSubscribeAsync(m_nUpnpHandle,cmdSubscribe.PublisherUrl,cmdSubscribe.TimeOut,
										  (Upnp_FunPtr)CUpnpControl::ClientCallBack,this);
			}
		}
		break;
	case CMD_TYPE_UNSUBSCRIBE:
		{
			CCommandUnSubscribe & cmdUnSubscribe = static_cast<CCommandUnSubscribe &> (command);
			if(!cmdUnSubscribe.bAsync)
			{
				nret = UpnpUnSubscribe(m_nUpnpHandle,cmdUnSubscribe.SubsId);
			}
			else
			{
				nret = UpnpUnSubscribeAsync(m_nUpnpHandle,cmdUnSubscribe.SubsId,
											(Upnp_FunPtr)CUpnpControl::ClientCallBack,this);
			}
		}
		break;
	default:
		return IUpnp::ExecuteCommand(command);
		break;
	}
	
	
	
	return (ERROR_TYPE)nret;
}
void CUpnpControl::ClientCallBack(EventType eventType, void *Event, void *Cookie )
{
	 ((CUpnpControl*)Cookie)->innerClientCallBack(eventType,Event);
}
void CUpnpControl::innerClientCallBack(EventType eventType, void *Event)
{
	switch(eventType)
	{
	case ET_CONTROL_ACTION_COMPLETE:
		{
			CEventActionComplete cevent;
			cevent.Translate(Event);
			LoopCallObserver(eventType,cevent);
		}
		break;
	case ET_CONTROL_GET_VAR_COMPLETE:
		{
			CEventStateVarComplete cevent;
			cevent.Translate(Event);
			LoopCallObserver(eventType,cevent);
		}
		break;
	case ET_DISCOVERY_ADVERTISEMENT_ALIVE:
	case ET_DISCOVERY_ADVERTISEMENT_BYEBYE:
	case ET_DISCOVERY_SEARCH_RESULT:
		{
			CEventDiscovery cevent;
			cevent.Translate(Event);
			LoopCallObserver(eventType,cevent);
		}
	    break;
	case ET_DISCOVERY_SEARCH_TIMEOUT:
		break;
	case ET_EVENT_RECEIVED:
		{
			CEventEvent cevent;
			cevent.Translate(Event);
			LoopCallObserver(eventType,cevent);
		}
		break;
	case ET_EVENT_RENEWAL_COMPLETE:
	case ET_EVENT_SUBSCRIBE_COMPLETE:
	case ET_EVENT_UNSUBSCRIBE_COMPLETE:
	case ET_EVENT_AUTORENEWAL_FAILED:
	case ET_EVENT_SUBSCRIPTION_EXPIRED:
		{
			CEventSubscribe cevent;
			cevent.Translate(Event);
			LoopCallObserver(eventType,cevent);
		}
		break;
	default:
	    break;
	}
	
}

void CUpnpControl::LoopCallObserver(EventType eventType,CEvent & event)
{
	QUICKREADLOCK(&m_rwLock);

	CListIUpnpObserver::iterator iterbegin = m_ListObserver.begin();
	CListIUpnpObserver::iterator iterend = m_ListObserver.end();
	for (iterbegin; iterbegin != iterend; ++iterbegin)
	{
		IUpnpControlPointObserver* observer  = (IUpnpControlPointObserver*)(*iterbegin);
		if(NHR_BREAK == observer->Nortify(eventType,event))
			break;
	}
	return; 
}
void* CUpnpControl::ThreadTickCountGenerator(void *arg)
{
	return ((CUpnpControl*)arg)->TickCountGenerator();
}
void* CUpnpControl::TickCountGenerator()
{
	int incr = 10;              
	int nret = 0;
	int trigger = 0;
	CEventTickCount eventTickCount;
	eventTickCount.nTickCount = 10;

	while(TRUE)
	{
		struct timespec spec;
		pthread_mutex_lock(&m_quitMutex);

		int nret = pthread_cond_timedwait(&m_quitCond,&m_quitMutex,
					IUpnpUtil::getTimeout(&spec,incr*1000));

		pthread_mutex_unlock(&m_quitMutex);

		if(nret == 0 || m_bQuitTickCountThread)
		{
			break;
		}


		LOCKREAD(&m_rwLock);
		
		CListIUpnpObserver::iterator iterbegin = m_ListObserver.begin();
		CListIUpnpObserver::iterator iterend = m_ListObserver.end();
		
		for (iterbegin; iterbegin != iterend; ++iterbegin)
		{
			IUpnpControlPointObserver * observer = (IUpnpControlPointObserver*)(*iterbegin);
			observer->Nortify(ET_EVENT_TICKCOUNT,eventTickCount);
			//ignoring return value;
		}

		
		UNLOCKREAD(&m_rwLock);
		if(++trigger == 3)
		{
			Trace("UpnpControll::Tick count [%d(s)] check\n",incr);
			trigger = 0;
		}

	}
	Trace("Tick count thread quit\n");
	return NULL;
}


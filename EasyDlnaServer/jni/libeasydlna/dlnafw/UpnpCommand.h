#ifndef __UPNPCOMMAND_H__
#define __UPNPCOMMAND_H__

#include "IUpnpDefine.h"


enum COMMAND_TYPE
{
	CMD_TYPE_UNKNOWN	  = -1,
	CMD_TYPE_SEARCH_ASYNC = 0,
	CMD_TYPE_SENDADVERTISEMENT = 1,
	CMD_TYPE_GETSERVICESTATUS = 2,
	CMD_TYPE_SENDACTION = 3,
	CMD_TYPE_SENDACTIONEX = 4,
	CMD_TYPE_ACCEPTSUBSCRIPTION = 5,
	CMD_TYPE_ACCEPTSUBSCRIPTIONEX = 6,
	CMD_TYPE_NORTIFY = 7,
	CMD_TYPE_NORTIFYEX = 8,
	CMD_TYPE_RENEWSUBSCRIPTION = 9,
	CMD_TYPE_SETMAXSUBSCRIPTION = 10,
	CMD_TYPE_SETMAXSUBSCRIPTIONTIMEOUT = 11,
	CMD_TYPE_SUBSCRIBE = 12,
	CMD_TYPE_UNSUBSCRIBE = 13,
};

typedef char SubscriptionID[44];


class IUpnp;

class CCommand
{
public:
	CCommand()
	{

	};
	virtual ~CCommand(){};
public:
	BOOL bAsync;
	COMMAND_TYPE nType;
		
};



class CCommandSearch:public CCommand
{
public :
	CCommandSearch()
	{
		nType = CMD_TYPE_SEARCH_ASYNC;
		bAsync = TRUE;
	};

	int Mx;
	const char *Target;
	const void *Cookie;
};

class CCommandSendAdvertisement:public CCommand
{
public:
	CCommandSendAdvertisement(int Exp)
	{
		nType = CMD_TYPE_SENDADVERTISEMENT;
		this->Exp = Exp;
	}
	
	int Exp;

};

class CCommandGetServiceVarStatus:public CCommand
{
public:
	CCommandGetServiceVarStatus(BOOL bAsync)
	{
		nType = CMD_TYPE_GETSERVICESTATUS;
		this->bAsync = bAsync;

		ActionURL = VarName = NULL;
		StVarVal = NULL;
	}

	const char *ActionURL;
	const char *VarName;
	DOMString  StVarVal;

friend class IUpnp;

private:	
	EventCallBack Fun; 
	const void *Cookie;
};

class CCommandSendAction : public CCommand
{
public:
	CCommandSendAction(BOOL bAsync)
	{
		nType = CMD_TYPE_SENDACTION;
		this->bAsync = bAsync;
		ActionURL = ServiceType = DevUDN = NULL;
		Action = NULL;
		RespNode = NULL;

		Fun = NULL;
		Cookie = NULL;
	}
	virtual ~CCommandSendAction()
	{
		if(Action)
			ixmlDocument_free(Action);
		if(RespNode)
			ixmlDocument_free(RespNode);
	}
	IN const char *ActionURL; 
	IN const char *ServiceType;
	IN const char *DevUDN;
	IN IXML_Document *Action;
	OUT IXML_Document *RespNode;

friend class IUpnp;
private:
	EventCallBack Fun;
	const void *Cookie;
};

class CCommandSendActionEx : public CCommand
{
public:
	CCommandSendActionEx(BOOL bAsync)
	{
		nType = CMD_TYPE_SENDACTIONEX;
		this->bAsync = bAsync;

		ActionURL = ServiceType = DevUDN = NULL;
		Header = Action = RespNode = NULL;
	}
	virtual ~CCommandSendActionEx()
	{
		if(Action)
			ixmlDocument_free(Action);
		if(Header)
			ixmlDocument_free(Header);
		if(RespNode)
			ixmlDocument_free(RespNode);
	}
	const char *ActionURL;
	IN const char *ServiceType;
	IN const char *DevUDN;
	IN IXML_Document *Header;
	IN IXML_Document *Action;
	OUT IXML_Document *RespNode;

	friend class IUpnp;
private:
	EventCallBack Fun;
	const void *Cookie;
};

class CComandAcceptSubscription:public CCommand
{
public:
	CComandAcceptSubscription()
	{
		nType = CMD_TYPE_ACCEPTSUBSCRIPTION;

		DevID = ServID = NULL;
		VarName = NewVal = NULL;
	};
	
	const char *DevID;
	const char *ServID;
	const char **VarName;
	const char **NewVal;
	int cVariables;
	SubscriptionID SubsId; 

};
class CComandAcceptSubscriptionEx:public CCommand
{
public:
	CComandAcceptSubscriptionEx()
	{
		nType = CMD_TYPE_ACCEPTSUBSCRIPTIONEX;

		DevID = ServID = NULL;
		PropSet = NULL;
	};
	
	virtual ~CComandAcceptSubscriptionEx()
	{
		if(PropSet)
			ixmlDocument_free(PropSet);
	}
	const char *DevID;
	const char *ServID;
	IXML_Document *PropSet;
	SubscriptionID SubsId; 

};

class CComandNortify:public CCommand
{
public:
	CComandNortify()
	{
		nType = CMD_TYPE_NORTIFY;

		DevID = ServID = NULL;
		VarName = NewVal = NULL;
	};

	IN const char *DevID;
	IN const char *ServID;
	IN const char **VarName;
	IN const char **NewVal;
	IN int cVariables;  

};

class CComandNortifyEx:public CCommand
{
public:
	CComandNortifyEx()
	{
		nType = CMD_TYPE_NORTIFYEX;
		DevID = ServID = NULL;
		PropSet = NULL;
	};
	virtual ~CComandNortifyEx()
	{
		if(PropSet)
			ixmlDocument_free(PropSet);
	}
	IN const char *DevID;
	IN const char *ServID;
	IXML_Document *PropSet;

};


class CCommandRenewSubscription:public CCommand
{
public:
	CCommandRenewSubscription(BOOL bAsync)
	{
		nType = CMD_TYPE_RENEWSUBSCRIPTION;
		this->bAsync = bAsync;
	}
	int TimeOut;
	SubscriptionID SubsId;

friend class IUpnp;
private:
	EventCallBack Fun;
	const void *Cookie;


};

class CCommandSetMaxSubscription:public CCommand
{
public:
	CCommandSetMaxSubscription(int MaxSubscription)
	{
		nType = CMD_TYPE_SETMAXSUBSCRIPTION;
		this->MaxSubscriptions = MaxSubscription;
	}
	int MaxSubscriptions;

};

class CCommandSetSubscriptionTimeOut:public CCommand
{
public:
	CCommandSetSubscriptionTimeOut(int MaxTimeOut)
	{
		nType = CMD_TYPE_SETMAXSUBSCRIPTIONTIMEOUT;
		MaxSubscriptionTimeOut = MaxTimeOut;
	}
	int MaxSubscriptionTimeOut;

};

class CCommandSubscribe:public CCommand
{
public:
	CCommandSubscribe(BOOL bAsync)
	{
		nType = CMD_TYPE_SUBSCRIBE;
		this->bAsync = bAsync;
		PublisherUrl = NULL;
	}

	const char *PublisherUrl;
	int TimeOut;
	SubscriptionID SubsId;

	friend class IUpnp;
private:
	EventCallBack Fun;
	const void *Cookie; 
};

class CCommandUnSubscribe:public CCommand
{
public:
	CCommandUnSubscribe(BOOL bAsync)
	{
		nType = CMD_TYPE_UNSUBSCRIBE;
		this->bAsync = bAsync;
	}

	SubscriptionID SubsId;

	friend class IUpnp;
private:
	EventCallBack Fun;
	const void *Cookie; 
};







class CEvent
{
public:
	virtual void Translate(void* outEvent) = 0;
	virtual ~CEvent(){};
};

class CEventActionRequest:public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_Action_Request* pRealEvent = (Upnp_Action_Request*)outEvent;
		ErrCode = pRealEvent->ErrCode;
		Socket = pRealEvent->Socket;

		ErrStr = (char*)pRealEvent->ErrStr;
		ActionName = (char*)pRealEvent->ActionName;
		DevUDN = (char*)pRealEvent->DevUDN;
		ServiceID = (char*)pRealEvent->ServiceID;
		
		ActionRequest = pRealEvent->ActionRequest;
		ActionResult  = pRealEvent->ActionResult;

		memcpy(&CtrlPtIPAddr,&pRealEvent->CtrlPtIPAddr,sizeof(CtrlPtIPAddr));

		SoapHeader	  = pRealEvent->SoapHeader;

		return;
	}
	void SetResult(void* outEvent)
	{
		Upnp_Action_Request* pRealEvent = (Upnp_Action_Request*)outEvent;
		pRealEvent->ActionResult = ActionResult;
		pRealEvent->ErrCode = ErrCode;
	}
public:
	int ErrCode;
	int Socket;
	char* ErrStr;
	char* ActionName;
	char* DevUDN;
	char* ServiceID;
	IXML_Document *ActionRequest;
	IXML_Document *ActionResult;
	struct in_addr CtrlPtIPAddr;
	IXML_Document *SoapHeader;
};


class CEventActionComplete:public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_Action_Complete* pRealEvent = (Upnp_Action_Complete*)outEvent;

		ErrCode = pRealEvent->ErrCode;
		CtrlUrl = (char*)pRealEvent->CtrlUrl;
		
		ActionRequest = pRealEvent->ActionRequest;
		ActionResult  = pRealEvent->ActionResult;

		return;
	}
public:
	int ErrCode;

	char *CtrlUrl ;

	IXML_Document *ActionRequest;

	IXML_Document *ActionResult;

};

class CEventStateVarRequest:public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_State_Var_Request * pRealEvent = (Upnp_State_Var_Request*)outEvent;
		ErrCode = pRealEvent->ErrCode;
		Socket  = pRealEvent->Socket;

		ErrStr  = (char*)pRealEvent->ErrStr;
		DevUDN = (char*)pRealEvent->DevUDN;
		ServiceID = (char*)pRealEvent->ServiceID;
		StateVarName = (char*)pRealEvent->StateVarName;
		
		memcpy(&CtrlPtIPAddr,&pRealEvent->CtrlPtIPAddr,sizeof(CtrlPtIPAddr));

		CurrentVal = pRealEvent->CurrentVal;

	}
	void SetResult(void* outEvent)
	{
		Upnp_State_Var_Request* pRealEvent = (Upnp_State_Var_Request*)outEvent;
		pRealEvent->ErrCode = ErrCode;
		pRealEvent->CurrentVal = CurrentVal;
	}
public:
	int ErrCode;
	int Socket;

	char *ErrStr;

	char *DevUDN;

	char *ServiceID;

	char *StateVarName;

	struct in_addr CtrlPtIPAddr;

	DOMString CurrentVal;
};

class CEventStateVarComplete:public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_State_Var_Complete *pRealEvent = (Upnp_State_Var_Complete*)outEvent;

		ErrCode = pRealEvent->ErrCode;
		CtrlUrl = (char*)pRealEvent->CtrlUrl;
		StateVarName = (char*)pRealEvent->StateVarName;

		CurrentVal = pRealEvent->CurrentVal;
	}
public:
	int ErrCode;

	char *CtrlUrl;

	char *StateVarName;

	DOMString CurrentVal;
};

class CEventEvent :public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_Event *pRealEvent = (Upnp_Event*)outEvent;
		
		Sid = (char*)pRealEvent->Sid;
		EventKey = pRealEvent->EventKey;
		ChangedVariables = pRealEvent->ChangedVariables;
	}

public:	
	char* Sid;

	int EventKey;

	IXML_Document *ChangedVariables;

};

class CEventDiscovery:public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_Discovery * pRealEvent = (Upnp_Discovery*)outEvent;

		ErrCode = pRealEvent->ErrCode;                  

		Expires = pRealEvent->Expires;                  

		DeviceId = (char*)pRealEvent->DeviceId;      

		DeviceType = (char*)pRealEvent->DeviceType;    

		ServiceType = (char*)pRealEvent->ServiceType;
		ServiceVer = (char*)pRealEvent->ServiceVer;    

		Location = (char*)pRealEvent->Location;      

		Os = (char*)pRealEvent->Os;            

		Date = (char*)pRealEvent->Date;            

		Ext = (char*)pRealEvent->Ext; 

		memcpy(&DestAddr,&pRealEvent->DestAddr,sizeof(DestAddr));
	}

public:
	int  ErrCode;                  

	int  Expires;                  

	char *DeviceId;      

	char *DeviceType;    

	char *ServiceType;

	char *ServiceVer;    

	char *Location;      

	char *Os;            

	char *Date;            

	char *Ext;           

	struct sockaddr_in DestAddr; 
};

class CEventSubscribe :public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_Event_Subscribe* pRealEvent = (Upnp_Event_Subscribe*)outEvent;
		
		Sid = (char*)pRealEvent->Sid;            
		ErrCode = pRealEvent->ErrCode;              
		PublisherUrl = (char*)pRealEvent->PublisherUrl; 
		TimeOut = pRealEvent->TimeOut; 
	}
public:
	char* Sid;            
	int ErrCode;              
	char *PublisherUrl; 
	int TimeOut;              

};

class CEventSubscriptionRequest:public CEvent
{
public:
	virtual void Translate(void* outEvent)
	{
		Upnp_Subscription_Request * pRealEvent = (Upnp_Subscription_Request*)outEvent;

		ServiceId = (char*)pRealEvent->ServiceId;
		UDN		  = (char*)pRealEvent->UDN;
		Sid		  = (char*)pRealEvent->Sid;
	}
public:
	char *ServiceId; 

	char *UDN;       

	char* Sid;

};

class CEventTickCount:public CEvent
{
public:
	virtual void Translate(void* outEvent){};
public:
	int nTickCount;
};




#endif


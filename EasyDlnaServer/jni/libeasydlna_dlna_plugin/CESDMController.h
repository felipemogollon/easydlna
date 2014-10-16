#ifndef __CESDMCONTROLLER_H__
#define __CESDMCONTROLLER_H__

#include "IDMController.h"
#include "IDlnaAVTransport.h"
#include "IDlnaConnectionManager.h"
#include "IDlnaContentDirectory.h"
#include "IDlnaRenderingControl.h"



struct CDetailDevice;
struct Service;
class CESDMController :public IDMController, IUpnpControlPointObserver
{
public:
	CESDMController();
	virtual ~CESDMController();

	virtual int SetContentDirectory(IDlnaContentDirectory* pIContentDirectory);
	virtual int SetAVTransport(IDlnaAVTransport* pIAVTransport);
	virtual int SetRenderingControl(IDlnaRenderingControl* pIRenderingControl);
	virtual int SetConnectionManager(IDlnaConnetionManager* pIConnectionManager);

	virtual int ScanNetwork(const char* scanContent,int TimeOut);

	virtual int SetObserver(IDMControllerObserver* pObserver);

	virtual IDlnaService* QueryService(const char* serviceType);

	virtual CDevice*	QueryDlnaDevice(const char* udn);
	virtual IUpnp*      QueryUpnp();


	virtual int Release();
	
	NORTIFY_HANDLE_RESULT Nortify(EventType type, CEvent & event);
	
	icstring GetDeviceSubUrl(icstring udn,icstring serviceType);
	icstring GetDeviceActionUrl(icstring udn,icstring serviceType);
	void SetDeviceSubscribeResult(icstring strUDN,icstring strSubUrl,CCommandSubscribe & subcomand);
private:

	int	  CanHandleThisType(const char* deviceType);
	int   HandleDeviceStatus(CEventDiscovery & discoveryEvent,EventType type);

	CDetailDevice*  HitDeviceBySubId(CListDevices* pListDevices,const char* subId);
	CDetailDevice*  HitDeviceBySubId(const char* subId);

	CDetailDevice * HitDevice(CListDevices * plistDevices,const char* udn,BOOL bRemove= FALSE);
	CDetailDevice * HitDevice(const char* udn,BOOL bRemove= FALSE);

	int   UpdateDevice(CDetailDevice* pDetailDevice,CEventDiscovery & discoveryEvent);

	int ParseDevice(IXML_Document *DescDoc,CEventDiscovery & discoveryEvent,CDetailDevice* pDevice);
	int ParseService(IXML_Element* serviceXMLNode,const char* location,Service* pDlnaService);
	int HandleNewDevice(CDetailDevice*& pDevice,CEventDiscovery & discoveryEvent);
	
	void ClearDevices(CListDevices* pList);
	void  HandleDeviceTickCount(CEventTickCount & tickCountEvent);
	void UpdateTickCountOneByOne(CListDevices *pList,int count);

	int HandleDeviceStateVar(CEventEvent & eventNortify);
	void UnsbscribeAllDevices();
	void UnsbscribeDevices(CListDevices* pList);
	void NortifyControllerObserver(CDevice* pObj)
	{
		if(m_pObserver)
		{
			m_LockContext = 1;
			m_pObserver->Nortify(*pObj);
			m_LockContext = 0;
		}
	}
	void ResetActiveAllInOne(CDetailDevice* pDevice)
	{
		if(m_pContentDirectory)
			m_pContentDirectory->ResetActive(pDevice);
		if(m_pConnectionManager)
			m_pConnectionManager->ResetActive(pDevice);
		if(m_pAVTransport)
			m_pAVTransport->ResetActive(pDevice);
		if(m_pRenderingControl)
			m_pRenderingControl->ResetActive(pDevice);
	};
	void DropActiveAllInOne(CDetailDevice* pDevice,const char* sid = NULL)
	{
		if(m_pContentDirectory)
			m_pContentDirectory->DropActive(pDevice,sid);
		if(m_pConnectionManager)
			m_pConnectionManager->DropActive(pDevice,sid);
		if(m_pAVTransport)
			m_pAVTransport->DropActive(pDevice,sid);
		if(m_pRenderingControl)
		m_pRenderingControl->DropActive(pDevice,sid);

	}
private:
	class CESConnetionManager: public IDlnaConnetionManager
	{
	public:
		CESConnetionManager(CESDMController* pController);
		virtual ~CESConnetionManager();
		virtual int SetObserver(IDlnaServiceObserver* pObserver)
		{
			m_Observer = pObserver;
			return 0;
		}
		virtual istring GetStatusValue(const char * ValName);
		virtual int Subscribe(BOOL bSub, int Timeout = 0);
		virtual	int SetActive(const char* udn);
		virtual int GetProtocalInfo(istring & source,istring &sink);

		virtual int GetCurrentConnectionIDs(istring & connectionids);
		
		virtual int GetCurrentConnectionInfo(ui4 connectionid,ui4& rcsid,ui4&avtransportid,
										istring& protocolinfo,istring& peerconnectionmanager,
										istring& peerconnectionid,istring&direction,
										istring&status);

		void NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify)
		{
			if(m_Observer)
				m_Observer->Nortify((IDlnaService*)this,pEventSource,eventNortify.ChangedVariables);
		};
		void DropActive(CDetailDevice *pDeactive,const char* sid = NULL)
		{
			if(pDeactive == m_pActiveDevice)
			{
				m_pActiveDevice = NULL;
				m_controlUrl = m_eventUrl = NULL;
				m_sid = NULL;

				return;
			}
			if(sid && sid==m_sid)
				m_sid = NULL;

		};
		void ResetActive(CDetailDevice* pDevice);

		int innerSubscribe(BOOL bSub, int Timeout);
	private:
		IDlnaServiceObserver * m_Observer;
		CESDMController* m_pController;
		CDetailDevice*		 m_pActiveDevice;
		icstring m_controlUrl;
		icstring m_eventUrl;
		icstring m_sid;
	};

	class CESAVTransport: public IDlnaAVTransport
	{
		friend class CESDMController;
	public:
		CESAVTransport(CESDMController* pController);
		virtual ~CESAVTransport();
			
		virtual int SetObserver(IDlnaServiceObserver* pObserver)
		{
			m_Observer = pObserver;
			return 0;			
		}
		virtual istring GetStatusValue(const char* ValName);
		virtual int Subscribe(BOOL bSub, int Timeout = 0);

		virtual	int SetActive(const char* udn);
		virtual  int SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata);
		virtual  int GetMediaInfo(ui4 instanceid,ui4& nrtracks,istring& mediaduration,istring& currenturi,
								  istring& currenturimetadata,istring &nexturi,istring& nexturimetadata,
								  istring& playmedium,istring& recordmedium,istring& writestatus);
		virtual  int GetTransportInfo(ui4 instanceid,istring& currenttransportstate,istring& currenttransportstatus,
										istring& currentspeed);
		virtual  int GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
									istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount);
		virtual  int GetDeviceCapabilities(ui4 instanceid,istring& playmedia,istring& recmedia,istring& recqualitymodes);
		virtual  int GetTransportSettings(ui4 instanceid,istring& playmode,istring& recqualitymode);
		virtual  int Stop(ui4 instanceid);

		virtual  int Play(ui4 instanceid,icstring speed);
		virtual  int Seek(ui4 instanceid,icstring unit,icstring target);
		virtual  int Next(ui4 instanceid);

		virtual  int Previous(ui4 instanceid);
		void NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify)
		{
			if(m_Observer)
				m_Observer->Nortify((IDlnaService*)this,pEventSource,eventNortify.ChangedVariables);
		};
		void ResetActive(CDetailDevice* pDevice);
		void DropActive(CDetailDevice *pDeactive,const char* sid = NULL)
		{
			if(pDeactive == m_pActiveDevice)
			{
				m_pActiveDevice = NULL;
				m_controlUrl = m_eventUrl = NULL;
				m_sid = NULL;
				return ;
			}
			if(sid && sid==m_sid)
				m_sid = NULL;
		};
		int innerSubscribe(BOOL bSub, int Timeout);
	private:
		int   PlayAction(const char* pActionName,ui4 instanceid);
	private:
		IDlnaServiceObserver* m_Observer;
		CESDMController* m_pController;
		CDetailDevice*		 m_pActiveDevice;
		icstring m_controlUrl;
		icstring m_eventUrl;
		icstring m_sid;

	};

	class CESContentDirectory : public IDlnaContentDirectory
	{
		friend class CESDMController;

	public:
		CESContentDirectory(CESDMController * pController);
		virtual ~CESContentDirectory();
		
		virtual int SetObserver(IDlnaServiceObserver* pObserver)
		{
			m_Observer = pObserver;
			return 0;			
		}
		virtual istring GetStatusValue(const char* ValName);
		virtual int Subscribe(BOOL bSub, int Timeout = 0);

		virtual	int SetActive(const char* udn);
		
		virtual int GetSearchCapabilities(istring & searchcaps);

		virtual int GetSortCapabilities(istring & sortcaps);

		virtual  int GetSystemUpdateID(ui4 & id);

		virtual int Browse(icstring id,icstring browseflag,icstring filter,
							ui4 startingindex,ui4 requestedcount,icstring sort,
							istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid);
		void ResetActive(CDetailDevice* pDevice);
		void NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify)
		{
			if(m_Observer)
				m_Observer->Nortify((IDlnaService*)this,pEventSource,eventNortify.ChangedVariables);
		};
		void DropActive(CDetailDevice *pDeactive,const char* sid = NULL)
		{
			if(pDeactive == m_pActiveDevice)
			{
				m_pActiveDevice = NULL;
				m_controlUrl = m_eventUrl = NULL;
				m_sid = NULL;
				return;
			}
			if(sid && sid==m_sid)
				m_sid = NULL;
		};
		int innerSubscribe(BOOL bSub, int Timeout);

	private:
		IDlnaServiceObserver* m_Observer;
		CESDMController* m_pController;
		CDetailDevice*		 m_pActiveDevice;
		icstring m_controlUrl;
		icstring m_eventUrl;
		icstring m_sid;
	};

	class CESRenderingControl: public IDlnaRenderingControl
	{
		friend class CESDMController;
	public:
		CESRenderingControl(CESDMController * pController);
		virtual ~CESRenderingControl();

	public:
		virtual int SetObserver(IDlnaServiceObserver* pObserver)
		{
			m_Observer = pObserver;
			return 0;			
		}
		virtual istring GetStatusValue(const char* ValName);
		virtual int Subscribe(BOOL bSub, int Timeout = 0);
		virtual	int SetActive(const char* udn);
		virtual int ListPresets(ui4 instanceid,istring& currentpresetnamelist);
		virtual int SelectPreset(ui4 instanceid,icstring presetname);
		
		virtual void NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify)
		{
			if(m_Observer)
				m_Observer->Nortify((IDlnaService*)this,pEventSource,eventNortify.ChangedVariables);
		};
		void ResetActive(CDetailDevice* pDevice);
		void DropActive(CDetailDevice *pDeactive,const char* sid = NULL)
		{
			if(pDeactive == m_pActiveDevice)
			{
				m_pActiveDevice = NULL;
				m_controlUrl = m_eventUrl = NULL;
				m_sid = NULL;
				return ;
			}
			if(sid && sid==m_sid)
				m_sid = NULL;
		};
		int innerSubscribe(BOOL bSub, int Timeout);
	private:
		IDlnaServiceObserver* m_Observer;
		CESDMController* m_pController;
		CDetailDevice*		 m_pActiveDevice;
		icstring m_controlUrl;
		icstring m_eventUrl;
		icstring m_sid;
	};


	CESConnetionManager* m_pConnectionManager;
	CESAVTransport*		 m_pAVTransport;
	CESContentDirectory* m_pContentDirectory;
	CESRenderingControl* m_pRenderingControl;

	IDlnaConnetionManager* m_pIConnectionManager;
	IDlnaAVTransport*	  m_pIDlnaAVTransport;
	IDlnaContentDirectory*  m_pIDlnaContentDirtory;
	IDlnaRenderingControl*  m_pIDlnaRenderintControl;

	IUpnpControlPoint*	   m_pIUpnp;
	IDMControllerObserver* m_pObserver;

friend class CESConnetionManager;
friend class CESAVTransport;
friend class CESContentDirectory;
friend class CESRenderingControl;

private:
	volatile int	m_LockContext;
	IUpnpUtil::CRWLock m_rwListLock;
	CListDevices  m_listRender;
	CListDevices  m_listServer;
	CListDevices  m_listPlayer;
};




#endif


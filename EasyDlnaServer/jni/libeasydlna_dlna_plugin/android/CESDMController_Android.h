#ifndef __CESDMCONTROLLER_ANDROID_H__
#define __CESDMCONTROLLER_ANDROID_H__

#ifdef USE_NDK
	#include <jni.h>
#else
	#include "JNIHelp.h"
#endif
#include "CESDMController.h"
#include "IUpnpUtil.h"
#include "IDlnaService.h"

class CDlnaService;
class IDlnaFrame;
class CESDMController;

class DMCObserver:public IDMControllerObserver,IDlnaServiceObserver
{
public:
	DMCObserver(JNIEnv* env);
	virtual ~DMCObserver();
	virtual int Nortify(CDevice & dlnaDevice);

	virtual int Nortify(IDlnaService *pSource, CDevice* pSrcDevice,IXML_Document * changeValueList);

	void SetJavaCallback(JNIEnv* env,jobject jcallback);
	void Destroy(JNIEnv* env);

private:
	IUpnpUtil::CRWLock m_lock;
	jobject m_jCallback;
	jclass  m_jDeviceClass;

	jfieldID judnString;
	jfieldID jnameString;
	jfieldID jtypeString;
	jfieldID jbaseUrlString;
	jfieldID jstate;
	jmethodID jConstructor;

	jmethodID jNotify;
	jmethodID jAVTStatusChanges;

};
class CESAVTransportClient_Android:public IDlnaAVTransport
{
public:
	CESAVTransportClient_Android(IUpnpControlPoint * pIUpnpDevice);
	virtual ~CESAVTransportClient_Android();

	virtual  int SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata){return -1;};

	virtual  int GetMediaInfo(ui4 instanceid,ui4& nrtracks,istring& mediaduration,istring& currenturi,
							  istring& currenturimetadata,istring &nexturi,istring& nexturimetadata,
							  istring& playmedium,istring& recordmedium,istring& writestatus){return -1;};

	virtual  int GetTransportInfo(ui4 instanceid,istring& currenttransportstate,istring& currenttransportstatus,
			   					 istring& currentspeed){return -1;};

	virtual  int GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
								 istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount ){return -1;};

	virtual  int GetDeviceCapabilities(ui4 instanceid,istring& playmedia,istring& recmedia,istring& recqualitymodes)
									   {return -1;};

	virtual  int GetTransportSettings(ui4 instanceid,istring& playmode,istring& recqualitymode){return -1;};

	virtual  int Stop(ui4 instanceid){return -1;};

	virtual  int Play(ui4 instanceid,icstring speed){return -1;};

	virtual  int Seek(ui4 instanceid,icstring unit,icstring target){return -1;};
	
	virtual  int Subscribe(icstring strudn,icstring strUrl);

	virtual  int Next(ui4 instanceid){return -1;};

	virtual  int Previous(ui4 instanceid){return -1;};

	int		playControl(icstring url,int type,icstring param,istring & result);
	virtual int SetObserver(IDlnaServiceObserver * pObserver){m_pServiceObserver = pObserver;return 0;};
	virtual void NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify);

	int ParseLastChange(JNIEnv* env,jstring & urlString,jstring & stateString,jstring &statusString,
							   jstring & durationString,IXML_Document * changeValueList);
	int parsePosition(icstring result);
public:
	IUpnpControlPoint * m_pIUpnpCtrlpt;
	IDlnaServiceObserver* m_pServiceObserver;
	CESDMController* m_pController;
};
class CESContentDirectory_Android : public IDlnaContentDirectory
{

public:
	CESContentDirectory_Android(IUpnpControlPoint * pIUpnpDevice);
	virtual ~CESContentDirectory_Android();
	virtual void NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify);
	virtual istring GetStatusValue(const char* ValName){return NULL;};
	virtual int Subscribe(BOOL bSub, int Timeout = 0){return -1;};


	virtual int GetSearchCapabilities(istring & searchcaps){return -1;};

	virtual int GetSortCapabilities(istring & sortcaps){return -1;};

	virtual  int GetSystemUpdateID(ui4 & id){return -1;};

	virtual int Browse(icstring id,icstring browseflag,icstring filter,
		ui4 startingindex,ui4 requestedcount,icstring sort,
		istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid){return -1;};

	int Subscribe(icstring strudn,icstring strUrl);

	int BrowseChildren(icstring url, icstring id,ui4 startingindex,ui4 requestedcount,istring& result);

	int BrowseMetaData(icstring url, icstring id,istring& result);


	int ParseChildren(JNIEnv* env,istring result, jobjectArray& joutInfoArray);
	int ParseMetadata(JNIEnv* env,istring result,jobject & jObjInitInfo);
public:
	IUpnpControlPoint * m_pIUpnpCtrlpt;
	CESDMController* m_pController;
};

class CESDMController_Android
{
public :
	CESDMController_Android();
	~CESDMController_Android();

	static int RegisterDMCService(JNIEnv* env);
	
	void  InitDMC(JNIEnv *env, IDlnaFrame* pDlnaFrame);
	void  DestroyDMC(JNIEnv *env);



public:
	CESDMController* m_pController;
	CESContentDirectory_Android* m_pCDS;
	CESAVTransportClient_Android* m_pAVT;
	DMCObserver* m_pObserver;
};















#endif

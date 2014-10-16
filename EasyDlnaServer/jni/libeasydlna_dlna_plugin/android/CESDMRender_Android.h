#ifndef __CESDMRENDER_ANDROID_H__
#define __CESDMRENDER_ANDROID_H__

#ifdef USE_NDK
	#include <jni.h>
#else
	#include "JNIHelp.h"
#endif
#include "IDlnaAVTransport.h"
#include "CESDMRender.h"

class CDlnaService;
class IDlnaFrame;



class CESAVTransport_Android:public IDlnaAVTransport
{
public:
	CESAVTransport_Android(JNIEnv *env,IUpnpDevice* pDevice);
	virtual ~CESAVTransport_Android();

public:
	void SetCallbackObject(JNIEnv*env, jobject jcallback);
	void Destroy(JNIEnv* env);
public:
	virtual  int SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata);
	virtual  int GetMediaInfo(ui4 instanceid,ui4& nrtracks,istring& mediaduration,istring& currenturi,
		istring& currenturimetadata,istring &nexturi,istring& nexturimetadata,
		istring& playmedium,istring& recordmedium,istring& writestatus){return ERROR_INVALID_ACTION;}

	virtual  int GetTransportInfo(ui4 instanceid,istring& currenttransportstate,istring& currenttransportstatus,
		istring& currentspeed){return ERROR_INVALID_ACTION;}

	

	virtual  int GetDeviceCapabilities(ui4 instanceid,istring& playmedia,istring& recmedia,istring& recqualitymodes)
									  {return ERROR_INVALID_ACTION;}
	virtual  int Next(ui4 instanceid)
	{return ERROR_INVALID_ACTION;}

	virtual  int Previous(ui4 instanceid)
	{return ERROR_INVALID_ACTION;}

	virtual  int GetTransportSettings(ui4 instanceid,istring& playmode,istring& recqualitymode)
				 {return ERROR_INVALID_ACTION;}
	
	virtual  int GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
								istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount);
	virtual  int Stop(ui4 instanceid);
	virtual  int Play(ui4 instanceid,icstring speed);
	virtual  int Pause(ui4 instanceid);
	virtual  int Seek(ui4 instanceid,icstring unit,icstring target);


	

	virtual int SetActive(const char* udn)
	{
		SAFEDUP(m_thizUDN,udn);
		return 0;
	}
	virtual int SetPropSet(const IXML_Document* PropSet);
	virtual int GetPropSet(IXML_Document*& PropSet);

	int NortifyStateChange(JNIEnv*env,jobject jstateObject);
public:
	static jclass    classJavaDMRService;
	static jmethodID methIdSetDataSource;
	static jmethodID methIdStop;
	static jmethodID methIdPlay;
	static jmethodID methIdPause;
	static jmethodID methIdSeek;
	static jmethodID methIdGetPos;
	static jobject	 m_jCallback;
private:
	char*   m_thizUDN;
	IUpnpDevice* m_pIUpnp;
	char*   uri;
	char*   duration;


};


class CESDMRender_Android
{
public :
	CESDMRender_Android();
	~CESDMRender_Android();

	static int RegisterDMRService(JNIEnv* env);

	void  InitDMR(JNIEnv *env, IDlnaFrame* pDlnaFrame);
	void  DestroyDMR(JNIEnv *env);



public:
	static istring      m_RenderName;
	CESDMRender* m_pRender;
	CESAVTransport_Android* m_pAVT;
};




#endif



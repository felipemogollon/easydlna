#ifndef __CESDMSERVER_ANDROID_H__
#define __CESDMSERVER_ANDROID_H__
#ifdef USE_NDK
	#include <jni.h>
#else
	#include "JNIHelp.h"
#endif
#include "IDlnaContentDirectory.h"
#include "CESDMServer.h"

#include <list>
using std::list;


class CDlnaService;
class IDlnaFrame;
class IWebVFSObserver;
class CESSpecialDevice_Android;
	
typedef list<int> CFileHandleList;






class CESContentDirectory_Device_Android : public IDlnaContentDirectory,IWebVFSCallbacks
{

public:
	CESContentDirectory_Device_Android(JNIEnv *env,IUpnpDevice * pIUpnpDevice);
	virtual ~CESContentDirectory_Device_Android();

public:
	void SetCallbackObject(JNIEnv*env, jobject jcallback);
	void Destroy(JNIEnv* env);
	void SetRemoteUIEnabled(BOOL remoteuistarted);
	virtual istring GetStatusValue(const char* ValName);
	virtual int Subscribe(BOOL bSub, int Timeout = 0){return -1;};


	virtual int GetSearchCapabilities(istring & searchcaps);

	virtual int GetSortCapabilities(istring & sortcaps);

	virtual  int GetSystemUpdateID(ui4 & id);
	
	virtual int Browse(icstring id,icstring browseflag,icstring filter,
		ui4 startingindex,ui4 requestedcount,icstring sort,
		istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid);
	
	virtual int SetPropSet(const IXML_Document* PropSet);
	virtual int GetPropSet(IXML_Document*& PropSet);

	virtual int SetActive(const char* udn)
	{
		SAFEDUP(m_thizUDN,udn);
		return 0;
	}

private:
	static int ParseJMetaData(JNIEnv* env,jobject jContentInfo, string & result);
	static int ParseJMetaData(JNIEnv* env,jobject jContentInfo,File_Info* info);

public:
	virtual int SetWebVFSObserver(IWebVFSObserver *pObserver);
	virtual int GetFileInfo(IN  const char *filename, OUT struct File_Info *info);
	virtual UpnpWebFileHandle FileOpen(const char *filename,IN enum UpnpOpenFileMode Mode);
	virtual int FileRead(IN UpnpWebFileHandle fileHnd, OUT char *buf, IN size_t buflen);
	virtual int FileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen);
	virtual int FileSeek(IN UpnpWebFileHandle fileHnd, IN off_t offset,IN int origin);
	virtual int FileClose(IN UpnpWebFileHandle fileHnd);


public:
	static jobject	 m_jCallback;
	static jmethodID methIdBrowseMetaData;
	static jmethodID methIdBrowseChildren;
	static jmethodID methIdGetFileInfo;
public:
	IUpnpDevice * m_pIUpnp;
	volatile		  ui4 m_systemUpdateId;
	char*   m_thizUDN;
	IUpnpUtil::CRWLock m_lock;
	CFileHandleList m_fileHandleList;
	//volatile BOOL m_bRemoteUiStarted;
	//CESSpecialDevice_Android* m_pSpecialDevice;
	
};




class CESDMServer_Android
{
public :
	CESDMServer_Android();
	~CESDMServer_Android();

	static int RegisterDMSService(JNIEnv* env);

	void  InitDMS(JNIEnv *env, IDlnaFrame* pDlnaFrame);
	void  DestroyDMS(JNIEnv *env);



public:
	static istring      m_ServerName;
	CESDMServer*		m_pServer;
	CESContentDirectory_Device_Android* m_pCDS;
};




#endif



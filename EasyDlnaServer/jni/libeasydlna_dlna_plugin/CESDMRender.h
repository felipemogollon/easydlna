#ifndef __CESDMRENDER_H__
#define __CESDMRENDER_H__

#include "IDMRender.h"
#include "IDlnaAVTransport.h"
#include "IDlnaConnectionManager.h"
#include "IDlnaRenderingControl.h"

class IUpnpDevice;

//#ifdef OS_ANDROID
//	typedef List<CFileInfo*>CFileList;
//#else
	typedef list<CFileInfo*>CFileList;
//#endif





class CESDMRender: public IDMRender, IUpnpDeviceObserver
{
public:
	CESDMRender();
	virtual ~CESDMRender();
	
public:
	virtual int SetDlnaDeviceNortifyHandler(IDlnaDeviceNortifyHandle* pNortifyHandler)
	{
		m_pDlnaDeviceHandler = pNortifyHandler;
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
	virtual int RegisterDMRender(icstring content,BOOL bFromLocalFile,icstring friendlyName = NULL);
	virtual int Broadcast(int TimeOut) ;
	virtual int Release();
	
	virtual IUpnp*   QueryUpnp();
public:
	static int HTTPGetFileInfo (IN const char *filename,OUT struct File_Info *info);
	static UpnpWebFileHandle HTTPOpenFile(IN const char *filename,IN enum UpnpOpenFileMode Mode);
	static int HTTPFileRead(IN UpnpWebFileHandle fileHnd,OUT char *buf,IN size_t buflen);
	static int HTTPFileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen);
	static int HTTPFileSeek(IN UpnpWebFileHandle fileHnd,IN off_t offset,IN int origin);
	static int HTTPFileClose(IN UpnpWebFileHandle fileHnd);
public:
	virtual ERROR_TYPE Nortify(EventType deviceEventType,CEvent & deviceEvent);


public:
	static char*  m_myUDN;

private:
	
	int   m_nFlags;
	IDlnaDeviceNortifyHandle* m_pDlnaDeviceHandler;
	
	IDlnaAVTransport*	   m_pAVTransport;
	IDlnaRenderingControl* m_pRenderingControl;
	IDlnaConnetionManager* m_pConnectionManager;
	
	static IUpnpUtil::CRWLock  m_fileLock;
	static CFileList  m_FileList;
	CDevice* m_pDeviceMySelf;
	IUpnpDevice* m_IUpnp;


};

















#endif


#ifndef __IUpnpRROR_H__
#define __IUpnpRROR_H__

#include "UpnpCommand.h"

enum DECLARE_TYPE{UPNP_UNKNOWN = -1,UPNP_CLIENT = 0,UPNP_ROOTDEVICE = 1,UPNP_ROOTDEVICE2 = 2};
enum DescType{REG_URL_DESC,REG_FILENAME_DESC,REG_BUF_DESC};


#define UPNP_PROPLIST_HEAD\
  "<e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\">"

#define UPNP_ITEM_PROP_HEAD\
	"<e:property>"

#define UPNP_ITEM_PROP_END\
	"</e:property>"
#define UPNP_PROPLIST_END\
	"</e:propertyset>"

struct UpnpVirtualDirCallbacks
{
	VDCallback_GetInfo get_info;
	VDCallback_Open open;
	VDCallback_Read read;
	VDCallback_Write write;
	VDCallback_Seek seek;
	VDCallback_Close close;
};
class CRegisterType
{
public:
	CRegisterType(){nDeclareType = UPNP_UNKNOWN;};
	DECLARE_TYPE nDeclareType;
};
class CRegisterTypeClient: public CRegisterType
{
public:
	CRegisterTypeClient(){nDeclareType = UPNP_CLIENT;};
};
class CRegisterTypeRootDevice: public CRegisterType
{
public:
	CRegisterTypeRootDevice(){nDeclareType =UPNP_ROOTDEVICE;};
	const char *DescUrl;
};
class CRegisterTypeRootDevice2: public CRegisterType
{
public:
	CRegisterTypeRootDevice2(){nDeclareType =UPNP_ROOTDEVICE2;};

	
	
	DescType descriptionType;
	const char* description;
	size_t bufferLen;
	int config_baseURL;
};

class IUpnpControlPoint;
class IUpnpDevice;


class IUpnp
{
public:
	virtual ~IUpnp(){};
	static ERROR_TYPE Start(const char* ip,unsigned short usPort);
	static ERROR_TYPE Finish();

	static char*		GetUpnpIp();
	static unsigned short GetUpnpPort();

public:
	static IUpnpControlPoint* CreateUpnpControlPoint();
	static IUpnpDevice*	CreateUpnpDevice();

	static ERROR_TYPE DestroyIUpnp(IUpnp* pDestroyObject);

public:
	virtual ERROR_TYPE Register(const CRegisterType & cType)  = 0;
	virtual ERROR_TYPE UnRegister() = 0;
	virtual ERROR_TYPE ExecuteCommand(CCommand & command){return ERROR_SUCCESS;};

	//http relative function
public:
	static ERROR_TYPE SetMaxContentLength(size_t contentLength);
	static ERROR_TYPE DownloadUrlItem(IN const char *url,OUT char **outBuf,OUT char *contentType);

	static ERROR_TYPE OpenHttpGet(IN const char *url,IN OUT void **handle,IN OUT char **contentType,
  								   IN OUT int *contentLength, IN OUT int *httpStatus,IN int timeout);

	static ERROR_TYPE OpenHttpGetProxy(IN const char *url,IN const char *proxy_str,IN OUT void **handle,
										    IN OUT char **contentType,IN OUT int *contentLength, 
											IN OUT int *httpStatus,IN int timeout);

	static ERROR_TYPE OpenHttpGetEx(IN const char *url, IN OUT void **handle, IN OUT char **contentType, 
  									 IN OUT int *contentLength, IN OUT int *httpStatus,	IN int lowRange,
									 IN int highRange,IN int timeout);

	static ERROR_TYPE ReadHttpGet(IN void *handle,IN OUT char *buf,IN OUT unsigned int *size,IN int timeout);
	static ERROR_TYPE HttpGetProgress(IN void *handle,OUT unsigned int *length,OUT unsigned int *total);
	static ERROR_TYPE CancelHttpGet(IN void *handle);
	static ERROR_TYPE CloseHttpGet(IN void *handle);

	static ERROR_TYPE OpenHttpPost(IN const char *url, IN OUT void **handle,IN const char *contentType, 
								   IN int contentLength, IN int timeout);

	static ERROR_TYPE WriteHttpPost(IN void *handle,IN char *buf, IN unsigned int *size,IN int timeout);
	static ERROR_TYPE CloseHttpPost(IN void *handle,IN OUT int *httpStatus,IN int timeout);
	static ERROR_TYPE DownloadXmlDoc(IN const char *url,OUT IXML_Document **xmlDoc);


public:
	static ERROR_TYPE SetWebServerRootDir(IN const char* rootDir);
	
	//static ERROR_TYPE SetVirtualDirCallbacks(IN UpnpVirtualDirCallbacks *callbacks);
	static ERROR_TYPE SetVirtualDirCallbacks(IN UpnpVirtualDirCallbacks *callbacks);

	static ERROR_TYPE EnableWebserver(IN int enable);

	static BOOL       IsWebserverEnabled();

	static ERROR_TYPE AddVirtualDir(const char *dirName);

	static ERROR_TYPE RemoveVirtualDir(IN const char *dirName);

	static ERROR_TYPE RemoveAllVirtualDirs();
};


struct CFileInfo
{
	int index;
	off_t offset;
	off_t length;
	char*  buf;
	long long id;
	CFileInfo()
	{
		index = 0;
		offset = length = 0LL;
		buf = NULL;
		id = -1;
	}
};




class IWebVFSObserver
{
public:
	IWebVFSObserver(){};
	virtual ~IWebVFSObserver(){};
public:
	virtual int Nortify(WebFileInfo & webfileInfo) = 0;
};

class IWebVFSCallbacks
{
public:
	IWebVFSCallbacks(){};
	virtual ~IWebVFSCallbacks(){};

public:
	virtual int SetWebVFSObserver(IWebVFSObserver *pObserver) = 0;
	virtual int GetFileInfo(IN  const char *filename, OUT struct File_Info *info) = 0;
	virtual UpnpWebFileHandle FileOpen(const char *filename,IN enum UpnpOpenFileMode Mode) = 0;
	virtual int FileRead(IN UpnpWebFileHandle fileHnd, OUT char *buf, IN size_t buflen) = 0;
	virtual int FileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen) = 0;
	virtual int FileSeek(IN UpnpWebFileHandle fileHnd, IN off_t offset,IN int origin) = 0;
	virtual int FileClose(IN UpnpWebFileHandle fileHnd) = 0;
};




enum NORTIFY_HANDLE_RESULT
{
		NHR_CONTINUE,
		NHR_BREAK,
};
class IUpnpControlPointObserver
{
public:	
	virtual ~IUpnpControlPointObserver(){};
	virtual NORTIFY_HANDLE_RESULT Nortify(EventType type, CEvent & event) = 0;
	virtual int SetNortifySource(IUpnp* pSource){m_pNortifySource = pSource;return 0;};

private:
	IUpnp * m_pNortifySource;
};

//#ifdef OS_ANDROID
//	typedef List<IUpnpControlPointObserver*>CListIUpnpObserver;
//#else
	typedef list<IUpnpControlPointObserver*>CListIUpnpObserver;
//#endif


class IUpnpControlPoint : public IUpnp
{
public:
	virtual ~IUpnpControlPoint(){};
	virtual ERROR_TYPE AddUpnpControlPointObserver(IUpnpControlPointObserver* IObserver) = 0;
	virtual ERROR_TYPE RemoveUpnpControlPointObserver(IUpnpControlPointObserver* observer) = 0;

};

class IUpnpDeviceObserver
{
public:
	virtual ~IUpnpDeviceObserver(){};
	virtual ERROR_TYPE Nortify(EventType deviceEventType,CEvent & deviceEvent) = 0;
	virtual int SetNortifySource(IUpnp* pSource){m_pNortifySource = pSource;return 0;};
private:
	IUpnp * m_pNortifySource;
};


class IUpnpDevice :public IUpnp
{
public:
	virtual ~IUpnpDevice(){};
	virtual ERROR_TYPE Register(const CRegisterType & cType)  = 0;
	virtual ERROR_TYPE UnRegister() = 0;
	virtual ERROR_TYPE AddUpnpDeviceObserver(IUpnpDeviceObserver* IObserver) = 0;
	virtual ERROR_TYPE RemoveUpnpDeviceObserver(IUpnpDeviceObserver* IObserver) = 0;	

};





#endif 


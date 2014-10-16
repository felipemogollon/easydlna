#ifndef __CESDMSERVER_H__
#define __CESDMSERVER_H__


#include "IUpnpUtil.h"
#include "IDMServer.h"
#include "IDlnaAVTransport.h"
#include "IDlnaConnectionManager.h"
#include "IDlnaContentDirectory.h"


typedef list<i64>CListIds;



struct CDEntryEx:public CDEntry
{
	static i64 refid;
	int   readable;
	off_t length;
	time_t lastmodified;
	char* postfix;
	char* filepath;
	int    childcounts;
	CListIds childids;
	CDEntryEx():CDEntry()
	{
		length = 0;
		filepath = NULL;
		postfix = NULL;
		childcounts = -1;
		readable = 1;
		lastmodified = (time_t)0LL;
	}
	~CDEntryEx()
	{
		FREEARRAY(filepath);
		FREEARRAY(postfix);
		childids.clear();
	}
	void RmoveAllChildren()
	{
		childids.clear();
	}
	void RemoveChildId(i64 & id)
	{
		
		CListIds::iterator itbegin = childids.begin();
		CListIds::iterator itend = childids.end();
		for (itbegin;itbegin!=itend; ++itbegin)
		{
			i64 iexist = (i64)(*itbegin);
			if(iexist == id)
			{
				childids.erase(itbegin);
				return;
			}
		}
		
		return;
	}
};

class IUpnpDevice;


typedef map<i64,CDEntryEx*>CMapEntriesEx;
typedef map<int,CFileInfo*>CMapFiles;


class CESDMServer: public IDMServer, IUpnpDeviceObserver,IDlnaContentDirectory,IWebVFSCallbacks
{
public:
	CESDMServer();
	virtual ~CESDMServer();
public:
	virtual IWebVFSCallbacks* QueryVFSCallbackInterface()
	{
		return m_pWebVFSCb;
	}
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
	virtual int SetContentDirectroy(IDlnaContentDirectory* pIContentDirectory)
	{
		m_pContentDirectory = pIContentDirectory;
		return 0;
	}
	virtual int SetConnectionManager(IDlnaConnetionManager* pIConnectionManager)
	{
		m_pConnectionManager = pIConnectionManager;
		return 0;
	}
	virtual int SetVFSCallbackInterface(IWebVFSCallbacks* pIWebVFSCallbacks)
	{
		m_pWebVFSCb = pIWebVFSCallbacks;
		return 0;
	}

	virtual int AddContentDirectory(const char* dirName);
	virtual int RemoveContentDirectory(const char* dirName);

	virtual int RegisterDMServer(const char* content,BOOL bFromLocalFile,const char* friendlyName = NULL);
	virtual int Broadcast(int TimeOut);
	virtual int Release();

	virtual IUpnp*      QueryUpnp();
	virtual int SetWebVFSObserver(IWebVFSObserver *pObserver)
	{
		m_pWebVFSObserver = pObserver;
		return 0;
	}
	virtual int GetFileInfo(IN  const char *filename, OUT struct File_Info *info);
	virtual UpnpWebFileHandle FileOpen(const char *filename,IN enum UpnpOpenFileMode Mode);
	virtual int FileRead(IN UpnpWebFileHandle fileHnd, OUT char *buf, IN size_t buflen);
	virtual int FileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen);
	virtual int FileSeek(IN UpnpWebFileHandle fileHnd, IN off_t offset,IN int origin);
	virtual int FileClose(IN UpnpWebFileHandle fileHnd);

public:
	static int HTTPGetFileInfo (IN const char *filename,OUT struct File_Info *info);
	static UpnpWebFileHandle HTTPOpenFile(IN const char *filename,IN enum UpnpOpenFileMode Mode);
	static int HTTPFileRead(IN UpnpWebFileHandle fileHnd,OUT char *buf,IN size_t buflen);
	static int HTTPFileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen);
	static int HTTPFileSeek(IN UpnpWebFileHandle fileHnd,IN off_t offset,IN int origin);
	static int HTTPFileClose(IN UpnpWebFileHandle fileHnd);

	int inner_HTTPGetFileInfo (IN const char *filename,OUT struct File_Info *info);
	UpnpWebFileHandle inner_HTTPOpenFile(IN const char *filename,IN enum UpnpOpenFileMode Mode);
	int inner_HTTPFileRead(IN UpnpWebFileHandle fileHnd,OUT char *buf,IN size_t buflen);
	int inner_HTTPFileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen);
	int inner_HTTPFileSeek(IN UpnpWebFileHandle fileHnd,IN off_t offset,IN int origin);
	int inner_HTTPFileClose(IN UpnpWebFileHandle fileHnd);

public:
	virtual ERROR_TYPE Nortify(EventType deviceEventType,CEvent & deviceEvent);
public:
	virtual int SetActive(const char* udn)
	{
		SAFEDUP(m_strActive,udn);
		return 0;			
	}
	virtual int GetPropSet(IXML_Document*& PropSet){return ERROR_INVALID_ACTION;};
	virtual  istring GetStatusValue(const char* ValName){return NULL;};

	virtual int GetSearchCapabilities(istring & searchcaps);

	virtual int GetSortCapabilities(istring & sortcaps);

	virtual  int GetSystemUpdateID(ui4 & id);
	virtual int Browse(icstring id,icstring browseflag,icstring filter,
		ui4 startingindex,ui4 requestedcount,icstring sort,
		istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid);
private:
	const char* GetUpnpClass(icstring postfix);
	char* GetShortName(icstring dirname);
	CDEntryEx* IsDirExist(const char* dirname);
	int AddAndParseDir(const char* dirname,CDEntryEx* pParentEntry);
	void AddFileInfo(CDEntryEx* pChildFile,icstring postfix);
	void GetIdFromFileName(const char* filename,i64 & idout);
	void AddEntryResoureInfo();
	void AddEntryResoureInfo(CDEntryEx* pChildFile);
	icstring GetContentTypeFromProtocolInfo(icstring protocolinfo,int & nlength);

public:
	static char*  m_myUDN;

private:

	IDlnaAVTransport*	   m_pAVTransport;
	IDlnaContentDirectory* m_pContentDirectory;
	IDlnaConnetionManager* m_pConnectionManager;
	IWebVFSCallbacks*	   m_pWebVFSCb;
	int      m_nFlag;
	CDevice* m_pDeviceMySelf;
	IUpnpDevice* m_IUpnp;
	
	char*  m_strActive;
	CMapEntriesEx      m_FileContents;
	int     m_fileIndex;
	CDEntryEx*		   m_pRootEntries;
	IUpnpUtil::CRWLock m_containerLock;
	
	IWebVFSObserver*		  m_pWebVFSObserver;
	IDlnaDeviceNortifyHandle* m_pDlnaDeviceHandler;
	CMapFiles          m_Files;
	volatile		  ui4 m_systemUpdateId;
};
#endif


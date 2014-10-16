#define LOG_TAG "easydlna-esdmserver"
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>

#include "CESDlnaDeviceHandler.h"
#include "ESDlnaHelper.h"
#include "CESAVTransport.h"
#include "CESConnectionManager.h"
#include "CESContentDirectory.h"

#include "CESDMServer.h"
#include "ESResouce.h"

#define EASYDLNA_UUID "uuid:8AA439EE-6A87-4c5e-ADFD-C489F11FEECC"
#define EASYDLNA_BASEUUID "uuid:8AA439EE-6A87-4c5e-ADFD"

#define EASYDLNA_DMS_FRIENDLYNAME "EasyDlna Media Server"

#define VIRTUAL_ROOT "/root"

#define CONTENT_URL_LENGTH (512)

i64 CDEntryEx::refid = -1LL;
#ifdef PLATFORM_64
#define MAKE_FD64(fd) (((long)fd)<<32)
#define PARSE_FD64(fd) ((int)(((long)fd)>>32))
#else
#define MAKE_FD64(fd) (((long)fd))
#define PARSE_FD64(fd) ((int)(((long)fd)))
#endif

MimeType MimeTypeAll[] = 
{
	{ "wmv",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/x-ms-wmv:"},
	{ "mjpg",  UPNPCLASS_ITEM_VIDEO, "http-get:*:video/x-motion-jpeg:"},
	{ "asf",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/x-ms-asf:"},
	{ "avc",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/avi:"},
	{ "avi",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/avi:"},
	{ "divx",  UPNPCLASS_ITEM_VIDEO, "http-get:*:video/avi:"},
	{ "mjpeg", UPNPCLASS_ITEM_VIDEO, "http-get:*:video/x-motion-jpeg:"},
	{ "mpeg",  UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg:"},
	{ "mpg",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg:"},
	{ "mpe",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg:"},
	{ "mp2p",  UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mp2p:"},
	{ "vob",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mp2p:"},
	{ "m1v",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg:"},
	{ "m2v",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg2:"},
	{ "mpg2",  UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg2:"},
	{ "mpeg2", UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg2:"},
	{ "m4v",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mp4:"},
	{ "m4p",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mp4:"},
	{ "mp4",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mp4:"},
	{ "mp4ps", UPNPCLASS_ITEM_VIDEO, "http-get:*:video/x-nerodigital-ps:"},
	{ "ts",    UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg2:"},
	{ "ogm",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg:"},
	{ "mkv",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg:"},
	{ "rmvb",  UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg:"},
	{ "mov",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/quicktime:"},
	{ "hdmov", UPNPCLASS_ITEM_VIDEO, "http-get:*:video/quicktime:"},
	{ "qt",    UPNPCLASS_ITEM_VIDEO, "http-get:*:video/quicktime:"},
	{ "bin",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg2:"},
	{ "iso",   UPNPCLASS_ITEM_VIDEO, "http-get:*:video/mpeg2:"},

	{ "3gp",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/3gpp:"},
	{ "aac",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/x-aac:"},
	{ "ac3",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/x-ac3:"},
	{ "aif",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/aiff:"},
	{ "aiff", UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/aiff:"},
	{ "dts",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/x-dts:"},
	{ "rmi",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/midi:"},
	{ "mid",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/midi:"},
	{ "mp1",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/mp1:"},
	{ "mp2",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/mp2:"},
	{ "mp3",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/mpeg:"},
	{ "mp4",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/mp4:"},
	{ "m4a",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/mp4:"},
	{ "ogg",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/x-ogg:"},
	{ "wav",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/wav:"},
	{ "pcm",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/l16:"},
	{ "lpcm", UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/l16:"},
	{ "l16",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/l16:"},
	{ "wma",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/x-ms-wma:"},
	{ "mka",  UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/mpeg:"},
	{ "rm",   UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/x-pn-realaudio:"},
	{ "flac", UPNPCLASS_ITEM_AUDIO, "http-get:*:audio/x-flac:"},

	{ "bmp",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/bmp:"},
	{ "ico",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/x-icon:"},
	{ "gif",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/gif:"},
	{ "jpeg", UPNPCLASS_ITEM_PHOTO, "http-get:*:image/jpeg:"},
	{ "jpg",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/jpeg:"},
	{ "jpe",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/jpeg:"},
	{ "pcd",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/x-ms-bmp:"},
	{ "png",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/png:"},
	{ "pnm",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/x-portable-anymap:"},
	{ "ppm",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/x-portable-pixmap:"},
	{ "qti",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/x-quicktime:"},
	{ "qtf",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/x-quicktime:"},
	{ "qtif", UPNPCLASS_ITEM_PHOTO, "http-get:*:image/x-quicktime:"},
	{ "tif",  UPNPCLASS_ITEM_PHOTO, "http-get:*:image/tiff:"},
	{ "tiff", UPNPCLASS_ITEM_PHOTO, "http-get:*:image/tiff:"},

	{ "m3u", UPNPCLASS_ITEM_PLAYLIST, "http-get:*:audio/mpegurl:"},
	{ "asx", UPNPCLASS_ITEM_PLAYLIST, "http-get:*:video/x-ms-asf:"},

	{"*",UPNPCLASS_ITEM_TEXT,"http-get:*:application/octet-stream:"},

	{ NULL, NULL, NULL}
};

static CESDMServer* g_pDMServer = NULL;
char* CESDMServer::m_myUDN = NULL;

class UDNRelease
{
public:
	UDNRelease(){};
	~UDNRelease()
	{
		if(CESDMServer::m_myUDN)
		{
			Trace("Release DMS UDN = %s\n",CESDMServer::m_myUDN);
			FREEARRAY(CESDMServer::m_myUDN);
		}

	}
};	
static UDNRelease g_UDNRelease;


CESDMServer::CESDMServer()
:m_pAVTransport(NULL),
m_pConnectionManager(NULL),
m_pContentDirectory(NULL),
m_pDeviceMySelf(NULL),
m_IUpnp(NULL),
m_pRootEntries(NULL),
m_systemUpdateId(0),
m_strActive(NULL),
m_nFlag(0),
m_pDlnaDeviceHandler(NULL),
m_pWebVFSObserver(NULL),
m_pWebVFSCb(NULL),
m_fileIndex(0)
{
	g_pDMServer = this;
	AddContentDirectory(VIRTUAL_ROOT);
	m_IUpnp = IUpnp::CreateUpnpDevice();
}
CESDMServer::~CESDMServer()
{

}

const char* GetLastDirName(const char* dirName)
{
	char* pos = strrchr((char*)dirName,'/');
	if(pos == NULL)
		return NULL;
	return (icstring)(pos+sizeof(char));
}
int CESDMServer::AddContentDirectory(const char* dirName)
{
	if(dirName == NULL)
		return -1;

	QUICKWRITELOCK(&m_containerLock);
	
	if(strcmp(dirName, VIRTUAL_ROOT) == 0 )//compare pointer values directly
	{
		assert(m_pRootEntries == NULL);
		m_pRootEntries = new CDEntryEx;
		m_pRootEntries->upnpclass = UPNPCLASS_CONTAINER;
		m_pRootEntries->title = strdup((const char*)VIRTUAL_ROOT+sizeof(char));
		m_pRootEntries->filepath = strdup(VIRTUAL_ROOT);
		m_pRootEntries->mediaclass = UPNPCLASS_CONTAINER;
		m_pRootEntries->objectid = ++CDEntryEx::refid;
		m_pRootEntries->childcounts = 0;
		Trace("Add dir [%s]\n",dirName);
		m_FileContents.insert(make_pair<i64,CDEntryEx*>(m_pRootEntries->objectid,m_pRootEntries));
		return 0;
	}
	
	struct stat dirstat;
	if(stat(dirName,&dirstat) != 0)
		return -1;
	if(!S_ISDIR(dirstat.st_mode))
		return -1;
	
	string strexpand = dirName;
	if(dirName[strlen(dirName)-1] != '/')
		strexpand += "/";
		
	if(IsDirExist(strexpand.c_str()) != NULL)
		return -1;
	

	return AddAndParseDir(strexpand.c_str(),m_pRootEntries);
}
CDEntryEx* CESDMServer::IsDirExist(const char* dirname)
{
	CMapEntriesEx::iterator itbegin = m_FileContents.begin();
	CMapEntriesEx::iterator itend = m_FileContents.end();
	
	for(itbegin;itbegin != itend; ++itbegin)
	{
		CDEntryEx *p = (CDEntryEx *)itbegin->second;
		if(!p)
		 {
			continue;
		 }
		if(p->childcounts != -1 && strcmp(p->filepath,dirname) == 0)
			return p;
	}
	return NULL;
}
char* CESDMServer::GetShortName(icstring dirname)
{
	int nl = strlen(dirname);
	bool bsplashend=false;
	if(dirname[nl-1] == '/')
		bsplashend = true;

	if(!bsplashend)
		return strdup(strrchr(dirname,'/')+sizeof(char));
	
	int i  = nl-2;
	for ( i = nl -2; i>=0; i--)
	{
		if(dirname[i] == '/')
			break;
	}
	
	return strndup(dirname + (i+1)*sizeof(char),nl-2-i);

}
int CESDMServer::AddAndParseDir(const char* dirname,CDEntryEx* pParentEntry)
{
	CDEntryEx *pNewEntries = new CDEntryEx;

	pNewEntries->parentid = pParentEntry->objectid;
	pNewEntries->objectid = ++CDEntryEx::refid;
	pNewEntries->title = GetShortName(dirname);
	pNewEntries->upnpclass = UPNPCLASS_CONTAINER_STORAGE;//UPNPCLASS_CONTAINER;
	pNewEntries->mediaclass = UPNPCLASS_CONTAINER_STORAGE;//UPNPCLASS_CONTAINER;
	pNewEntries->childcounts = 0;

	typedef list<CDEntryEx*>CListEntriesEx;
	CListEntriesEx tmpListEntriesEx;
	tmpListEntriesEx.push_back(pNewEntries);
	pNewEntries->filepath = strdup(dirname);
	int nAddedFlag = -1;
	#if defined(OS_ANDROID) || defined(PLATFORM_64)
	typedef int (* COMPAREFUNC)(const dirent**, const dirent**);
	#else
	typedef int (* COMPAREFUNC)(const void*, const void*);
	#endif	
	while (tmpListEntriesEx.size())
	{
		CDEntryEx* pTemp = tmpListEntriesEx.front();
		tmpListEntriesEx.pop_front();
		
		struct dirent **pDirNameList = NULL;
		
		int n = scandir(pTemp->filepath,&pDirNameList,NULL,(COMPAREFUNC)alphasort);
		if(n <0)
		{
			if(nAddedFlag == -1)
			{
				nAddedFlag = 0;
			}
			DELETE(pTemp);
			continue;
		}
		if(nAddedFlag == -1)
		{
			nAddedFlag = 1;
		}
		
		
		for (int i = 0; i < n; i++)
		{
			if(pDirNameList[i]->d_name[0] == '.')
			{
				FREE(pDirNameList[i]);
				continue;
		   }

			icstring dname = (const char*)pDirNameList[i]->d_name;
			string absolutepath = pTemp->filepath;
			absolutepath += dname;
			
			struct stat statinfo;
		
			if(stat(absolutepath.c_str(),&statinfo) == 0)
			{
				++pTemp->childcounts;

				istring title=NULL,postfix=NULL;

				postfix = strrchr((char*)dname,'.');
				if(postfix == NULL)
					title = strdup(dname);
				else
				{
					title = strndup(dname,postfix-dname);
					postfix = strdup((postfix+sizeof(char)));
				}
				
				if(S_ISDIR(statinfo.st_mode))
				{
					CDEntryEx* pNewContent = new CDEntryEx;
					pNewContent->parentid = pTemp->objectid;
					pNewContent->objectid = ++CDEntryEx::refid;
					
					pTemp->childids.push_back(pNewContent->objectid);

					pNewContent->title = title;
					absolutepath += "/";
					pNewContent->filepath = strdup(absolutepath.c_str());
					pNewContent->childcounts = 0;
					pNewContent->upnpclass = UPNPCLASS_CONTAINER_STORAGE;//UPNPCLASS_CONTAINER;
					pNewContent->mediaclass = UPNPCLASS_CONTAINER_STORAGE;//UPNPCLASS_CONTAINER;
					
					tmpListEntriesEx.push_back(pNewContent);
				}
				else
				{
					
					icstring filepath = absolutepath.c_str();
					int readable = 0;
					if(access(filepath,R_OK) < 0)
					{
						if(errno != EACCES)
						{
							Trace("Can not access file [%s]\n",filepath);
							continue;
						}
					}
					else
						readable = 1;
					
					CDEntryEx* pChildFile = new CDEntryEx;
					
					pChildFile->filepath = strdup(absolutepath.c_str());
					pChildFile->postfix = postfix;
					pChildFile->title = title;
					pChildFile->parentid = pTemp->objectid;
					pChildFile->objectid = ++CDEntryEx::refid;
					
					pChildFile->readable = readable;
					pChildFile->length = statinfo.st_size;
					pChildFile->lastmodified = statinfo.st_mtime;

					pTemp->childids.push_back(pChildFile->objectid);

					Trace("Add info title=[%s],path=[%s],id=[%lld],pid=[%lld]\n",
						pChildFile->title,pChildFile->filepath,pChildFile->objectid,pChildFile->parentid);
					
					if(m_IUpnp)
						AddEntryResoureInfo(pChildFile);

					m_FileContents.insert(make_pair<i64,CDEntryEx*>(pChildFile->objectid,pChildFile));


					
				}
			}	

			
			FREE(pDirNameList[i]);
		}

		FREEARRAY(pDirNameList);
		
		Trace("Add directory title=[%s],path=[%s],id=[%lld],pid=[%lld],child=[%d]\n",
					pTemp->title,pTemp->filepath,pTemp->objectid,pTemp->parentid,pTemp->childcounts);
		
		m_FileContents.insert(make_pair<i64,CDEntryEx*>(pTemp->objectid,pTemp));
	
	}
	
	if(nAddedFlag == 1)
	{
		++pParentEntry->childcounts;
		pParentEntry->childids.push_back(pNewEntries->objectid);
	}

	Trace("AddAndParseDir %d",m_FileContents.size());
	return 0;
}
void CESDMServer::AddEntryResoureInfo()
{
	QUICKWRITELOCK(&m_containerLock);
	CMapEntriesEx::iterator itbegin = m_FileContents.begin();
	CMapEntriesEx::iterator itend = m_FileContents.end();

	for (itbegin; itbegin != itend; ++itbegin)
	{
		CDEntryEx* pEntry = itbegin->second;
		if(pEntry && pEntry->childcounts<0 && pEntry->rescounts == 0)
		{
			AddEntryResoureInfo(pEntry);
		}
	}
	Trace("AddEntryResoureInfo %d",m_FileContents.size());
	return;
}
void CESDMServer::AddEntryResoureInfo(CDEntryEx* pChildFile)
{

	pChildFile->rescounts = 1;
	pChildFile->entryres = new EntryResourceInfo*[1];
	EntryResourceInfo* pTempResInfo = pChildFile->entryres[0] = new EntryResourceInfo;
	pTempResInfo->contenturi = (char*)malloc(CONTENT_URL_LENGTH);
	memset(pTempResInfo->contenturi,0,CONTENT_URL_LENGTH);
	
	snprintf(pTempResInfo->contenturi,CONTENT_URL_LENGTH,
			"http://%s:%d%s/%08lld_%s.%s",
			m_IUpnp->GetUpnpIp(),m_IUpnp->GetUpnpPort(),DLNA_WEB_SERVER_ROOTDIR,
			pChildFile->objectid,pChildFile->title,pChildFile->postfix);
	
	if(pChildFile->postfix == NULL)
	{
		pChildFile->mediaclass = UPNPCLASS_ITEM_TEXT;
		pChildFile->entryres[0]->protocolinfo = strdup("http-get:*:application/octet-stream:");

		return;
	}

	MimeType *pIterBegin = MimeTypeAll;
	while (pIterBegin->postfix)
	{
		if(strcasecmp(pIterBegin->postfix,pChildFile->postfix) == 0)
		{
			pChildFile->mediaclass = pIterBegin->mediaclass;
			pChildFile->entryres[0]->protocolinfo = strdup(pIterBegin->protocolinfo);
			break;
		}
		if(pIterBegin->postfix[0]=='*')
		{
			pChildFile->mediaclass = pIterBegin->mediaclass;
			pChildFile->entryres[0]->protocolinfo = strdup(pIterBegin->protocolinfo);
			break;
		}

		++pIterBegin;
	}
	
	return;
	
		
	
}

int CESDMServer::RemoveContentDirectory(const char* dirName)
{
	if(dirName == NULL)
		return -1;

	QUICKWRITELOCK(&m_containerLock);


	struct stat dirstat;
	if(stat(dirName,&dirstat) != 0)
		return -1;
	if(!S_ISDIR(dirstat.st_mode))
		return -1;
		
	string strexpand = dirName;
	if(dirName[strlen(dirName)-1] != '/')
		strexpand += "/";
		
	CDEntryEx *pLockedEntry = IsDirExist(strexpand.c_str());
	if(!pLockedEntry)
		return -1;
	
	if(pLockedEntry->childcounts < 0)
		return -1;

	typedef list<CDEntryEx*>CListEntries;
	CListEntries tmpEntries;
	
	tmpEntries.push_back(pLockedEntry);
	CMapEntriesEx::iterator itdel = m_FileContents.end();
	CDEntryEx* pParent = m_FileContents[pLockedEntry->parentid];
	--pParent->childcounts;
	
	pParent->RemoveChildId(pLockedEntry->objectid);

	while (tmpEntries.size())
	{
		CDEntryEx* pTemp = tmpEntries.front();
		tmpEntries.pop_front();

		CListIds::iterator itbegin = pTemp->childids.begin();
		CListIds::iterator itend = pTemp->childids.end();
		for (itbegin; itbegin!=itend; ++itbegin)
		{
			i64 id = (i64)(*itbegin);

			CDEntryEx* pItemDel = m_FileContents[id];
			if(pItemDel->childcounts >=0)
				tmpEntries.push_back(pItemDel);
			else
			{
				itdel = m_FileContents.find(id);
				m_FileContents.erase(itdel);
				Trace("Remove info title=[%s],path=[%s],id=[%lld],pid=[%lld]\n",
					pItemDel->title,pItemDel->filepath,pItemDel->objectid,pItemDel->parentid);
				DELETE(pItemDel);
			}
		}
		
		itdel = m_FileContents.find(pTemp->objectid);
		m_FileContents.erase(itdel);
		Trace("Remove dir title=[%s],path=[%s],id=[%lld],pid=[%lld],counts=[%d]\n",
			pTemp->title,pTemp->filepath,pTemp->objectid,pTemp->parentid,pTemp->childcounts);
		DELETE(pTemp);
	}
	return 0;
}

void CESDMServer::GetIdFromFileName(const char* filename,i64 & idout)
{
	idout = -1LL;
	char* punderscore = strrchr((char*)filename,'_');
	if(punderscore == NULL)
		return;
	char* psplash = strrchr((char*)filename,'/');
	if(psplash == NULL)
		return ;
	istring strid = strndup((psplash + sizeof(char)), punderscore-psplash-1);
	idout = atol(strid);
	FREEARRAY(strid);
	return;
}
int CESDMServer::RegisterDMServer(const char* content,BOOL bFromLocalFile,icstring friendlyName /*= NULL*/)
{
	if(!m_IUpnp)
		return -1;

	

	ERROR_TYPE errType = ERROR_SUCCESS;
	
	CRegisterTypeRootDevice2 registerType;
	registerType.descriptionType = bFromLocalFile? REG_FILENAME_DESC:REG_BUF_DESC;
	registerType.description = content;
	
	if(friendlyName == NULL)
		friendlyName = EASYDLNA_DMS_FRIENDLYNAME;

	if(bFromLocalFile == FALSE && content == NULL)
	{
		char* udn = NULL;
		if(m_myUDN)
			udn = m_myUDN;
		else
			udn = GetUDNByRunTime(EASYDLNA_BASEUUID);

		if(udn == NULL)
			udn = strdup(EASYDLNA_UUID);

		int totallen = (strlen(ES_SERVER_DESCRIPTION) + strlen(friendlyName)  +64)*sizeof(char);
		char* description = new char [totallen];
		memset(description,0,totallen);

		snprintf(description,totallen,ES_SERVER_DESCRIPTION,friendlyName,udn);

		registerType.description =  description;
		registerType.bufferLen = totallen;
		registerType.config_baseURL = 1;

		errType = m_IUpnp->Register(registerType);

		DELETEARRAY(description);

		if(!m_myUDN)
			m_myUDN = udn;
	}
	else
	{
		errType = m_IUpnp->Register(registerType);
	}

	if(errType == ERROR_SUCCESS)
	{
		UpnpVirtualDirCallbacks callbacks = 
		{
			CESDMServer::HTTPGetFileInfo,
			CESDMServer::HTTPOpenFile,
			CESDMServer::HTTPFileRead,
			CESDMServer::HTTPFileWrite,
			CESDMServer::HTTPFileSeek,
			CESDMServer::HTTPFileClose
		}; 

		IUpnp::EnableWebserver(TRUE);
		IUpnp::SetVirtualDirCallbacks(&callbacks);
		IUpnp::AddVirtualDir(DLNA_WEB_SERVER_ROOTDIR);
		
		if(!m_pDlnaDeviceHandler)
		{
			m_pDlnaDeviceHandler = new CESDlnaDeviceHandler;
			m_nFlag |= 0x00000001;
		}

		m_pDlnaDeviceHandler->SetUpnpDevice(m_IUpnp);

		if(!m_pAVTransport)
		{
			m_pAVTransport = new CESAVTransport();
			m_nFlag |=0x00000010;
		}
		if(!m_pConnectionManager)
		{
			m_pConnectionManager = new CESConnetionManager();
			m_nFlag |= 0x00000100;
		}
		if(!m_pContentDirectory)
		{
			m_pContentDirectory = (IDlnaContentDirectory*)(this);
		}
		if(!m_pWebVFSCb)
		{
			m_pWebVFSCb = (IWebVFSCallbacks*)(this);
		}
		if(m_pAVTransport)
		{
			m_pAVTransport->SetActive(m_myUDN);
			m_pDlnaDeviceHandler->SetAVTransport(m_pAVTransport);
		}
		if(m_pConnectionManager)
		{
			m_pConnectionManager->SetActive(m_myUDN);
			m_pDlnaDeviceHandler->SetConnectionManager(m_pConnectionManager);
		}
		if(m_pContentDirectory)
		{
			m_pContentDirectory->SetActive(m_myUDN);
			m_pDlnaDeviceHandler->SetContentDirectory(m_pContentDirectory);
									
		}

		m_IUpnp->AddUpnpDeviceObserver((IUpnpDeviceObserver*)this);
		
		AddEntryResoureInfo();
		
		Trace("ESDMServer:Register success\n");
	}
	else
	{
		IUpnp::DestroyIUpnp(m_IUpnp);
		m_IUpnp = NULL;

		Trace(
			"ESDMSender:Register failed [%s]\n",IUpnpUtil::GetErrorMessage(errType));
	}
	return (int)errType;
}
int CESDMServer::Broadcast(int TimeOut)
{
	if(!m_IUpnp)
		return -1;
	CCommandSendAdvertisement cmdSendAdvertise(TimeOut);
	return (int)m_IUpnp->ExecuteCommand(cmdSendAdvertise);
}
int CESDMServer::Release()
{
	IUpnp::RemoveVirtualDir(DLNA_WEB_SERVER_ROOTDIR);
	if(m_nFlag & 0x00000001)
		DELETE(m_pDlnaDeviceHandler);
	if(m_nFlag & 0x00000010)
		DELETE(m_pAVTransport );
	if(m_nFlag & 0x00000100)
		DELETE(m_pConnectionManager);
	
	//FREEARRAY(m_myUDN);
	FREEARRAY(m_strActive);

	CMapEntriesEx::iterator itbegin = m_FileContents.begin();
	CMapEntriesEx::iterator itend = m_FileContents.end();
	for (itbegin; itbegin != itend; ++itbegin)
	{
		CDEntryEx *pEn = (CDEntryEx*)itbegin->second;
		DELETE(pEn);
	}
	m_FileContents.clear();
	CDEntryEx::refid = -1;
	CMapFiles::iterator itbeginF = m_Files.begin();
	CMapFiles::iterator itendF = m_Files.begin();
	for (itbeginF; itbeginF != itendF; ++itbeginF)
	{
		CFileInfo *pFile = (CFileInfo*)itbeginF->second;
		DELETE(pFile);
	}
	m_Files.clear();
	
	if(m_IUpnp)
	{
		m_IUpnp->RemoveUpnpDeviceObserver((IUpnpDeviceObserver*)this);	
		m_IUpnp->UnRegister();
		IUpnp::DestroyIUpnp(m_IUpnp);
	}
	Trace("ESDMServer Release succeed\n");
	delete(this);
	return 0;
}

IUpnp* CESDMServer::QueryUpnp()
{
	return m_IUpnp;
}


const char* CESDMServer::GetContentTypeFromProtocolInfo(icstring protocolinfo,int & nlength)
{
	char* lastdot = strrchr((char*)protocolinfo,'.');
	char* secondlastdot = strchr((char*)protocolinfo,'*') + 1;

	nlength = lastdot-secondlastdot;
	int beginpos = secondlastdot - protocolinfo  + 1;
	return protocolinfo+beginpos;

}
int CESDMServer::GetFileInfo (IN const char *filename,OUT struct File_Info *info)
{
	i64 id = -1;
	GetIdFromFileName(filename,id);
	if(id == -1)
		return -1;

	QUICKREADLOCK(&m_containerLock);
	CMapEntriesEx::iterator itfind = m_FileContents.find(id);
	if(itfind != m_FileContents.end())
	{
		CDEntryEx * pEntryRead = itfind->second;
		if(pEntryRead == NULL)
			return -1;

		info->file_length = pEntryRead->length;
		info->last_modified = pEntryRead->lastmodified;
		info->is_directory = pEntryRead->childcounts<0?0:1;
		info->is_readable = pEntryRead->readable;

		int ncpylen = 0;
		icstring contenttype = 
			GetContentTypeFromProtocolInfo(pEntryRead->entryres[0]->protocolinfo,ncpylen);

		info->content_type = strndup(contenttype,ncpylen);

		return 0;
	}
	else
		return -1;
	


}

UpnpWebFileHandle CESDMServer::FileOpen(IN const char *filename,IN enum UpnpOpenFileMode Mode)
{

	i64 id = -1;
	GetIdFromFileName(filename,id);
	if(id == -1)
		return NULL;
	
	QUICKWRITELOCK(&m_containerLock);
	CMapEntriesEx::iterator itfind = m_FileContents.find(id);
	if(itfind == m_FileContents.end())
		return NULL;
	CDEntryEx * pEntryRead = itfind->second;

	int fd = open(pEntryRead->filepath,Mode==UPNP_READ?O_RDONLY:O_WRONLY);

	if(fd == -1)
	{
		Trace("Cannot open file [%s]\n",pEntryRead->filepath);
		return NULL;
	}
	
	CFileInfo* pFileInfo = new CFileInfo;
	pFileInfo->id = id;
	pFileInfo->length = pEntryRead->length;
	pFileInfo->index = ++m_fileIndex;
	m_Files.insert(make_pair<int,CFileInfo*>(fd,pFileInfo));

	if(m_pWebVFSObserver)
	{

		WebFileInfo webFileInfo;
		webFileInfo.index = pFileInfo->index;
		webFileInfo.url = /*strdup*/(pEntryRead->entryres[0]->contenturi);
		webFileInfo.realpath = /*strdup*/(pEntryRead->filepath);
		webFileInfo.readpos =  0;
		webFileInfo.filesize = pEntryRead->length;
		m_pWebVFSObserver->Nortify(webFileInfo);
	}

	return (UpnpWebFileHandle)MAKE_FD64(fd);
	
}
int CESDMServer::FileRead(IN UpnpWebFileHandle fileHnd,OUT char *buf,IN size_t buflen)
{
	int fd = PARSE_FD64(fileHnd);

	CMapFiles::iterator itfindFile = m_Files.find(fd);
	if(itfindFile == m_Files.end())
	{
		Trace("Fail to read file on fd=[%d]\n",fd);
		return -1;				
	}
	CFileInfo *pFileInfo =itfindFile->second;

	int nret = read(fd,buf,buflen);
	if(nret == -1)
	{
		Trace("Read file error on [%d]\n",fd);
		return -1;
	}

	pFileInfo->offset += nret;

	if(m_pWebVFSObserver)
		{
			CMapEntriesEx::iterator itfind = m_FileContents.find(pFileInfo->id);
			if(itfind != m_FileContents.end())
			{
				CDEntryEx * pEntryRead = itfind->second;

				WebFileInfo webFileInfo;
				webFileInfo.index = pFileInfo->index;
				webFileInfo.url = /*strdup*/(pEntryRead->entryres[0]->contenturi);
				webFileInfo.realpath = /*strdup*/(pEntryRead->filepath);
				webFileInfo.readpos = pFileInfo->offset;
				webFileInfo.filesize = pEntryRead->length;
				//Trace("%s read pos = %lld,index=%d\n",
				 //     pEntryRead->filepath,pFileInfo->offset,pFileInfo->index);
				m_pWebVFSObserver->Nortify(webFileInfo);
			}
			
		}

	
	return nret;

}
int CESDMServer::FileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen)
{
	Trace("Write file error : does not support file write\n");
	return -1;
}
int CESDMServer::FileSeek(IN UpnpWebFileHandle fileHnd,IN off_t offset,IN int origin)
{
	int fd = PARSE_FD64(fileHnd);

	CMapFiles::iterator itfindFile = m_Files.find(fd);
	if(itfindFile == m_Files.end())
	{
		Trace("Fail to read file on fd=[%d]\n",fd);
		return -1;				
	}
	CFileInfo *pFileInfo =itfindFile->second;

	off_t lret = lseek(fd,offset,origin);
	if(lret == -1)
	{
		Trace("Seek file fails on fd = [%d]\n",fd);
		return -1;
	}

	pFileInfo->offset += lret;




	return 0;
}
int CESDMServer::FileClose(IN UpnpWebFileHandle fileHnd)
{
	int fd = PARSE_FD64(fileHnd);

	QUICKWRITELOCK(&m_containerLock);

	CMapFiles::iterator itfind = m_Files.find(fd);
	if(itfind == m_Files.end())
	{
		Trace("Fail to Close file on fd=[%d]\n",fd);
		return -1;
	}

	CFileInfo *pFileInfo = (CFileInfo*)itfind->second;
	
	close(fd);
	m_Files.erase(itfind);
	if(m_pWebVFSObserver)
	{
		CMapEntriesEx::iterator itfind = m_FileContents.find(pFileInfo->id);
		if(itfind != m_FileContents.end())
		{
			CDEntryEx * pEntryRead = itfind->second;

			WebFileInfo webFileInfo;
			webFileInfo.index = pFileInfo->index;
			webFileInfo.url = /*strdup*/(pEntryRead->entryres[0]->contenturi);
			webFileInfo.realpath = /*strdup*/(pEntryRead->filepath);
			webFileInfo.readpos = pFileInfo->offset;
			webFileInfo.filesize = pEntryRead->length;
			webFileInfo.closed = 1;
			m_pWebVFSObserver->Nortify(webFileInfo);
		}
		


	}

	DELETE(pFileInfo);

	return 0;
}
int CESDMServer::inner_HTTPGetFileInfo (IN const char *filename,OUT struct File_Info *info)
{
	if(strcmp(filename,DMS_AVT_SCPDURL) == 0)
	{
		info->file_length = AVT_DESCRIPTION_LEN;
		info->last_modified = (time_t)0;
		info->is_directory =0;
		info->is_readable = 1;
		info->content_type = ixmlCloneDOMString(CONTENT_TYPE);
	}
	else if(strcmp(filename,DMS_CMS_SCPDURL) == 0)
	{
		info->file_length = CMS_DESCRIPTION_LEN;
		info->last_modified = (time_t)0;
		info->is_directory = 0;
		info->is_readable = 1;
		info->content_type =ixmlCloneDOMString(CONTENT_TYPE);
	}
	else if(strcmp(filename,DMS_CDS_SCPDURL) == 0)
	{
		info->file_length = CDS_DESCRIPTION_LEN;
		info->last_modified = (time_t)0;
		info->is_directory = 0;
		info->is_readable = 1;
		info->content_type =ixmlCloneDOMString(CONTENT_TYPE);
	}
	else
	{
		if(!m_pWebVFSCb)
			return -1;
		return m_pWebVFSCb->GetFileInfo(filename,info);	
	}

	return 0;
}
UpnpWebFileHandle CESDMServer::inner_HTTPOpenFile(IN const char *filename,IN enum UpnpOpenFileMode Mode)
{
	
	static int half_fd = 0x0000FFFF;
	if(strcmp(filename,DMS_AVT_SCPDURL) == 0)
	{
		//long getfd = (long)DMS_AVT_SCPDURL;
		int fd = ++half_fd;

		

		//fd = ((fd<<16)>>16) | (half_fd << 16);

		CFileInfo * pFileInfo = new CFileInfo;
		pFileInfo->length = AVT_DESCRIPTION_LEN;
		pFileInfo->buf = AVT_DESCRIPTION;
		QUICKWRITELOCK(&m_containerLock);
		m_Files.insert(make_pair<int,CFileInfo*>(fd,pFileInfo));

		return (UpnpWebFileHandle)MAKE_FD64(fd);
	}
	else if(strcmp(filename,DMS_CMS_SCPDURL) == 0)
	{
		//long getfd = (long)DMS_CMS_SCPDURL;
		//int fd = (int)((getfd<<32)>>32);
		int fd = ++half_fd;

		//fd = ((fd<<16)>>16)| (half_fd << 16);

		CFileInfo * pFileInfo = new CFileInfo;
		pFileInfo->length = CMS_DESCRIPTION_LEN;
		pFileInfo->buf = CMS_DESCRIPTION;
		QUICKWRITELOCK(&m_containerLock);
		m_Files.insert(make_pair<int,CFileInfo*>(fd,pFileInfo));

		return (UpnpWebFileHandle)MAKE_FD64(fd);
	}
	else if(strcmp(filename,DMS_CDS_SCPDURL) == 0)
	{
		//long getfd = (long)DMS_CDS_SCPDURL;
		//int fd = (int)((getfd<<32)>>32);
		int fd = ++half_fd;
		//++half_fd;

		//fd = ((fd<<16)>>16) | (half_fd << 16);

		CFileInfo * pFileInfo = new CFileInfo;
		pFileInfo->length = CDS_DESCRIPTION_LEN;
		pFileInfo->buf = CDS_DESCRIPTION;
		QUICKWRITELOCK(&m_containerLock);
		m_Files.insert(make_pair<int,CFileInfo*>(fd,pFileInfo));

		return (UpnpWebFileHandle)MAKE_FD64(fd);
	}
	else
	{
		if(!m_pWebVFSCb)
			return NULL;
		return m_pWebVFSCb->FileOpen(filename,Mode);
	}
}
int CESDMServer::inner_HTTPFileRead(IN UpnpWebFileHandle fileHnd,OUT char *buf,IN size_t buflen)
{
	int fd = (int)PARSE_FD64(fileHnd);

	int nret = -1;
		
	if(fd > 0x0000FFFF)
	{
		QUICKREADLOCK(&m_containerLock);

		CMapFiles::iterator itfindFile = m_Files.find(fd);
		if(itfindFile == m_Files.end())
		{
			Trace("Fail to read file on fd=[%d]\n",fd);
			return -1;				
		}
		CFileInfo *pFileInfo =itfindFile->second;
		
		
		if( pFileInfo->offset >= pFileInfo->length)
			return 0;

		int leftsize = pFileInfo->length - pFileInfo->offset;

		buflen = buflen<leftsize?buflen:leftsize;

		const char* content = pFileInfo->buf;

		memcpy(buf,content+pFileInfo->offset,buflen);
		pFileInfo->offset += buflen;

		nret = buflen;		
		
		return nret;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
	}
	else
	{
		if(!m_pWebVFSCb)
			return -1;

		return m_pWebVFSCb->FileRead(fileHnd,buf,buflen);
	}
	
	
	
}
int CESDMServer::inner_HTTPFileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen)
{
	if(!m_pWebVFSCb)
		return -1;
	return m_pWebVFSCb->FileWrite(fileHnd,buf,buflen);
}
int CESDMServer::inner_HTTPFileSeek(IN UpnpWebFileHandle fileHnd,IN off_t offset,IN int origin)
{
	int fd = (int)PARSE_FD64(fileHnd);

	int nret = -1;

	if(fd > 0x0000FFFF)
	{
		QUICKREADLOCK(&m_containerLock);

		CMapFiles::iterator itfindFile = m_Files.find(fd);
		if(itfindFile == m_Files.end())
		{
			Trace("Fail to read file on fd=[%d]\n",fd);
			return -1;				
		}
		CFileInfo *pFileInfo =itfindFile->second;

		if(origin == SEEK_CUR)
		{
			pFileInfo->offset += offset ;

			if(pFileInfo->offset > pFileInfo->length)
				pFileInfo->offset = pFileInfo->length;

		}
		else if(origin == SEEK_END)
		{
			pFileInfo->offset -= offset;
			if(pFileInfo->offset < 0)
				pFileInfo->offset = 0;
		}
		else if(origin == SEEK_SET)
		{
			pFileInfo->offset = offset;
			if(pFileInfo->offset > pFileInfo->length)
				pFileInfo->offset = pFileInfo->length;
			if(pFileInfo->offset < 0)
				pFileInfo->offset = 0;

		}
		return 0;
	}
	else
	{
		if(!m_pWebVFSCb)
			return -1;

		return m_pWebVFSCb->FileSeek(fileHnd,offset,origin);
	}

}
int CESDMServer::inner_HTTPFileClose(IN UpnpWebFileHandle fileHnd)
{
	int fd = (int)PARSE_FD64(fileHnd);

	int nret = -1;

	if(fd > 0x0000FFFF)
	{

		QUICKWRITELOCK(&m_containerLock);

		CMapFiles::iterator itfind = m_Files.find(fd);
		if(itfind == m_Files.end())
		{
			Trace("Fail to Close file on fd=[%d]\n",fd);
			return -1;
		}

		CFileInfo *pFileInfo = (CFileInfo*)itfind->second;

		close(fd);
		m_Files.erase(itfind);

		DELETE(pFileInfo);
		return 0;
	}
	else
	{
		if(!m_pWebVFSCb)
			return -1;
		return m_pWebVFSCb->FileClose(fileHnd);
	}
}

int CESDMServer::HTTPGetFileInfo (IN const char *filename,OUT struct File_Info *info)
{
		return g_pDMServer->inner_HTTPGetFileInfo(filename,info);
}
UpnpWebFileHandle CESDMServer::HTTPOpenFile(IN const char *filename,
											IN enum UpnpOpenFileMode Mode)
{
	return g_pDMServer->inner_HTTPOpenFile(filename,Mode);
}
int CESDMServer::HTTPFileRead(IN UpnpWebFileHandle fileHnd,OUT char *buf,IN size_t buflen)
{
	return g_pDMServer->inner_HTTPFileRead(fileHnd,buf,buflen);
}
int CESDMServer::HTTPFileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen)
{
	return g_pDMServer->inner_HTTPFileWrite(fileHnd,buf,buflen);
}
int CESDMServer::HTTPFileSeek(IN UpnpWebFileHandle fileHnd,IN off_t offset,IN int origin)
{
	return g_pDMServer->inner_HTTPFileSeek(fileHnd,offset,origin);
}
int CESDMServer::HTTPFileClose(IN UpnpWebFileHandle fileHnd)
{
	return g_pDMServer->inner_HTTPFileClose(fileHnd);
}
ERROR_TYPE CESDMServer::Nortify(EventType deviceEventType,CEvent & deviceEvent)
{
	switch(deviceEventType)
	{
	case ET_CONTROL_ACTION_REQUEST:
		{
			CEventActionRequest & actionReq = static_cast<CEventActionRequest &>(deviceEvent);

			if(strcmp(actionReq.DevUDN,m_myUDN) || actionReq.ErrCode != UPNP_E_SUCCESS)
			{
				Trace(
					"Received an action but udn is not the same.[%d]\n",actionReq.ErrCode);
				return ERROR_SUCCESS;
			}

			return m_pDlnaDeviceHandler->HandleDlnaActionRequest(actionReq);

		}
		break;
	case ET_CONTROL_GET_VAR_REQUEST:
		{
			CEventStateVarRequest & eventGetVar = static_cast<CEventStateVarRequest &>(deviceEvent);
			if(strcmp(eventGetVar.DevUDN,m_myUDN) || 
				eventGetVar.ErrCode != UPNP_E_SUCCESS)
			{
				Trace(
					"Received an GetStateVar Request but udn is not the same.[%d]\n",eventGetVar.ErrCode);
				return ERROR_SUCCESS;
			}
			return m_pDlnaDeviceHandler->HandleDlnaGetVarRequest(eventGetVar);
		}
		break;
	case ET_EVENT_SUBSCRIPTION_REQUEST:
		{
			CEventSubscriptionRequest& eventSubsReq = static_cast<CEventSubscriptionRequest&>(deviceEvent);

			if(strcmp(eventSubsReq.UDN,m_myUDN) != 0)
				return ERROR_SUCCESS;
			return m_pDlnaDeviceHandler->HandleDlnaSubRequest(eventSubsReq);

		}
		break;
	default:
		break;
	}

	return ERROR_SUCCESS;
}

int CESDMServer::GetSearchCapabilities(istring & searchcaps)
{
	searchcaps = strdup(EMPTY_VALUE);
	return 0;
}

int CESDMServer::GetSortCapabilities(istring & sortcaps)
{
	sortcaps = strdup(EMPTY_VALUE);
	return 0;
}

int CESDMServer::GetSystemUpdateID(ui4 & id)
{
	id = m_systemUpdateId;
	return 0;
}
void AppendContainerResult(string& strresult,CDEntryEx* pEntry)
{
	char tempbuf[1024] = {'\0'};

	snprintf(tempbuf,1024,CONTAINER_HEAD,pEntry->objectid,pEntry->parentid,pEntry->childcounts);
	strresult += (const char*)tempbuf;

	memset(tempbuf,0,1024);

	snprintf(tempbuf,1024,TITLE_ITEM,pEntry->title);
	strresult += (const char*)tempbuf;
	memset(tempbuf,0,1024);

	snprintf(tempbuf,1024,CLASS_ITEM,pEntry->upnpclass);
	strresult += (const char*)tempbuf;

	strresult += CONTAINER_END;

}
void AppendItemResult(string& strresult,CDEntryEx* pEntry)
{
	char tempbuf[1024] = {'\0'};

	snprintf(tempbuf,1024,ITEM_HEAD,pEntry->objectid,pEntry->parentid);
	strresult += (const char*)tempbuf;

	memset(tempbuf,0,1024);

	snprintf(tempbuf,1024,TITLE_ITEM,pEntry->title);
	strresult += (const char*)tempbuf;
	memset(tempbuf,0,1024);

	snprintf(tempbuf,1024,CLASS_ITEM,pEntry->mediaclass);
	strresult += (const char*)tempbuf;

	memset(tempbuf,0,1024);
	if(pEntry->rescounts != 0)
	{
		snprintf(tempbuf,1024,RES_ITEM,
			pEntry->entryres[0]->protocolinfo,(i64)pEntry->length,pEntry->entryres[0]->contenturi);
		strresult += (const char*)tempbuf;
	}

	strresult += ITEM_END;
}

int CESDMServer::Browse(icstring id,icstring browseflag,icstring filter,
				   ui4 startingindex,ui4 requestedcount,icstring sort,
				   istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid)
{
	if(sort && strcmp(sort,EMPTY_VALUE) != 0)
		return 709;
	//if(filter&&strcmp(filter,"*") != 0)
	//	return 501;
		
	int nBrowseFlag = -1;
	if(strcmp(browseflag,"BrowseMetadata") == 0)
	{
		nBrowseFlag = 0;
		if(startingindex != 0)
			return 402;
		numberreturned = 1;
	}
	else if(strcmp(browseflag,"BrowseDirectChildren") == 0)
		nBrowseFlag = 1;
	else
		return 402;
	
	i64 fileid = atoll(id);
		
	QUICKREADLOCK(&m_containerLock);
	
	CMapEntriesEx::iterator itfind = m_FileContents.find(fileid);
	if(itfind == m_FileContents.end())
		return 701;

	CDEntryEx* pFindEntry = itfind->second;

	if(nBrowseFlag == 0)
	{
		
		string strresult;
		strresult += DIDL_HEAD;
		if(pFindEntry->childcounts >= 0) //container
		{
			AppendContainerResult(strresult,pFindEntry);
		}
		else
		{
			AppendItemResult(strresult,pFindEntry);
		}
		
		strresult += DIDL_END;

		result = strdup(strresult.c_str());
		totalmatches = 0;
		updateid = m_systemUpdateId;
	}
	else
	{
		if(pFindEntry->childcounts < 0)
			return 501;
		
		string strresult;
		strresult += DIDL_HEAD;
		
		CListIds::iterator itbegin = pFindEntry->childids.begin();
		CListIds::iterator itend = pFindEntry->childids.end();
		
		for (itbegin; itbegin != itend; ++itbegin)
		{
			i64 child = *(itbegin);
			
			if(startingindex > child)
				continue;

			CDEntryEx *pChild = m_FileContents[child];
			if(!pChild)
			{
				assert(0);
				continue;
			}
			
			if(pChild->childcounts<0)
			{
				AppendItemResult(strresult,pChild);
			}
			else
			{
				AppendContainerResult(strresult,pChild);
			}
			++numberreturned;
			if(--requestedcount == 0)
				break;
		}
		
		strresult += DIDL_END;
		result = strdup(strresult.c_str());
		totalmatches = numberreturned;
		updateid = m_systemUpdateId;
	}

	return 0;
}





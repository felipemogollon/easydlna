#include "CESDMRender.h"
#include "ESResouce.h"
#include "CESDlnaDeviceHandler.h"
#include "ESDlnaHelper.h"
#include "CESRenderingControl.h"
#include "CESConnectionManager.h"
#include "CESAVTransport.h"


#define EASYDLNA_UUID "uuid:F7CA5454-3F48-4390-8009-03CB907BA09D"
#define EASYDLNA_BASEUUID "uuid:F7CA5454-3F48-4390-8009"

#define EASYDLNA_DMR_FRIENDLYNAME "EasyDlna Media Renderer"


IUpnpUtil::CRWLock CESDMRender::m_fileLock;
CFileList CESDMRender::m_FileList;

char*  CESDMRender::m_myUDN = NULL;
class UDNRelease
{
public:
	UDNRelease(){};
	~UDNRelease()
	{
		if(CESDMRender::m_myUDN)
		{
			Trace("Release DMR UDN = %s\n",CESDMRender::m_myUDN);
			FREEARRAY(CESDMRender::m_myUDN);
		}

	}
};
static UDNRelease g_UDNRelease;

CESDMRender::CESDMRender()
:m_pAVTransport(NULL),
m_pConnectionManager(NULL),
m_pRenderingControl(NULL),
m_pDeviceMySelf(NULL),
m_IUpnp(NULL),
m_pDlnaDeviceHandler(NULL),
m_nFlags(0)
{
	m_IUpnp = IUpnp::CreateUpnpDevice();
}
CESDMRender::~CESDMRender()
{
}



int CESDMRender::RegisterDMRender(icstring content,BOOL bFromLocalFile,icstring friendlyName /*= NULL*/)
{
	if(!m_IUpnp)
		return -1;

	
	
	ERROR_TYPE errType = ERROR_SUCCESS;

	CRegisterTypeRootDevice2 registerType;
	registerType.descriptionType = bFromLocalFile? REG_FILENAME_DESC:REG_BUF_DESC;
	registerType.description = content;
	
	if(friendlyName == NULL)
		friendlyName = EASYDLNA_DMR_FRIENDLYNAME;

	if(bFromLocalFile == FALSE && content == NULL)
	{
		char* udn = NULL;
		if(m_myUDN)
			udn = m_myUDN;
		else
			udn = GetUDNByRunTime(EASYDLNA_BASEUUID);
		
		if(udn == NULL)
			udn = strdup(EASYDLNA_UUID);

		int totallen = (strlen(ES_RENDER_DESCRIPTION) + strlen(friendlyName) + 64)*sizeof(char);
		char* description = new char [totallen];
		memset(description,0,totallen);

		snprintf(description,totallen,ES_RENDER_DESCRIPTION,friendlyName,udn);
		
		registerType.description =  description;
		registerType.bufferLen = totallen;
		registerType.config_baseURL = 1;
		
		errType = m_IUpnp->Register(registerType);

		DELETEARRAY(description);
		if(m_myUDN == NULL)
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
			CESDMRender::HTTPGetFileInfo,
			CESDMRender::HTTPOpenFile,
			CESDMRender::HTTPFileRead,
			CESDMRender::HTTPFileWrite,
			CESDMRender::HTTPFileSeek,
			CESDMRender::HTTPFileClose
		}; 
		
		IUpnp::EnableWebserver(TRUE);
		IUpnp::SetVirtualDirCallbacks(&callbacks);
		IUpnp::AddVirtualDir(DLNA_WEB_RENDER_ROOTDIR);
		
		if(!m_pDlnaDeviceHandler)
		{
			m_pDlnaDeviceHandler = new CESDlnaDeviceHandler;
			m_nFlags |= 0x00000001;
		}

		m_pDlnaDeviceHandler->SetUpnpDevice(m_IUpnp);

		if(!m_pAVTransport)
		{
			m_pAVTransport = new CESAVTransport();
			m_nFlags |= 0x00000010;
		}
		if(!m_pConnectionManager)
		{
			m_pConnectionManager = new CESConnetionManager();
			m_nFlags |= 0x00000100;
		}
		if(!m_pRenderingControl)
		{
			m_pRenderingControl = new CESRenderingControl();
			m_nFlags |= 0x00001000;
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
		if(m_pRenderingControl)
		{
			m_pRenderingControl->SetActive(m_myUDN);
			m_pDlnaDeviceHandler->SetRenderingControl(m_pRenderingControl);
		}
		
		m_IUpnp->AddUpnpDeviceObserver((IUpnpDeviceObserver*)this);
		Trace("ESDMRender:Register success\n");
	}
	else
	{
		IUpnp::DestroyIUpnp(m_IUpnp);
		m_IUpnp = NULL;

		Trace(
				"ESDMRender:Register failed [%s]\n",IUpnpUtil::GetErrorMessage(errType));
	}
	return (int)errType;
	
}
int CESDMRender::Broadcast(int TimeOut)
{
	if(!m_IUpnp)
		return -1;
	CCommandSendAdvertisement cmdSendAdvertise(TimeOut);
	return (int)m_IUpnp->ExecuteCommand(cmdSendAdvertise);
}
int CESDMRender::Release()
{
	IUpnp::RemoveVirtualDir(DLNA_WEB_RENDER_ROOTDIR);
	if(m_nFlags & 0x00000001)
	{
		DELETE(m_pDlnaDeviceHandler);
	}

	if(m_nFlags & 0x00000010)
	{
		DELETE(m_pAVTransport);
	}
	if(m_nFlags & 0x00000100)
	{
		DELETE(m_pConnectionManager);
	}
	if(m_nFlags & 0x00001000)
	{
		DELETE(m_pRenderingControl);
	}

	//FREEARRAY(m_myUDN);
	if(m_IUpnp)
	{
		m_IUpnp->RemoveUpnpDeviceObserver((IUpnpDeviceObserver*)this);	
		m_IUpnp->UnRegister();
		IUpnp::DestroyIUpnp(m_IUpnp);
		m_IUpnp = NULL;
	}
	
	CFileList::iterator itbegin = m_FileList.begin();
	CFileList::iterator itend = m_FileList.end();

	for (itbegin; itbegin != itend; ++itbegin)
	{
		CFileInfo *ptemp = *itbegin;
		DELETE(ptemp);
	}

	m_FileList.clear();
	Trace("DMRender release success\n");
	delete(this);
	return 0;
	
}
int CESDMRender::HTTPGetFileInfo (const char *filename,struct File_Info *info)
{
	memset(info,0,sizeof(struct File_Info));
	if(strcmp(filename,AVT_SCPDURL) == 0)
	{
		info->file_length = AVT_DESCRIPTION_LEN;
		info->last_modified = (time_t)0;
		info->is_directory =0;
		info->is_readable = 1;
		info->content_type = ixmlCloneDOMString(CONTENT_TYPE);
	}
	else if(strcmp(filename,CMS_SCPDURL) == 0)
	{
		info->file_length = CMS_DESCRIPTION_LEN;
		info->last_modified = (time_t)0;
		info->is_directory = 0;
		info->is_readable = 1;
		info->content_type =ixmlCloneDOMString(CONTENT_TYPE);
	}
	else if(strcmp(filename,RCS_SCPDURL) == 0)
	{
		info->file_length = RCS_DESCRIPTION_LEN;
		info->last_modified = (time_t)0;
		info->is_directory = 0;
		info->is_readable = 1;
		info->content_type =ixmlCloneDOMString(CONTENT_TYPE);
	}
	return 0;
}
UpnpWebFileHandle CESDMRender::HTTPOpenFile(IN const char *filename,IN enum UpnpOpenFileMode Mode)
{
	if(strcmp(filename,AVT_SCPDURL) == 0)
	{
		CFileInfo *pNewFile = new CFileInfo;
		pNewFile->buf = AVT_DESCRIPTION;
		pNewFile->length = AVT_DESCRIPTION_LEN;
		
		QUICKWRITELOCK(&m_fileLock);
		m_FileList.push_back(pNewFile);

		return (UpnpWebFileHandle)pNewFile;
	}
	else if(strcmp(filename,CMS_SCPDURL) == 0)
	{
		CFileInfo *pNewFile = new CFileInfo;
		pNewFile->buf = CMS_DESCRIPTION;
		pNewFile->length = CMS_DESCRIPTION_LEN;
		
		QUICKWRITELOCK(&m_fileLock);
		m_FileList.push_back(pNewFile);

		return (UpnpWebFileHandle)pNewFile;
	}
	else if(strcmp(filename,RCS_SCPDURL) == 0)
	{
		CFileInfo *pNewFile = new CFileInfo;
		pNewFile->buf = RCS_DESCRIPTION;
		pNewFile->length = RCS_DESCRIPTION_LEN;
		
		QUICKWRITELOCK(&m_fileLock);
		m_FileList.push_back(pNewFile);

		return (UpnpWebFileHandle)pNewFile;
	}
	else
		return NULL;
}
int CESDMRender::HTTPFileRead(IN UpnpWebFileHandle fileHnd,OUT char *buf,IN size_t buflen)
{
	CFileInfo *pFileInfo = (CFileInfo*)fileHnd;
	if(!pFileInfo)
		return -1;
	
	if(pFileInfo->offset>= pFileInfo->length)
		return 0;
	
	int leftsize = pFileInfo->length - pFileInfo->offset;

	buflen = buflen<leftsize?buflen:leftsize;
	memcpy(buf,pFileInfo->buf+pFileInfo->offset,buflen);
	
	pFileInfo->offset += buflen;
	
	return buflen;

}
int CESDMRender::HTTPFileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen)
{
	return -1;
}
int CESDMRender::HTTPFileSeek(IN UpnpWebFileHandle fileHnd,IN off_t offset,IN int origin)
{
	CFileInfo* pFileInfo = (CFileInfo*)fileHnd;
	if(!pFileInfo)
		return NULL;

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
int CESDMRender::HTTPFileClose(IN UpnpWebFileHandle fileHnd)
{
	CFileInfo* pFileInfo = (CFileInfo*)fileHnd;
	
	QUICKWRITELOCK(&m_fileLock);
	CFileList::iterator itbegin = m_FileList.begin();
	CFileList::iterator itend = m_FileList.end();

	for (itbegin; itbegin != itend; ++itbegin)
	{
		CFileInfo *ptemp = *itbegin;
		if(ptemp == pFileInfo)
		{
			DELETE(pFileInfo);
			m_FileList.erase(itbegin);
			break;
		}
	}

	return 0;
}
IUpnp*  CESDMRender::QueryUpnp()
{
	return m_IUpnp;
}
ERROR_TYPE CESDMRender::Nortify(EventType deviceEventType,CEvent & deviceEvent)
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











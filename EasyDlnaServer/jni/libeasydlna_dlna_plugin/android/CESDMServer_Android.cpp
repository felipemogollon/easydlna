
#define LOG_TAG "easydlna-esdmserver_android"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "CESDMServer_Android.h"
#include "CESDlna_Android.h"

#include "IDMServer.h"
#include "IDlnaFrame.h"

int start_stop_DMS(JNIEnv* env,jobject jthiz,jboolean jbStart)
{
	if(jbStart != 0)
	{
		Trace("start DMS\n");
		g_pDlnaService->CreateDMS(env);
		return g_pDlnaService->m_pDMS->m_pServer->QueryUpnp()->GetUpnpPort();

	}
	else
	{
		g_pDlnaService->DestroyDMS(env);
	}


	return 0;
}

//int start_remoteUI(JNIEnv* env,jobject jthiz,jboolean jbStart)
//{
//	Trace("startremoteUI: \n");
//
//	CESDMServer_Android* pSever_Android = g_pDlnaService->m_pDMS;
//
//	if(pSever_Android == NULL)
//		return -1;
//
//	pSever_Android->m_pCDS->SetRemoteUIEnabled(jbStart);
//
//	return 0;
//}


static void sendBroadcast(JNIEnv* env,jobject jthiz,jint jtimeOut)
{
	CESDMServer_Android* pSever_Android = g_pDlnaService->m_pDMS;
	if(pSever_Android == NULL || pSever_Android->m_pServer == NULL)
		return ;
	pSever_Android->m_pServer->Broadcast(jtimeOut);
	return;
}

int add_remove_DMSCallback(JNIEnv* env,jobject jthiz,jobject jcallback,jboolean jbAdd)
{
	CESDMServer_Android* pSever_Android = g_pDlnaService->m_pDMS;
	if(pSever_Android == NULL)
		return -1;
	if(jbAdd)
	{
		pSever_Android->m_pCDS->SetCallbackObject(env,jcallback);
		pSever_Android->m_pServer->RegisterDMServer(NULL,NULL,CESDMServer_Android::m_ServerName);
		pSever_Android->m_pServer->Broadcast(900);
	}
	else
	{
		pSever_Android->m_pCDS->SetCallbackObject(env,NULL);
	}

	return 0;
}
void setDMSFriendlyName(JNIEnv* env,jobject jthiz,jstring jstrFriendlyName)
{
	icstring strName = env->GetStringUTFChars(jstrFriendlyName,NULL);
	if(!strName)
		return;

	FREEARRAY(CESDMServer_Android::m_ServerName);
	CESDMServer_Android::m_ServerName = strdup(strName);

	env->ReleaseStringUTFChars(jstrFriendlyName,strName);
	return;
}
void notifyChange(JNIEnv* env,jobject jthiz,jstring jstrChangeValue)
{
	CESDMServer_Android* pSever_Android = g_pDlnaService->m_pDMS;
	if(pSever_Android == NULL)
		return ;

	icstring strChangeValue = env->GetStringUTFChars(jstrChangeValue,NULL);
	if(!strChangeValue)
		return;

	IXML_Document *PropSet = NULL;

	IUpnpUtil::AddToPropertySet(&PropSet,"ContainerUpdateIDs",strChangeValue);

	if(PropSet)
	{
		pSever_Android->m_pCDS->SetPropSet(PropSet);
		//ixmlDocument_free(PropSet);
	}
	else
	{
		Trace("SetProp error\n");
	}

	env->ReleaseStringUTFChars(jstrChangeValue,strChangeValue);
	return;
}
istring CESDMServer_Android::m_ServerName = NULL;

CESDMServer_Android::CESDMServer_Android():
m_pServer(NULL),
m_pCDS(NULL)
{

}
CESDMServer_Android::~CESDMServer_Android()
{
}
int CESDMServer_Android::RegisterDMSService(JNIEnv*env)
{
	JNINativeMethod Methods[] =
	{
		{"native_start_stop_DMS", "(Z)I",(void *)start_stop_DMS},
/*		{"native_startRemoteUI", "(Z)I",(void *)start_remoteUI},*/
		{"native_add_remove_DMSObserver","(Lcom/easydlna/dlna/service/DMSService$IDMSObserver;Z)V",
			(void *)add_remove_DMSCallback},
		{"native_set_DMSFriendlyName","(Ljava/lang/String;)V",(void*)setDMSFriendlyName},
		{"native_notifyChange","(Ljava/lang/String;)V",(void*)notifyChange},
		{"native_sendBroadcast","(I)V",(void*)sendBroadcast},
	};

	return RegisterNativeMethods(env,JAVA_DMSSERVICE_CLASS_NAME,Methods,
				sizeof(Methods)/sizeof(Methods[0]));
}
void  CESDMServer_Android::InitDMS(JNIEnv *env, IDlnaFrame* pDlnaFrame)
{
	m_pServer = (CESDMServer*)pDlnaFrame->GetDMServer("easydlna");
	if(!m_pServer)
	{
		Trace("fail to get server from easydlna");
		return ;
	}
	m_pCDS = new CESContentDirectory_Device_Android(env,(IUpnpDevice*)m_pServer->QueryUpnp());

	m_pServer->SetContentDirectroy((IDlnaContentDirectory*)m_pCDS);
	m_pServer->SetVFSCallbackInterface((IWebVFSCallbacks*)m_pCDS);
	//m_pServer->RegisterDMServer(NULL,NULL,m_ServerName);

	return ;
}
void  CESDMServer_Android::DestroyDMS(JNIEnv *env)
{
	m_pServer->Release();
	m_pServer = NULL;
	m_pCDS->Destroy(env);
	m_pCDS = NULL;
	delete(this);

	return ;
}



jobject CESContentDirectory_Device_Android::m_jCallback = NULL;
jmethodID CESContentDirectory_Device_Android::methIdBrowseMetaData = NULL;
jmethodID CESContentDirectory_Device_Android::methIdBrowseChildren = NULL;
jmethodID CESContentDirectory_Device_Android::methIdGetFileInfo = NULL;

CESContentDirectory_Device_Android::CESContentDirectory_Device_Android(JNIEnv *env,IUpnpDevice * pIUpnpDevice):
m_pIUpnp(pIUpnpDevice),
m_systemUpdateId(0),
m_thizUDN(NULL)/*,
m_bRemoteUiStarted(FALSE)*/
{
	jclass classJavaDMRService  = (jclass)env->FindClass(JAVA_DMSCALLBACK_CLASS_NAME);


	methIdBrowseMetaData = env->GetMethodID(classJavaDMRService,"browseMetaData",
								"(Ljava/lang/String;)Lcom/easydlna/dlna/service/ContentInfo;");

	methIdBrowseChildren = env->GetMethodID(classJavaDMRService,"browseChildren",
								"(Ljava/lang/String;II)[Lcom/easydlna/dlna/service/ContentInfo;");


	methIdGetFileInfo = env->GetMethodID(classJavaDMRService,"getFileInfo",
				"(Ljava/lang/String;)Lcom/easydlna/dlna/service/ContentInfo;");

	//m_pSpecialDevice = new CESSpecialDevice_Android();

}
CESContentDirectory_Device_Android::~CESContentDirectory_Device_Android()
{
	FREEARRAY(m_thizUDN);
	
	CFileHandleList::iterator itbegin = m_fileHandleList.begin();
	CFileHandleList::iterator itend = m_fileHandleList.end();
	for (itbegin; itbegin != itend; ++itbegin)
	{
		int fn = (int)(*itbegin);
		if(fn!=-1)
			close(fn);
	}
	m_fileHandleList.clear();
	//DELETE(m_pSpecialDevice);
}
void CESContentDirectory_Device_Android::SetCallbackObject(JNIEnv*env, jobject jcallback)
{
	if(m_jCallback)
	{
		env->DeleteGlobalRef(m_jCallback);
		m_jCallback = NULL;
	}
	if(jcallback)
		m_jCallback = env->NewGlobalRef(jcallback);
	return;
}
void CESContentDirectory_Device_Android::Destroy(JNIEnv* env)
{
	//m_pSpecialDevice->Close();
	if(m_jCallback)
		env->DeleteGlobalRef(m_jCallback);
	m_jCallback = NULL;
	delete(this);
}

istring CESContentDirectory_Device_Android::GetStatusValue(const char* ValName)
{
	return NULL;
}

int CESContentDirectory_Device_Android::GetSearchCapabilities(istring & searchcaps)
{
	searchcaps = strdup("");
	return 0;
}

int CESContentDirectory_Device_Android::GetSortCapabilities(istring & sortcaps)
{
	sortcaps = strdup("");
	return 0;
}

int CESContentDirectory_Device_Android::GetSystemUpdateID(ui4 & id)
{
	return m_systemUpdateId;
}
int CESContentDirectory_Device_Android::ParseJMetaData(JNIEnv* env,jobject jContentInfo,string & strresult)
{
	int nret = 0;
	MetaDataInfo metaOutInfo;
	icstring id = NULL,parentid = NULL,protocalInfo = NULL,resourceUrl = NULL,thumbnailUrl = NULL,mediaClass = NULL;

	jstring jid = NULL,jparentId = NULL,jtitle = NULL,jmediaClass = NULL,jprotocalInfo = NULL,jthumbnailUrl = NULL,jresourceUrl = NULL,jresourceRtspUrl = NULL;
	jint jChildCount = 0;
	char tempbuf[1024] = {'\0'};

	jid = (jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jId);
	if(jid == NULL)
	{
		nret = 501;
		goto bail;
	}
	metaOutInfo.pmyId = (istring)env->GetStringUTFChars(jid,NULL);
	jparentId =(jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jParentId);
	if(jparentId == NULL)
	{
		nret = 501;
		goto bail;
	}

	metaOutInfo.pparentId = (istring)env->GetStringUTFChars(jparentId,NULL);

	jChildCount = env->GetIntField(jContentInfo,MetaDataInfo::jChildCount);
	if(jChildCount == -1)
		metaOutInfo.bDirectoy = false;
	else
	{
		metaOutInfo.bDirectoy = true;
		metaOutInfo.childCount = jChildCount;
	}

	jtitle =(jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jtitle);
	if(jtitle == NULL)
	{
		nret = 501;

		goto bail;
	}
	metaOutInfo.pTitle = (istring)env->GetStringUTFChars(jtitle,NULL);

	metaOutInfo.size = (int64_t)env->GetLongField(jContentInfo,MetaDataInfo::jSize);

	jmediaClass  = (jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jmediaClass);
	if(jmediaClass)
	{
		metaOutInfo.pMediaclass = (istring)env->GetStringUTFChars(jmediaClass,NULL);
	}
	jprotocalInfo = (jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jprotocalInfo);
	if(jprotocalInfo)
	{
		metaOutInfo.pProtocalInfo = (istring)env->GetStringUTFChars(jprotocalInfo,NULL);
	}
	jresourceUrl = (jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jresourceUrl);
	if(jresourceUrl)
	{
		metaOutInfo.pResourceUrl = (istring)env->GetStringUTFChars(jresourceUrl,NULL);
	}
	jresourceRtspUrl = (jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jresourceRtspUrl);
	if(jresourceUrl)
	{
		metaOutInfo.pResourceRtspUrl = (istring)env->GetStringUTFChars(jresourceRtspUrl,NULL);
	}

	jthumbnailUrl = (jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jthumbnailUrl);
	if(jthumbnailUrl)
	{
		metaOutInfo.pThumbnailUrl = (istring)env->GetStringUTFChars(jthumbnailUrl,NULL);
	}

	if(metaOutInfo.bDirectoy)
	{
		snprintf(tempbuf,1024,CONTAINER_HEAD_STRING,metaOutInfo.pmyId,metaOutInfo.pparentId,metaOutInfo.childCount);
		strresult += (const char*)tempbuf;

		memset(tempbuf,0,1024);

		snprintf(tempbuf,1024,TITLE_ITEM,metaOutInfo.pTitle);
		strresult += (const char*)tempbuf;
		memset(tempbuf,0,1024);

		snprintf(tempbuf,1024,CLASS_ITEM,CONTAINER_TYPE_STRORAGE);
		strresult += (const char*)tempbuf;

		strresult += CONTAINER_END;
	}
	else
	{
		snprintf(tempbuf,1024,ITEM_HEAD_STRING,metaOutInfo.pmyId,metaOutInfo.pparentId);
		strresult += (const char*)tempbuf;

		memset(tempbuf,0,1024);

		snprintf(tempbuf,1024,TITLE_ITEM,metaOutInfo.pTitle);
		strresult += (const char*)tempbuf;
		memset(tempbuf,0,1024);

		if(metaOutInfo.pMediaclass)
		{
			snprintf(tempbuf,1024,CLASS_ITEM,metaOutInfo.pMediaclass);
			strresult += (const char*)tempbuf;
		}


		memset(tempbuf,0,1024);
		snprintf(tempbuf,1024,RES_ITEM,metaOutInfo.pProtocalInfo,metaOutInfo.size,metaOutInfo.pResourceUrl);
		strresult += (const char*)tempbuf;

		memset(tempbuf,0,1024);
		snprintf(tempbuf,1024,RES_ITEM,metaOutInfo.pProtocalInfo,metaOutInfo.size,metaOutInfo.pResourceRtspUrl);
		strresult += (const char*)tempbuf;

		if(metaOutInfo.pThumbnailUrl)
		{
			snprintf(tempbuf,1024,ALBUM_ITEM,metaOutInfo.pThumbnailUrl);
			strresult += (const char*)tempbuf;

		}
		strresult += ITEM_END;
	}

bail:

	if(metaOutInfo.pmyId)
		env->ReleaseStringUTFChars(jid,metaOutInfo.pmyId);
	if(metaOutInfo.pparentId)
		env->ReleaseStringUTFChars(jparentId,metaOutInfo.pparentId);
	if(metaOutInfo.pTitle)
		env->ReleaseStringUTFChars(jtitle,metaOutInfo.pTitle);
	if(metaOutInfo.pProtocalInfo)
		env->ReleaseStringUTFChars(jprotocalInfo,metaOutInfo.pProtocalInfo);
	if(metaOutInfo.pResourceUrl)
		env->ReleaseStringUTFChars(jresourceUrl,metaOutInfo.pResourceUrl);
	if(metaOutInfo.pResourceRtspUrl)
		env->ReleaseStringUTFChars(jresourceUrl,metaOutInfo.pResourceRtspUrl);
	if(metaOutInfo.pThumbnailUrl)
		env->ReleaseStringUTFChars(jthumbnailUrl,metaOutInfo.pThumbnailUrl);
	if(metaOutInfo.pMediaclass)
		env->ReleaseStringUTFChars(jmediaClass,metaOutInfo.pMediaclass);

	return nret;

}
int CESContentDirectory_Device_Android::Browse(icstring id,icstring browseflag,icstring filter,
				   ui4 startingindex,ui4 requestedcount,icstring sort,
				   istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid)
{

	if(m_jCallback == NULL)
		return -1;

	if(sort && strcmp(sort,"") != 0)
		return 709;
	//if(filter&&strcmp(filter,"*") != 0)
	//	return 501;

	JNIEnv * env = GetJNIEnv();
	int nret = 0;
	totalmatches = -1;
	if(strcmp(browseflag,"BrowseMetadata") == 0)
	{
		jstring jId = env->NewStringUTF(id);
		jobject jContentInfo = env->CallObjectMethod(m_jCallback,methIdBrowseMetaData,jId);
		env->DeleteLocalRef(jId);
		if(jContentInfo == NULL)
		{
			nret = 501;
		}
		else
		{
			string strresult;
			strresult += DIDL_HEAD;
			nret = ParseJMetaData(env,jContentInfo,strresult);
			strresult += DIDL_END;
			env->DeleteLocalRef(jContentInfo);
			result = strdup(strresult.c_str());
			numberreturned = totalmatches = 1;
		}
	}
	else if(strcmp(browseflag,"BrowseDirectChildren") == 0)
	{
		jstring jId = env->NewStringUTF(id);
		jobjectArray jContentInfoArray =(jobjectArray)env->CallObjectMethod(m_jCallback,methIdBrowseChildren,jId,
									(jint)startingindex,(jint)requestedcount);
		env->DeleteLocalRef(jId);
		if(jContentInfoArray == NULL)
		{
			nret = 501;
		}
		else
		{
			string strresult;
			strresult += DIDL_HEAD;
			int len = env->GetArrayLength((jarray)jContentInfoArray);

			for (int i = 0; i < len; i++)
			{
				jobject jContentInfo = env->GetObjectArrayElement(jContentInfoArray,i);
				if(jContentInfo)
				{
					nret = ParseJMetaData(env,jContentInfo,strresult);
					if(totalmatches == -1)
						totalmatches =  env->GetIntField(jContentInfo,MetaDataInfo::jTotalMatches);					
					env->DeleteLocalRef(jContentInfo);
					env->SetObjectArrayElement(jContentInfoArray,i,NULL);
					
				}
				else
				{
					Trace("in BrowseDirectChildren no content info\n");
				}

			}
			strresult += DIDL_END;
			env->DeleteLocalRef(jContentInfoArray);
			numberreturned = len;
			result = strdup(strresult.c_str());

		}
	}
	else
	{

		nret =  402;
	}

	DetachJNIThread();

	return nret;
}
int CESContentDirectory_Device_Android::GetPropSet(IXML_Document*& PropSet)
{
	string resultValue="";

	ERROR_TYPE nret = IUpnpUtil::AddToPropertySet(&PropSet,"",resultValue.c_str());

	if(nret != ERROR_SUCCESS)
		return nret;
	return 0;
}

int CESContentDirectory_Device_Android::SetPropSet(const IXML_Document* PropSet)
{
	CComandNortifyEx nortifyEx;
	nortifyEx.DevID = m_thizUDN;
	nortifyEx.ServID = DLNA_CONTENTDIRECTORY_ID;
	nortifyEx.PropSet = (IXML_Document* )PropSet;

	return m_pIUpnp->ExecuteCommand(nortifyEx);
}
int CESContentDirectory_Device_Android::SetWebVFSObserver(IWebVFSObserver *pObserver)
{
	return -1;
}
int CESContentDirectory_Device_Android::GetFileInfo(IN const char *filename, OUT struct File_Info *info)
{
	Trace("BufferingScreenShot: GetFileInfo %s\n",filename);
//    Trace("BufferingScreenShot: GetFileInfo %s, %d\n",filename,m_bRemoteUiStarted);

	// Go to define special device relevant
//	if(strcasecmp(SPECIAL_DEVICE,filename) == 0)
//	{
//		if(m_bRemoteUiStarted != TRUE)
//			return 501;
//		return	m_pSpecialDevice->GetContentInfo(info);
//	}

	
	int nret = 0;
	JNIEnv *env = GetJNIEnv();
	jstring jid = env->NewStringUTF(filename);
	jobject jreturn = env->CallObjectMethod(m_jCallback,methIdGetFileInfo,jid);
	if(!jreturn)
	{
		Trace("GetFileInfo fails%s\n",filename);
		nret = -1;
	}
	else
	{
		nret = ParseJMetaData(env,jreturn,info);
	}
	if(jid)
		env->DeleteLocalRef(jid);
	if(jreturn)
		env->DeleteLocalRef(jreturn);

	DetachJNIThread();
	Trace("CESContentDirectory: GetFileInfo return %d\n",nret);
	return nret ;
}
int CESContentDirectory_Device_Android::ParseJMetaData(JNIEnv* env,jobject jContentInfo,File_Info* info)
{
	info->file_length=(off_t)env->GetLongField(jContentInfo,MetaDataInfo::jSize);

	info->last_modified = 0;
	info->is_directory = false;
	info->is_readable = true;

	jstring mime_type = (jstring)env->GetObjectField(jContentInfo,MetaDataInfo::jprotocalInfo);
	if(mime_type)
	{
		icstring strMimeType = env->GetStringUTFChars(mime_type,NULL);
		info->content_type = strdup(strMimeType);

		Trace("CESContentDirectory: content_type = %s\n",info->content_type);
			
		env->ReleaseStringUTFChars(mime_type,strMimeType);
	}

	Trace("CESContentDirectory: file_length = %d\n",info->file_length);
	
	return 0;
}

//void CESContentDirectory_Device_Android::SetRemoteUIEnabled(BOOL remoteuistarted)
//{
//	m_bRemoteUiStarted = remoteuistarted;
//}

UpnpWebFileHandle CESContentDirectory_Device_Android::FileOpen(const char *filename,IN enum UpnpOpenFileMode Mode)
{
	int nret = 0;
//	if(strcasecmp(SPECIAL_DEVICE,filename) == 0)
//	{
//		return (UpnpWebFileHandle)m_pSpecialDevice->FileOpen();
//	}
	
	JNIEnv *env = GetJNIEnv();
	jstring jid = env->NewStringUTF(filename);
	jobject jreturn = env->CallObjectMethod(m_jCallback,methIdGetFileInfo,jid);
	if(!jreturn)
	{
		Trace("File not found %s\n",filename);
		nret = -1;
	}
	else
	{
		jstring jrealPath = (jstring)env->GetObjectField(jreturn,MetaDataInfo::jrealPath);
		if(jrealPath)
		{
			icstring realpath = env->GetStringUTFChars(jrealPath,NULL);

			int fd = open(realpath,O_RDONLY);
			if(fd == -1)
			{
				Trace("fail to open %s\n",realpath);
				nret = -1;
			}
			else
			{
				IUpnpUtil::CQuickRWLock rwLock(&m_lock,FALSE);
				m_fileHandleList.push_back(fd);
				nret = fd;
			}

			env->ReleaseStringUTFChars(jrealPath,realpath);
		}
		else
		{
			nret = -1;
		}
	}
	if(jid)
		env->DeleteLocalRef(jid);
	if(jreturn)
		env->DeleteLocalRef(jreturn);

	DetachJNIThread();
	Trace("File opened %d\n",nret);
	return (UpnpWebFileHandle)nret ;

}
int CESContentDirectory_Device_Android::FileRead(IN UpnpWebFileHandle fileHnd, OUT char *buf, IN size_t buflen)
{
	int fd = (int)fileHnd;
//	if(m_pSpecialDevice->isFileOpenedByMe(fd))
//		return m_pSpecialDevice->FileRead(buf, buflen);
	
	Trace("Normal FileRead \n");
	return read(fd,buf,buflen);
}
int CESContentDirectory_Device_Android::FileWrite(IN UpnpWebFileHandle fileHnd,IN char *buf,IN size_t buflen)
{
	return -1;
}
int CESContentDirectory_Device_Android::FileSeek(IN UpnpWebFileHandle fileHnd, IN off_t offset,IN int origin)
{
	int fd = (int)fileHnd;

	return lseek(fd,offset,origin);
}
int CESContentDirectory_Device_Android::FileClose(IN UpnpWebFileHandle fileHnd)
{
//	if(m_pSpecialDevice->isFileOpenedByMe((int)fileHnd))
//		return m_pSpecialDevice->FileClose();

	Trace("Normal FileClose\n");
	
	int fd = (int)fileHnd;
	close(fd);

	IUpnpUtil::CQuickRWLock wlock(&m_lock,FALSE);
	CFileHandleList::iterator itbegin = m_fileHandleList.begin();
	CFileHandleList::iterator itend = m_fileHandleList.end();

	for (itbegin; itbegin!=itend; ++itbegin)
	{
		if(fd == (int)*itbegin)
		{
			m_fileHandleList.erase(itbegin);
			return 0;
		}
	}
	return -1;
}

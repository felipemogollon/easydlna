#define LOG_TAG "easydlna-esdmcontroller_android"
#include "CESDlna_Android.h"
#include "CESDMController_Android.h"

#include <assert.h>

typedef list<IXML_Node*>CXMLNodeList;
#define SERACH_ALL ":all"
#define SEARCH_ROOTDEVICE ":rootdevice"
#define SERACH_DMR "urn:schemas-upnp-org:device:MediaRenderer:1"
#define SERACH_DMS "urn:schemas-upnp-org:device:MediaServer:1"

#define SERACH_TYPE_DMS 0
#define SERACH_TYPE_DMR 1
#define SERACH_TYPE_UNKNOWN -1

#define AVT_SETDATASOURCE 0
#define AVT_PLAY 1
#define AVT_STOP 2
#define AVT_PAUSE 3
#define AVT_SEEK 4
#define AVT_GETPOS 5
int start_stop_DMC(JNIEnv* env,jobject jthiz,jboolean jbStart)
{
	if(jbStart != 0)
	{
		Trace("start DMC\n");
		g_pDlnaService->CreateDMC(env);


	}
	else
	{
		Trace("stop DMC\n");
		g_pDlnaService->DestroyDMC(env);
	}
	
	
	return 0;
}

int add_remove_DMCCallback(JNIEnv* env,jobject jthiz,jobject jcallback,jboolean jbAdd)
{
	CESDMController_Android* pController_Android = g_pDlnaService->m_pDMC;
	if(pController_Android == NULL)
		return -1;
	if(jbAdd)
	{
		pController_Android->m_pObserver->SetJavaCallback(env,jcallback);
		pController_Android->m_pController->ScanNetwork(SERACH_ALL,3);
	}
	else
	{
		pController_Android->m_pObserver->SetJavaCallback(env,NULL);
	}

	return 0;
}
int playControl(JNIEnv* env,jobject jthiz,jstring avtActionUrl,jint type,jstring jStringParam)
{
	CESDMController_Android* pController_Android = g_pDlnaService->m_pDMC;
	if(pController_Android == NULL)
		return -1;
	
	CESAVTransportClient_Android *pAVT = pController_Android->m_pAVT;
	if(!pAVT)
		return -1;
	
	icstring url = NULL, param = NULL;
	istring result = NULL;
	int nret = 0;

	url = env->GetStringUTFChars(avtActionUrl,NULL);
	if(url == NULL)
		goto bail;
	if(type == AVT_SETDATASOURCE)
	{
	   param = env->GetStringUTFChars(jStringParam,NULL);
	   if(param == NULL)
	   	goto bail;
	}
	nret = pAVT->playControl(url,type,param,result);
	FREEARRAY(result);
bail:
	if(url)
		env->ReleaseStringUTFChars(avtActionUrl,url);
	if(param)
		env->ReleaseStringUTFChars(jStringParam,param);
	
	return nret;
}
int subscribeToService(icstring strudn,icstring strServiceType)
{
	CESDMController_Android* pController_Android = g_pDlnaService->m_pDMC;
	if(pController_Android == NULL)
		return -1;

	CESContentDirectory_Android * pCDS = pController_Android->m_pCDS;
	CESAVTransportClient_Android* pAVT = pController_Android->m_pAVT;
	if(!pCDS)
		return -1;
	CESDMController *pController = pController_Android->m_pController;
	if(!pController)
		return -1;

	icstring strSubUrl =pController->GetDeviceSubUrl(strudn,strServiceType);
	if(!strSubUrl)
		goto bail;
	
	if(strcmp(DLNA_CONTENTDIRECTORY,strServiceType) == 0)
		pCDS->Subscribe(strudn,strSubUrl);
	else if(strcmp(DLNA_AVTRANSPORT,strServiceType) == 0)
		pAVT->Subscribe(strudn,strSubUrl);

bail:

	return 0;
}
jstring getDeviceActionUrl(JNIEnv* env,jobject jthiz,jstring jstrUDN,jstring jstrServiceType)
{
	CESDMController_Android* pController_Android = g_pDlnaService->m_pDMC;
	if(pController_Android == NULL)
		return NULL;

	icstring strUDN = env->GetStringUTFChars(jstrUDN,NULL);
	if(!strUDN)
		return NULL;
	icstring strServiceType = env->GetStringUTFChars(jstrServiceType,NULL);
	if(!strServiceType)
	{
		env->ReleaseStringUTFChars(jstrUDN,strUDN);
		return NULL;
	}
	CESDMController* pController = pController_Android->m_pController;
	jstring jstrReturn  = NULL;

	istring strReturn = (istring)pController->GetDeviceActionUrl(strUDN,strServiceType
																/*DLNA_CONTENTDIRECTORY*/);
	if(strReturn)
	{
		jstrReturn = env->NewStringUTF(strReturn);

		subscribeToService(strUDN,strServiceType);

		FREEARRAY(strReturn);

		
	}
	
	env->ReleaseStringUTFChars(jstrUDN,strUDN);
	env->ReleaseStringUTFChars(jstrServiceType,strServiceType);
	

	return jstrReturn;
}

void searchNetwork(JNIEnv* env,jobject jthiz,jint type)
{
	CESDMController_Android* pController_Android = g_pDlnaService->m_pDMC;
	if(pController_Android == NULL)
		return ;
	CESDMController *pController = pController_Android->m_pController;
	if(!pController)
		return ;
	int search_type = (int)type;
	switch(search_type)
	{
	case SERACH_TYPE_DMS:
		pController->ScanNetwork(SERACH_DMS,5);
		break;
	case SERACH_TYPE_DMR:
		pController->ScanNetwork(SERACH_DMR,5);
	    break;
	case SERACH_TYPE_UNKNOWN:
		pController->ScanNetwork(SEARCH_ROOTDEVICE,5);
	default:
		pController->ScanNetwork(SERACH_ALL,5);
	    break;
	}
	return ;

}
jobjectArray browseChildren(JNIEnv* env,jobject jthiz,jstring jstringUrl,
							jstring jstringId,jint jIndex,jint jMaxCount)
{
	int nret = 0;
	jobjectArray joutInfoArray = NULL;

	if(jstringUrl == NULL)
		return NULL;

	CESDMController_Android* pController_Android = g_pDlnaService->m_pDMC;
	if(!pController_Android)
		return NULL;

	CESContentDirectory_Android* pCDS = pController_Android->m_pCDS;
	if(!pCDS)
		return NULL;
	
	icstring url = NULL,id = NULL;
	istring result = NULL;
	
	url = env->GetStringUTFChars(jstringUrl,NULL);
	if(!url)
	{
		nret = -1;
		goto fail;
	}
	id = env->GetStringUTFChars(jstringId,NULL);
	if(!id)
	{
		nret = -1;
		goto fail;
	}
	
	
	
	nret = pCDS->BrowseChildren(url,id,jIndex,jMaxCount,result);
	Trace("browseChildren url=%s,id=%s,nret=%d\n",url,id,nret);
	if(nret == 0)
	{
		pCDS->ParseChildren(env,result,joutInfoArray);
	}

	FREEARRAY(result);

fail:
	if(url)
		env->ReleaseStringUTFChars(jstringUrl,url);
	if(id)
		env->ReleaseStringUTFChars(jstringId,id);

	return joutInfoArray;
}
int browseMetaData(JNIEnv* env,jobject jthiz,jstring jstringUrl, jstring jstringId,jobject jObjInitInfo)
{
	int nret = 0;

	if(jstringUrl == NULL)
		return -1;
	CESDMController_Android* pController_Android = g_pDlnaService->m_pDMC;
	if(!pController_Android)
		return -1;
	CESContentDirectory_Android* pCDS = pController_Android->m_pCDS;
	if(!pCDS)
		return -1;

	icstring url = NULL,id = NULL;
	istring result = NULL;
	
	url = env->GetStringUTFChars(jstringUrl,NULL);
	if(!url)
	{
		nret = -1;
		goto fail;
	}
	id = env->GetStringUTFChars(jstringId,NULL);
	if(!id)
	{
		nret = -1;
		goto fail;
	}


	nret = pCDS->BrowseMetaData(url,id,result);
	Trace("browseMetaData url=%s,id=%s,nret=%d\n",url,id,nret);
	if(nret == 0)
	{
		pCDS->ParseMetadata(env,result,jObjInitInfo);
	}
	
	FREEARRAY(result);
fail:
	if(url)
		env->ReleaseStringUTFChars(jstringUrl,url);
	if(id)
		env->ReleaseStringUTFChars(jstringId,id);

	return nret;
}
void TransbackToJavaObject(JNIEnv* env, MetaDataInfo& metaInfo,jobject & jContentInfoObject)
{
	if(metaInfo.pmyId)
	{
		jstring jstrmyId = env->NewStringUTF(metaInfo.pmyId);
		env->SetObjectField(jContentInfoObject,MetaDataInfo::jId,jstrmyId);
	}
	if(metaInfo.pparentId)
	{
		jstring jstrpparentId = env->NewStringUTF(metaInfo.pparentId);
		env->SetObjectField(jContentInfoObject,MetaDataInfo::jParentId,jstrpparentId);
	}
	
	if(!metaInfo.bDirectoy)
		metaInfo.childCount = -1;

	env->SetIntField(jContentInfoObject,MetaDataInfo::jChildCount,metaInfo.childCount);


	if(metaInfo.pTitle)
	{
		jstring jstrTitle = env->NewStringUTF(metaInfo.pTitle);
		env->SetObjectField(jContentInfoObject,MetaDataInfo::jtitle,jstrTitle);
	}
	if(metaInfo.pProtocalInfo)
	{
		jstring jstrProtocal = env->NewStringUTF(metaInfo.pProtocalInfo);
		env->SetObjectField(jContentInfoObject,MetaDataInfo::jprotocalInfo,jstrProtocal);
	}
	if(metaInfo.pResourceUrl)
	{
		jstring jstrResourceUrl = env->NewStringUTF(metaInfo.pResourceUrl);
		env->SetObjectField(jContentInfoObject,MetaDataInfo::jresourceUrl,jstrResourceUrl);
	}
	if(metaInfo.pResourceRtspUrl)
	{
		jstring jstrResourceRtspUrl = env->NewStringUTF(metaInfo.pResourceRtspUrl);
		env->SetObjectField(jContentInfoObject,MetaDataInfo::jresourceRtspUrl,jstrResourceRtspUrl);
	}
	if(metaInfo.pThumbnailUrl)
	{
		jstring jstrThumbnailUrl = env->NewStringUTF(metaInfo.pThumbnailUrl);
		env->SetObjectField(jContentInfoObject,MetaDataInfo::jthumbnailUrl,jstrThumbnailUrl);
	}

}
int ParseContainerInfo(JNIEnv*env,jobject jContentInfoObject,IXML_Element* elementContainer,
						   MetaDataInfo & metaInfo)
{
	metaInfo.pmyId = (char*)ixmlElement_getAttribute(elementContainer,"id");
	metaInfo.pparentId = (char*)ixmlElement_getAttribute(elementContainer,"parentID");
	DOMString childCountStr = (char*)ixmlElement_getAttribute(elementContainer,"childCount");
	metaInfo.childCount = childCountStr != NULL ?atoi(childCountStr):0;

//	const char* pvalue = NULL;

// 	if((pvalue =ixmlElement_getAttribute(elementContainer,"restricted")))
// 	{
// 		metaInfo.modifiable = strcmp(pvalue,"true")==0;
// 	}
// 	if((pvalue =ixmlElement_getAttribute(elementContainer,"searchable")))
// 	{
// 		metaInfo.searchable = strcmp(pvalue,"true")==0;
// 	}

	IXML_NodeList* pNodeList = ixmlNode_getChildNodes((IXML_Node*)elementContainer);
	assert(pNodeList);

	int nodeLen = ixmlNodeList_length(pNodeList);
	for (int i = 0; i < nodeLen; i++)
	{
		IXML_Node* pNode = ixmlNodeList_item(pNodeList,i);

		if(strcmp(ixmlNode_getNodeName(pNode),"dc:title") == 0)
		{
			metaInfo.pTitle = (char*)ixmlNode_getNodeValue(pNode->firstChild);
			//Trace("parse dir item title=[%s]\n",metaInfo.pTitle);
			break;
		}

	}

	TransbackToJavaObject(env,metaInfo,jContentInfoObject);

	ixmlNodeList_free(pNodeList);

	return 0;
}
int ParseItemInfo(JNIEnv*env,jobject jContentInfoObject,IXML_Element* elementItem,MetaDataInfo & metaInfo)
{
	metaInfo.pmyId = (char*)ixmlElement_getAttribute(elementItem,"id");
	metaInfo.pparentId =(char*) ixmlElement_getAttribute(elementItem,"parentID");

	IXML_NodeList* pNodeList = ixmlNode_getChildNodes((IXML_Node*)elementItem);
	assert(pNodeList);

	IXML_NamedNodeMap* pAttributes = NULL;
	int nodeLen = ixmlNodeList_length(pNodeList);

	int checkCount = 3;
	for (int i = 0; i < nodeLen; i++)
	{
		IXML_Node* pNode = ixmlNodeList_item(pNodeList,i);
		if(strcmp(ixmlNode_getNodeName(pNode),"dc:title") == 0)
		{
			metaInfo.pTitle = (char*)ixmlNode_getNodeValue(pNode->firstChild);
			//Trace("parse file item title=[%s]\n",metaInfo.pTitle);
			--checkCount;
		}
		else if(strcmp(ixmlNode_getNodeName(pNode),"upnp:albumArtURI") == 0)
		{
			if(metaInfo.pThumbnailUrl == NULL)
			{
				metaInfo.pThumbnailUrl = (char*)ixmlNode_getNodeValue(pNode->firstChild);
				--checkCount;
			}
		}
		else if(strcmp(ixmlNode_getNodeName(pNode),"res") == 0 )
		{
			if(metaInfo.pResourceUrl != NULL) continue;

			char* pResUrl = (char*)ixmlNode_getNodeValue(pNode->firstChild);
			if(strncmp(pResUrl,"127",3) != 0)
			{
				metaInfo.pResourceUrl = pResUrl;
				pAttributes = ixmlNode_getAttributes(pNode);
				if(pAttributes)
				{
					IXML_Node* pAttributeNode = ixmlNamedNodeMap_getNamedItem(pAttributes,"protocolInfo");
					if(pAttributeNode)
						metaInfo.pProtocalInfo = (char*)ixmlNode_getNodeValue(pAttributeNode);
					//pAttributeNode = ixmlNamedNodeMap_getNamedItem(pAttributes,"size");
					//if(pAttributeNode)
					//	metaInfo.size = atoi(ixmlNode_getNodeValue(pAttributeNode));
				}
				--checkCount;
			}
		}
		if(checkCount == 0)
			break;
	}

	TransbackToJavaObject(env,metaInfo,jContentInfoObject);


	ixmlNamedNodeMap_free(pAttributes);
	ixmlNodeList_free(pNodeList);

	return 0;
}
int RemoveNonePhsychalContainer(IXML_NodeList* pContainerList,int lenContent, CXMLNodeList &xmlNodeList)
{
	for (int i = 0; i < lenContent; i++)
	{
		IXML_Node *pNode = ixmlNodeList_item(pContainerList,i);
		
		if(strcmp(ixmlElement_getAttribute((IXML_Element*)pNode,"childCount"),"0") == 0)
		{
			Trace("No child in this directory\n");
			continue;
		}

		IXML_NodeList* pNodeList = ixmlNode_getChildNodes((IXML_Node*)pNode);

		
		BOOL bFirsrChild = strcmp(ixmlElement_getAttribute((IXML_Element*)pNode,"parentID"),"0") == 0;
		int nodeLen = ixmlNodeList_length(pNodeList);

		BOOL  canBeAdd = TRUE;
		int checkCount = 2;
		for (int i = 0; i < nodeLen; i++)
		{
			IXML_Node* pNodeChild = ixmlNodeList_item(pNodeList,i);

			if(strcmp(ixmlNode_getNodeName(pNodeChild),"upnp:class") == 0)
			{	
				if(strcmp((char*)ixmlNode_getNodeValue(pNodeChild->firstChild), CONTAINER_TYPE_STRORAGE) != 0 && !bFirsrChild)
				{
					canBeAdd = FALSE;
					break;
				}
				--checkCount;
			}
			else if(strcmp(ixmlNode_getNodeName(pNode),"dc:title") == 0)
			{
				icstring title= ixmlNode_getNodeValue(pNode->firstChild);
				if(strcmp(title,"All Image Items") == 0 ||
					strcmp(title,"All Audio Items") == 0 ||
					strcmp(title,"All Video Items") == 0 ||
					strcmp(title,"All Playlists") == 0 ) //erase virtual dir from intel-media-server
				{
					canBeAdd = FALSE;
					break;
				}
				--checkCount;
			}

			if(checkCount == 0)
				break;
		}
		if(canBeAdd)
			xmlNodeList.push_back(pNode);
	}

	return 0;

}

int ParseCDandItemList(JNIEnv *env,jobjectArray& joutInfoArray,IXML_NodeList* pContainerList,IXML_NodeList* pItemList)
{
	int lenContent = 0, lenItem = 0;
	if(pContainerList)
	{
		lenContent = ixmlNodeList_length(pContainerList);
	}
	if(pItemList)
	{
		lenItem = ixmlNodeList_length(pItemList);
	}


	if(lenContent + lenItem == 0)
	{
		Trace("ParseCDandItemList no items and contents\n");
		return 0;
	}
	jclass jthizClass = env->FindClass(JAVA_CONTENT_CLASS_NAME);
	if(!jthizClass)
	{
		Trace("fail to find class of %s\n",JAVA_CONTENT_CLASS_NAME);
		return -1;
	}
	
	
	
	CXMLNodeList xmlNodeList;
	if(lenContent != 0)
		RemoveNonePhsychalContainer(pContainerList,lenContent,xmlNodeList);

	lenContent = xmlNodeList.size();
	
	if(lenContent + lenItem == 0)
	{
		Trace("ParseCDandItemList no items and contents after remove virtual dir\n");
		return 0;
	}
	
	Trace("ParseCDandItemList content = [%d],item = [%d]\n",lenContent, lenItem);

	//jobject jItem = env->NewObject(jthizClass,MetaDataInfo::jConstructId);
	joutInfoArray = env->NewObjectArray(lenContent+lenItem,jthizClass,NULL);
	
	CXMLNodeList::iterator itbegin = xmlNodeList.begin();
	CXMLNodeList::iterator itend = xmlNodeList.end();
	int index = -1;
	for (itbegin; itbegin!=itend; ++itbegin)
	{
		IXML_Node *pNode = *(itbegin);
		index +=1 ;
		//Trace("dir item index [%d]\n",index);
		
		jobject jContentInfoObj = env->NewObject(jthizClass,MetaDataInfo::jConstructId);
		if(jContentInfoObj == NULL)
		{
			Trace("fail to create object in JNI for dir!!!\n");
			break;
		}

		MetaDataInfo metaInfo;
		metaInfo.bDirectoy = true;
		ParseContainerInfo(env,jContentInfoObj,(IXML_Element*)pNode,metaInfo);
		env->SetObjectArrayElement(joutInfoArray,index,jContentInfoObj);
		env->DeleteLocalRef(jContentInfoObj);
	}
	xmlNodeList.clear();
	index += 1;
	for (int i = 0; i < lenItem; i++)
	{
		IXML_Node *pNode = ixmlNodeList_item(pItemList,i);

		jobject jContentInfoObj = env->NewObject(jthizClass,MetaDataInfo::jConstructId);
		if(jContentInfoObj == NULL)
		{
			Trace("fail to create object in JNI for file !!!\n");
			break;
		}
		MetaDataInfo metaInfo;
		metaInfo.bDirectoy = false;

		ParseItemInfo(env,jContentInfoObj,(IXML_Element*)pNode,metaInfo);
		env->SetObjectArrayElement(joutInfoArray,index+i,jContentInfoObj);
		env->DeleteLocalRef(jContentInfoObj);
	}

	return 0;	
}
CESContentDirectory_Android::CESContentDirectory_Android(IUpnpControlPoint * pIUpnpCtrlpt)
:m_pIUpnpCtrlpt(pIUpnpCtrlpt),
m_pController(NULL)
//m_pObserver(NULL)
{

}
CESContentDirectory_Android::~CESContentDirectory_Android()
{

}
int CESContentDirectory_Android::Subscribe(icstring strUDN,icstring strUrl)
{
	CCommandSubscribe subcomand(FALSE);
	subcomand.PublisherUrl = strUrl;
	subcomand.TimeOut = 900;
	
	int nret = m_pIUpnpCtrlpt->ExecuteCommand(subcomand);

	if(nret != 0)
	{
		Trace("subscribe to %s fails %s\n",strUrl,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		m_pController->SetDeviceSubscribeResult(strUDN,strUrl,subcomand);
	}
	return 0;
}
void CESContentDirectory_Android::NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify)
{
	if(pEventSource && eventNortify.ChangedVariables)
	{
		IXML_Element *pContainrUpdateid = ixmlDocument_getElementById(eventNortify.ChangedVariables,"ContainerUpdateIDs");
		if(!pContainrUpdateid)
		{
			Trace("device %s notify sth we do not handle\n",pEventSource->pstrUDN);
			return;
		}
		else
		{
			icstring iValue = ixmlNode_getNodeValue(((IXML_Node*)pContainrUpdateid)->firstChild);
			if(iValue == NULL)
			{
				Trace("device %s notify ContainerUpdateIDs without value\n",pEventSource->pstrUDN);
				return;
			}	
			/*if(m_pObserver)
			{
				m_pObserver->NortifyChanges(pEventSource->pstrUDN,iValue);
			}*/
		}
		
	}
	return;
}
int CESContentDirectory_Android::ParseChildren(JNIEnv* env,istring result, jobjectArray& joutInfoArray)
{
	IXML_Document* pxmlDoc = ixmlParseBuffer(result);
	int nret = 0;
	if(!pxmlDoc)
	{
		Trace("Cannot parse result-info into XML\n");
		nret = -1;
		return nret;
	}

	IXML_NodeList *pContainerList = ixmlDocument_getElementsByTagName(pxmlDoc,"container");
	IXML_NodeList* pItemList = ixmlDocument_getElementsByTagName(pxmlDoc,"item");

	if(!pContainerList && !pItemList)
	{
		Trace("ParseContentDirectory Cannot get valid information from result\n");

		nret = -1;
		ixmlDocument_free(pxmlDoc);

		return nret;
	}

	ParseCDandItemList(env,joutInfoArray,pContainerList,pItemList);

	if(pContainerList)
		ixmlNodeList_free(pContainerList);
	if(pItemList)
		ixmlNodeList_free(pItemList);

	ixmlDocument_free(pxmlDoc);
	return nret;

}
int CESContentDirectory_Android::ParseMetadata(JNIEnv* env,istring result,jobject & jObjInitInfo)
{
	IXML_Document* pxmlDoc = ixmlParseBuffer(result);
	if(!pxmlDoc)
	{
		Trace("Fail to parse istring into xml doc\n");
		return -1;
	}

	IXML_Element*  element = NULL;
	MetaDataInfo metaInfo;
	int nret = 0;

	if((element = ixmlDocument_getElementById(pxmlDoc,"container")) != NULL)	
	{
		Trace("browse meta-info of a directory\n");

		metaInfo.bDirectoy = TRUE;
		ParseContainerInfo(env,jObjInitInfo,element,metaInfo);
	}
	else if((element = ixmlDocument_getElementById(pxmlDoc,"item"))!=NULL)
	{
		Trace("browse meta-info of a file\n");

		metaInfo.bDirectoy = FALSE;
		ParseItemInfo(env,jObjInitInfo,element,metaInfo);

	}
	else
	{
		Trace("can not get container or item info from xml\n");
		nret = -1;
	}

	ixmlDocument_free(pxmlDoc);

	return nret;

}
int CESContentDirectory_Android::BrowseChildren(icstring url, icstring id,ui4 startingindex,
												ui4 requestedcount,istring& result)
{
	char index[9]={'\0'},count[9]={'\0'};
	snprintf(index,9,"%d",startingindex);
	snprintf(count,9,"%d",requestedcount);		

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = url;

	cmdSendAction.ServiceType = DLNA_CONTENTDIRECTORY;
	cmdSendAction.Action = 
		IUpnpUtil::MakeAction("Browse",DLNA_CONTENTDIRECTORY,
		6,"ObjectID",id,"BrowseFlag",ACTION_BROWSE_CHILDREN,
		"Filter","*","StartingIndex",index,"RequestedCount",
		count,"SortCriteria","");

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_BROWSE,
			cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = m_pIUpnpCtrlpt->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_BROWSE,
			IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		icstring pValueResult = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Result");
		if(pValueResult)
		{
			SAFEDUP(result,pValueResult);
		}

	}

	return nret;
}



int CESContentDirectory_Android::BrowseMetaData(icstring url, icstring id,istring& result)
{
	char index[9]={'\0'},count[9]={'\0'};
	snprintf(index,9,"%d",0);
	snprintf(count,9,"%d",0);		

	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = url;

	cmdSendAction.ServiceType = DLNA_CONTENTDIRECTORY;
	cmdSendAction.Action = 
		IUpnpUtil::MakeAction("Browse",DLNA_CONTENTDIRECTORY,
		6,"ObjectID",id,"BrowseFlag",ACTION_BROWSE_METADATA,
		"Filter","*","StartingIndex",index,"RequestedCount",
		count,"SortCriteria","");

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for [%s] at [%s]\n",ACTION_BROWSE,
			cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = m_pIUpnpCtrlpt->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to [%s] : [%s]\n",ACTION_BROWSE,
			IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		icstring pValueResult = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"Result");
		if(pValueResult)
		{
			SAFEDUP(result,pValueResult);
		}
	}

	return nret;
}
CESAVTransportClient_Android::CESAVTransportClient_Android(IUpnpControlPoint * pIUpnpDevice)
:m_pIUpnpCtrlpt(pIUpnpDevice),
m_pController(NULL),
m_pServiceObserver(NULL)
{

}
CESAVTransportClient_Android::~CESAVTransportClient_Android()
{
}
int CESAVTransportClient_Android::Subscribe(icstring strudn,icstring strUrl)
{
	CCommandSubscribe subcomand(FALSE);
	subcomand.PublisherUrl = strUrl;
	subcomand.TimeOut = 900;

	int nret = m_pIUpnpCtrlpt->ExecuteCommand(subcomand);

	if(nret != 0)
	{
		Trace("subscribe to %s fails %s\n",strUrl,IUpnpUtil::GetErrorMessage(nret));
	}
	else
	{
		m_pController->SetDeviceSubscribeResult(strudn,strUrl,subcomand);
	}

	return 0;
}
void CESAVTransportClient_Android::NortifyObserver(CDevice* pEventSource,CEventEvent & eventNortify)
{
	if(m_pServiceObserver)
		m_pServiceObserver->Nortify((IDlnaService*)this,pEventSource,eventNortify.ChangedVariables);
};
int CESAVTransportClient_Android::parsePosition(icstring result)
{
	char* pars = (char*)result;
	int factor = 2;
	int nret = 0;
	while(pars = strtok(pars,":"))
	{
		int x = atoi(pars);
		switch(factor)
		{
		case 2:
			x *= 3600;
			break;
		case 1:
			x *=60;
			break;
		default:
			break;
	      }
		nret += x;
		factor--;
		pars = NULL;// according to strtok,sebsequent parsing requires param "pars" be NULL
	}
	return nret;
	
}
int	CESAVTransportClient_Android::playControl(icstring url,int type,icstring param,istring & result)
{
	CCommandSendAction cmdSendAction(FALSE);

	cmdSendAction.ActionURL = url;

	cmdSendAction.ServiceType = DLNA_AVTRANSPORT;
	switch(type)
	{
	case AVT_SETDATASOURCE:
		cmdSendAction.Action = 
			IUpnpUtil::MakeAction("SetAVTransportURI",DLNA_AVTRANSPORT,
								  3,"InstanceID","0","CurrentURI",param,
								  "CurrentURIMetaData","");
		break;
	case AVT_PLAY:
		cmdSendAction.Action = 
			IUpnpUtil::MakeAction("Play",DLNA_AVTRANSPORT,
								 2,"InstanceID","0","Speed","1");
		break;
	case AVT_STOP:
		cmdSendAction.Action = 
			IUpnpUtil::MakeAction("Stop",DLNA_AVTRANSPORT,
							      1,"InstanceID","0");
	    break;
	case AVT_PAUSE:
		cmdSendAction.Action = 
			IUpnpUtil::MakeAction("Pause",DLNA_AVTRANSPORT,
								  1,"InstanceID","0");
	    break;

	case AVT_GETPOS:
		cmdSendAction.Action = 
			IUpnpUtil::MakeAction("GetPositionInfo",DLNA_AVTRANSPORT,
			1,"InstanceID","0");
		break;
	case AVT_SEEK://
	default:
	    return ERROR_OUTOF_MEMORY;
	}
	

	if(cmdSendAction.Action == NULL)
	{
		Trace(
			"fail to make action for AVT [%d] at [%s]\n",type,
			cmdSendAction.ActionURL);
		return ERROR_OUTOF_MEMORY;
	}

	ERROR_TYPE nret = m_pIUpnpCtrlpt->ExecuteCommand(cmdSendAction);
	if(nret != ERROR_SUCCESS)
	{
		Trace(
			"fail to AVT [%d] : [%s]\n",type,
			IUpnpUtil::GetErrorMessage(nret));
	}
	else 
	{
		if(type == AVT_GETPOS)
		{
			icstring pValueResult = IUpnpUtil::GetValueFromXML((cmdSendAction.RespNode),"RelTime");
			if(pValueResult)
			{
			   Trace("get pos: %s\n",pValueResult);
			   nret = (ERROR_TYPE)parsePosition(pValueResult);
			}
		}
	}

	return nret;
}


int CESAVTransportClient_Android::ParseLastChange(JNIEnv* env,jstring & urlString,jstring & stateString,jstring& statusString,
					 jstring & durationString,IXML_Document * changeValueList)
{

	const char* lastChange = IUpnpUtil::GetValueFromXML(changeValueList,"LastChange");
	if(lastChange ==NULL)
	{
	  Trace("no lastchange in this xml!\n");
	}
	IXML_Document* pDocLastChange = ixmlParseBuffer(lastChange);
	if(pDocLastChange == NULL)
	{
		Trace("fail to parse last change into xml!\n");
		return -1;
	}
	icstring url = NULL,state = NULL,status=NULL,duration=NULL;
	url = IUpnpUtil::GetAttributeValueFromXML(pDocLastChange,"AVTransportURI","val");
	state = IUpnpUtil::GetAttributeValueFromXML(pDocLastChange,"TransportState","val");
	status = IUpnpUtil::GetAttributeValueFromXML(pDocLastChange,"TransportStatus","val");
	duration = IUpnpUtil::GetAttributeValueFromXML(pDocLastChange,"CurrentTrackDuration","val");

	if(url == NULL && state == NULL && status == NULL && duration ==NULL)
	{
		ixmlDocument_free(pDocLastChange);
		return -1;
	}
	
	urlString = env->NewStringUTF(url==NULL?"":url);
	stateString = env->NewStringUTF(state==NULL?"":state);
	statusString = env->NewStringUTF(status==NULL?"":status);
	durationString = env->NewStringUTF(duration==NULL?"":duration);
	Trace("ParseLastChange : url=%s,state=%s,status=%s,duration=%s\n",
						url?url:"null",state?state:"null",status?status:"null",duration?duration:"null");
	
	ixmlDocument_free(pDocLastChange);
	return 0;

}

DMCObserver::DMCObserver(JNIEnv *env)
:m_jCallback(NULL),
judnString(NULL),
jnameString(NULL),
jtypeString(NULL),
jstate(NULL),
jConstructor(NULL),
jNotify(NULL),
jbaseUrlString(NULL),
m_jDeviceClass(NULL)
{
	jclass jdeviceclass = env->FindClass(JAVA_DLNADEVICE_CLASS_NAME);
	m_jDeviceClass = (jclass)env->NewGlobalRef(jdeviceclass);
	judnString = env->GetFieldID(jdeviceclass,"udnString","Ljava/lang/String;");
	jnameString = env->GetFieldID(jdeviceclass,"nameString","Ljava/lang/String;");
	jtypeString = env->GetFieldID(jdeviceclass,"typeString","Ljava/lang/String;");
	jbaseUrlString = env->GetFieldID(jdeviceclass,"baseUrlString","Ljava/lang/String;");
	jstate = env->GetFieldID(jdeviceclass,"stateNow","I");
	
	jConstructor = env->GetMethodID(jdeviceclass,"<init>","()V");

	jclass jNotitfyclass = env->FindClass(JAVA_DMCCALLBACK_CLASS_NAME);
	jNotify = env->GetMethodID(jNotitfyclass,"Nortify","(Lcom/easydlna/dlna/service/DlnaDevice;)I");
	jAVTStatusChanges = env->GetMethodID(jNotitfyclass,"AVTStatusChangesNotify",
										"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
}
DMCObserver::~DMCObserver()
{
}
void DMCObserver::SetJavaCallback(JNIEnv* env,jobject jcallback)
{
	QUICKWRITELOCK(&m_lock);

	if(m_jCallback)
	{
		env->DeleteGlobalRef(m_jCallback);
		m_jCallback = NULL;
	}
	if(jcallback != NULL)
		m_jCallback = env->NewGlobalRef(jcallback);
	return ;
}
int DMCObserver::Nortify(IDlnaService *pSource, CDevice* pSrcDevice,IXML_Document * changeValueList)
{
	Trace("notify statue value CHANGE\n");

	switch(pSource->GetServiceType())
	{
	case DLNA_ST_AVTRANSPORT:
		{
			QUICKREADLOCK(&m_lock);

			if(m_jCallback)
			{	
				JNIEnv* env = GetJNIEnv();

				jstring jUDN = NULL;

				Trace("Nortify AVT Changes to JAVA \n");

				jstring jurl = NULL,jstate = NULL,jstatus = NULL,jduration= NULL;
				
				CESAVTransportClient_Android* pAVT = (CESAVTransportClient_Android*)pSource;
				if(pAVT->ParseLastChange(env,jurl,jstate,jstatus,jduration,changeValueList) == 0)
				{
					jUDN = env->NewStringUTF(pSrcDevice->pstrUDN);
					env->CallIntMethod(m_jCallback,jAVTStatusChanges,jUDN,jurl,jstate,jstatus,jduration);
				}
				if(jUDN)
					env->DeleteLocalRef(jUDN);
				if(jurl)
					env->DeleteLocalRef(jurl);
				if(jstate)
					env->DeleteLocalRef(jstate);
				if(jduration)
					env->DeleteLocalRef(jduration);

				DetachJNIThread();
			}
		}
		break;
	case DLNA_ST_CONTENTDIRECTORY:
		Trace("notify statue value is CDS\n");
		break;
	default:
		Trace("notify statue value is not AVT or CDS\n");
		break;
	}

	
	Trace("notify changes end\n");
	return 0;

}
int DMCObserver::Nortify(CDevice & dlnaDevice)
{
	Trace("notify a device\n");

	if(strcmp(dlnaDevice.pstrDeviceType,SERACH_DMS) != 0 && 
		strcmp(dlnaDevice.pstrDeviceType,SERACH_DMR) !=0 )
	{
		Trace("Notify a non DMS|DMR device\n");
		return 0;
	}
	QUICKREADLOCK(&m_lock);
	if(m_jCallback)
	{	
		JNIEnv* env = GetJNIEnv();
		jclass jdeviceclass = m_jDeviceClass;
		if(jdeviceclass == NULL)
		{
			Trace("fail to find class of %s\n",JAVA_DLNADEVICE_CLASS_NAME);
			DetachJNIThread();
			return -1;
		}
		
		jobject jDlnaDevice = env->NewObject(jdeviceclass,jConstructor);
	
		jstring jUDN = env->NewStringUTF(dlnaDevice.pstrUDN);
		env->SetObjectField(jDlnaDevice,judnString,jUDN);

		jstring jbase = env->NewStringUTF(dlnaDevice.pBaseUrl);
		env->SetObjectField(jDlnaDevice,jbaseUrlString,jbase);

		if(dlnaDevice.pstrFriendlyName)
		{
			jstring jname = env->NewStringUTF(dlnaDevice.pstrFriendlyName);
			env->SetObjectField(jDlnaDevice,jnameString,jname);
		}


		jstring jType = env->NewStringUTF(dlnaDevice.pstrDeviceType);
		env->SetObjectField(jDlnaDevice,jtypeString,jType);

		env->SetIntField(jDlnaDevice,jstate,dlnaDevice.onLine);
		
		Trace("Nortify DlnaDevice to JAVA \n");
		int nNeedSubs = env->CallIntMethod(m_jCallback,jNotify,jDlnaDevice);

		DetachJNIThread();
	}
	Trace("notify a device end\n");
	return 0;
}
void DMCObserver::Destroy(JNIEnv* env)
{
	if(m_jCallback)
	{
		env->DeleteGlobalRef(m_jCallback);
		m_jCallback = NULL;
	}
	if(m_jDeviceClass)
		env->DeleteGlobalRef(m_jDeviceClass);
	delete(this);

	return;
}

CESDMController_Android::CESDMController_Android()
:m_pController(NULL),
m_pCDS(NULL),
m_pAVT(NULL)
{

}
CESDMController_Android::~CESDMController_Android()
{
	
}

int CESDMController_Android::RegisterDMCService(JNIEnv* env)
{
	JNINativeMethod Methods[] = 
	{
		{"native_start_stop_DMC", "(Z)I",(void *)start_stop_DMC},
		{"native_add_remove_DMCObserver","(Lcom/easydlna/dlna/service/DMCService$IDMCObserver;Z)V",(void *)add_remove_DMCCallback},
		{"native_getDeviceActionUrl","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
				(void*)getDeviceActionUrl},
		{"native_browseChildren","(Ljava/lang/String;Ljava/lang/String;II)[Lcom/easydlna/dlna/service/ContentInfo;",
				(void*)browseChildren},
		{"native_browseMetaData","(Ljava/lang/String;Ljava/lang/String;Lcom/easydlna/dlna/service/ContentInfo;)I",
				(void*)browseMetaData},
		{"native_searchNetwork","(I)V",(void*)searchNetwork},
		{"native_playControl","(Ljava/lang/String;ILjava/lang/String;)I",(void*)playControl},
	};

	return RegisterNativeMethods(env,JAVA_DMCSERVICE_CLASS_NAME,Methods,
		sizeof(Methods)/sizeof(Methods[0]));
}
void  CESDMController_Android::InitDMC(JNIEnv *env, IDlnaFrame* pDlnaFrame)
{
	m_pController = (CESDMController*)pDlnaFrame->GetDMController("easydlna");
	if(!m_pController)
	{
		Trace("fail to get controller from easydlna");
		return ;
	}
	m_pObserver = new DMCObserver(env);
	m_pCDS = new CESContentDirectory_Android((IUpnpControlPoint*)m_pController->QueryUpnp());
	m_pCDS->m_pController = m_pController;
	m_pCDS->SetObserver((IDlnaServiceObserver*)m_pObserver);

	m_pAVT = new CESAVTransportClient_Android((IUpnpControlPoint*)m_pController->QueryUpnp());
	m_pAVT->m_pController = m_pController;
	m_pAVT->SetObserver((IDlnaServiceObserver*)m_pObserver);
	

	m_pController->SetObserver(m_pObserver);
	m_pController->SetContentDirectory(m_pCDS);
	m_pController->SetAVTransport(m_pAVT);

	return ;
}

void CESDMController_Android::DestroyDMC(JNIEnv* env)
{
	m_pController->Release();
	m_pController = NULL;
	DELETE(m_pCDS);
	DELETE(m_pAVT);
	m_pObserver->Destroy(env);
	m_pObserver = NULL;
	delete(this);
	return ;
}

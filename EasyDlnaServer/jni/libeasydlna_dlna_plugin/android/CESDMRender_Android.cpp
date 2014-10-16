
#define LOG_TAG "Android_DMRender"

#include "CESDMRender_Android.h"
#include "CESDlna_Android.h"

#include "IDMRender.h"
#include "IDlnaFrame.h"
#include "IDlnaAVTransport.h"

#define JAVA_DMRSERVICE_CLASS_NAME "com/easydlna/dlna/service/DMRService"
#define JAVA_DMRCALLBACK_CLASS_NAME "com/easydlna/dlna/service/DMRService$IDMRObserver"

int start_stop_DMR(JNIEnv* env,jobject jthiz,jboolean jbStart)
{
	if(jbStart != 0)
	{
		Trace("start DMR\n");
		g_pDlnaService->CreateDMR(env);


	}
	else
	{
		Trace("stop DMR\n");
		g_pDlnaService->DestroyDMR(env);
	}


	return 0;
}

int add_remove_DMRCallback(JNIEnv* env,jobject jthiz,jobject jcallback,jboolean jbAdd)
{
	CESDMRender_Android* pRender_Android = g_pDlnaService->m_pDMR;
	if(pRender_Android == NULL)
		return -1;
	if(jbAdd)
	{
		pRender_Android->m_pAVT->SetCallbackObject(env,jcallback);
		pRender_Android->m_pRender->Broadcast(900);
	}
	else
	{
		pRender_Android->m_pAVT->SetCallbackObject(env,NULL);
	}

	return 0;
}
void setDMRFriendlyName(JNIEnv* env,jobject jthiz,jstring jstrFriendlyName)
{
	icstring strName = env->GetStringUTFChars(jstrFriendlyName,NULL);
	if(!strName)
		return;
	
	FREEARRAY(CESDMRender_Android::m_RenderName);
	CESDMRender_Android::m_RenderName = strdup(strName);
	
	env->ReleaseStringUTFChars(jstrFriendlyName,strName);
	return;
}
void sendRenderStatus(JNIEnv* env,jobject jthiz,jobject jrenderStatus)
{
	CESDMRender_Android* pRender_Android = g_pDlnaService->m_pDMR;
	if(pRender_Android == NULL)
		return ;
	pRender_Android->m_pAVT->NortifyStateChange(env,jrenderStatus);
}
static void sendBroadcast(JNIEnv* env,jobject jthiz,jint jtimeOut)
{
	CESDMRender_Android* pSever_Android = g_pDlnaService->m_pDMR;
	if(pSever_Android == NULL || pSever_Android->m_pRender == NULL)
		return ;
	pSever_Android->m_pRender->Broadcast(jtimeOut);
	return;
}
istring CESDMRender_Android::m_RenderName = NULL;
CESDMRender_Android::CESDMRender_Android():
m_pRender(NULL),
m_pAVT(NULL)
{
}
CESDMRender_Android::~CESDMRender_Android()
{
}
int CESDMRender_Android::RegisterDMRService(JNIEnv*env)
{
	JNINativeMethod Methods[] = 
	{
		{"native_start_stop_DMR", "(Z)I",(void *)start_stop_DMR},
		{"native_add_remove_DMRObserver","(Lcom/easydlna/dlna/service/DMRService$IDMRObserver;Z)V",(void *)add_remove_DMRCallback},
		{"native_set_DMRFriendlyName","(Ljava/lang/String;)V",(void*)setDMRFriendlyName},
		{"native_send_RenderStatus","(Lcom/easydlna/dlna/service/RenderStatus;)V",(void*)sendRenderStatus},
		{"native_sendBroadcast","(I)V",(void*)sendBroadcast},
		
	};
	
	return RegisterNativeMethods(env,JAVA_DMRSERVICE_CLASS_NAME,Methods,
		sizeof(Methods)/sizeof(Methods[0]));
}
void  CESDMRender_Android::InitDMR(JNIEnv *env, IDlnaFrame* pDlnaFrame)
{
	m_pRender = (CESDMRender*)pDlnaFrame->GetDMRender("easydlna");
	if(!m_pRender)
	{
		Trace("fail to get render from easydlna");
		return ;
	}
	m_pAVT = new CESAVTransport_Android(env,(IUpnpDevice*)m_pRender->QueryUpnp());
	m_pRender->SetAVTransport(m_pAVT);
	
	m_pRender->RegisterDMRender(NULL,NULL,m_RenderName);
	return ;
}

void CESDMRender_Android::DestroyDMR(JNIEnv* env)
{
	m_pRender->Release();
	m_pRender = NULL;
	m_pAVT->Destroy(env);
	m_pAVT = NULL;
	delete(this);
	return ;
}


jclass   CESAVTransport_Android::classJavaDMRService = NULL;
jmethodID CESAVTransport_Android::methIdSetDataSource = NULL;
jmethodID CESAVTransport_Android::methIdStop = NULL;
jmethodID CESAVTransport_Android::methIdPlay = NULL;
jmethodID CESAVTransport_Android::methIdPause  = NULL;
jmethodID CESAVTransport_Android::methIdSeek = NULL;
jmethodID CESAVTransport_Android::methIdGetPos = NULL;
jobject   CESAVTransport_Android::m_jCallback = NULL;

CESAVTransport_Android::CESAVTransport_Android(JNIEnv *env,IUpnpDevice* pDevice):
m_thizUDN(NULL),
uri(NULL),
duration(NULL),
m_pIUpnp(pDevice)
{	
	jclass classJavaDMRService  = (jclass)env->FindClass(JAVA_DMRCALLBACK_CLASS_NAME);

	
	methIdSetDataSource = env->GetMethodID(classJavaDMRService,"setDataSource", "(Ljava/lang/String;)I");
	
	methIdStop = env->GetMethodID(classJavaDMRService,"stop","()I");
	
		
	methIdPlay = env->GetMethodID(classJavaDMRService,"play","()I");
		
	methIdPause = env->GetMethodID(classJavaDMRService,"pause","()I");
		
	methIdSeek = env->GetMethodID(classJavaDMRService,"seek","(I)I");
		
	methIdGetPos = env->GetMethodID(classJavaDMRService,"getPosition","()I");

}
CESAVTransport_Android::~CESAVTransport_Android()
{
	FREEARRAY(m_thizUDN);
	FREEARRAY(uri);
	FREEARRAY(duration);
}
void CESAVTransport_Android::Destroy(JNIEnv* env)
{
	if(m_jCallback)
		env->DeleteGlobalRef(m_jCallback);
	m_jCallback = NULL;
	delete(this);

}
void CESAVTransport_Android::SetCallbackObject(JNIEnv*env, jobject jcallback)
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
int CESAVTransport_Android::GetPropSet(IXML_Document*& PropSet)
{
	string resultValue;
	resultValue += AVT_EVENT_HEAD;


	resultValue += AVT_EVENT_END;

	ERROR_TYPE nret = IUpnpUtil::AddToPropertySet(&PropSet,"LastChange",resultValue.c_str());

	if(nret != ERROR_SUCCESS)
		return nret;
	return 0;
}

int CESAVTransport_Android::SetPropSet(const IXML_Document* PropSet)
{
 	CComandNortifyEx nortifyEx;
 	nortifyEx.DevID = m_thizUDN;
 	nortifyEx.ServID = DLNA_AVTRANSPORT_ID;
 	nortifyEx.PropSet = (IXML_Document* )PropSet;
 
 	return m_pIUpnp->ExecuteCommand(nortifyEx);
}
int CESAVTransport_Android::SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata)
{
	if(!m_jCallback)
		return ERROR_INVALID_ACTION;
	
	int nret = 0;
	if(!uri)
	{
		nret = ERROR_INVALID_PARAM;
		return nret;
	}
	
	JNIEnv* env = GetJNIEnv();
	if(!env)
		return ERROR_INVALID_ACTION;

	jstring jstruri = env->NewStringUTF(uri);

	nret = env->CallIntMethod(m_jCallback,methIdSetDataSource,jstruri);
	
bail:
	Trace(
		"SetAVTransportURI to %s,result=%d\n",uri?uri:"Null",nret);

	DetachJNIThread();

	return nret;
}

int CESAVTransport_Android::Stop(ui4 instanceid)
{
	if(!m_jCallback)
		return ERROR_INVALID_ACTION;

	JNIEnv* env = GetJNIEnv();
	if(!env)
		return ERROR_INVALID_ACTION;

	int nret = 0;

	jint id = instanceid;

	nret = env->CallIntMethod(m_jCallback,methIdStop);

bail:
	Trace(
		"Stop result=%d\n",nret);

	DetachJNIThread();

	return nret;
}
int CESAVTransport_Android::Play(ui4 instanceid,icstring speed)
{
	if(!m_jCallback)
		return ERROR_INVALID_ACTION;

	JNIEnv* env = GetJNIEnv();
	if(!env)
		return ERROR_INVALID_ACTION;

	int nret = 0;

	jint id = instanceid;

	nret = env->CallIntMethod(m_jCallback,methIdPlay);
	if(nret == -1)
		nret = ERROR_INVALID_ACTION;

bail:
	Trace(
		"Play result=%d\n",nret);

	DetachJNIThread();

	return nret;
}
int CESAVTransport_Android::Pause(ui4 instanceid)
{
	if(!m_jCallback)
		return ERROR_INVALID_ACTION;

	JNIEnv* env = GetJNIEnv();
	if(!env)
		return ERROR_INVALID_ACTION;

	int nret = 0;

	jint id = instanceid;

	nret = env->CallIntMethod(m_jCallback,methIdPause);
	if(nret == -1)
		nret = ERROR_INVALID_ACTION;

	Trace(
		"Pause result=%d\n",nret);

	DetachJNIThread();

	return nret;
}

int CESAVTransport_Android::Seek(ui4 instanceid,icstring unit,icstring target)
{
	if(!m_jCallback)
		return ERROR_INVALID_ACTION;

	JNIEnv* env = GetJNIEnv();
	if(!env)
		return ERROR_INVALID_ACTION;
	
	int ntarget = atoi(target);

	int nret = env->CallIntMethod(m_jCallback,methIdSeek,ntarget);
	
	Trace(
		"Seek result=%d\n",time);

	DetachJNIThread();
	
	return 0;

}
int CESAVTransport_Android::GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
					istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount)
{
	if(!m_jCallback)
		return ERROR_INVALID_ACTION;

	JNIEnv* env = GetJNIEnv();
	if(!env)
		return ERROR_INVALID_ACTION;

	int time = env->CallIntMethod(m_jCallback,methIdGetPos);

	if(time == -1)
	{
		time =  ERROR_INVALID_ACTION;
	}
	else
	{
		Trace(
			"GetPositionInfo result=%d\n",time);

		time = time/1000;
		int seconds = time % 60;
		time = time / 60;
		int minutes = time % 60;
		int hours = time / 60;

		char tmp[256]={'\0'};
		snprintf(tmp,256,"%2d:%2d:%2d",hours,minutes,seconds);
		for(int i =0; i < 256 && tmp[i] != '\0'; i++)
		{
		   if(tmp[i] == ' ')
		   	   tmp[i] = '0';
		}

		reltime = strdup((char*)tmp);
		abstime = strdup((char*)tmp);

		relcount = abscount = 0x7fffffff;

		track = 1;
		SAFEDUP(trackduration,duration);
		trackmetadata = strdup("NOT_IMPLEMENTED");

		SAFEDUP(trackuri,uri);

		time = 0;
	}
	
	
	DetachJNIThread();

	return time;

}
int CESAVTransport_Android::NortifyStateChange(JNIEnv*env,jobject jstateObject)
{
	static jfieldID javtUriId = NULL;
	static jfieldID jstateId = NULL;
	static jfieldID jstatusId = NULL;
	static jfieldID jdurationId = NULL;
	if(javtUriId == NULL)
	{
		jclass thizclass = env->GetObjectClass(jstateObject);
		javtUriId = env->GetFieldID(thizclass,"urlString","Ljava/lang/String;");
		jstateId = env->GetFieldID(thizclass,"stateString","Ljava/lang/String;");
		jstatusId = env->GetFieldID(thizclass,"statusString","Ljava/lang/String;");
		jdurationId = env->GetFieldID(thizclass,"durationString","Ljava/lang/String;");

	}
	jstring javtUri = (jstring)env->GetObjectField(jstateObject,javtUriId);
	jstring jstate = (jstring)env->GetObjectField(jstateObject,jstateId);
	jstring jstatus = (jstring)env->GetObjectField(jstateObject,jstatusId);
	jstring jduration = (jstring)env->GetObjectField(jstateObject,jdurationId);

	const char* stravturi  = env->GetStringUTFChars(javtUri,NULL);
	if(stravturi && strlen(stravturi) == 0)
	{
		env->ReleaseStringUTFChars(javtUri,stravturi);
		stravturi = NULL;
	}

	const char* strstate  = env->GetStringUTFChars(jstate,NULL);
	if(strstate && strlen(strstate) == 0)
	{
		env->ReleaseStringUTFChars(jstate,strstate);
		strstate = NULL;
	}

	const char* strstatus  = env->GetStringUTFChars(jstatus,NULL);
	if(strstatus && strlen(strstatus) == 0)
	{
		env->ReleaseStringUTFChars(jstatus,strstatus);
		strstatus = NULL;
	}

	const char* strduration  = env->GetStringUTFChars(jduration,NULL);
	if(strduration && strlen(strduration) == 0)
	{
		env->ReleaseStringUTFChars(jduration,strduration);
		strduration = NULL;
	}

#define TEMP_BUF_SIZE (512)

	char tmp[TEMP_BUF_SIZE] = {'\0'};
	string strResult;

	strResult += AVT_EVENT_HEAD;
	snprintf(tmp,TEMP_BUF_SIZE,AVT_INSTANCE_HEAD,"0");
	strResult+=tmp;
	memset(tmp,0,TEMP_BUF_SIZE);


	if(strstate)
	{
		snprintf(tmp,TEMP_BUF_SIZE,AVT_EVENT_ITEM,"TransportState",strstate);
		strResult+=tmp;
		memset(tmp,0,TEMP_BUF_SIZE);
	}

	if(strstatus)
	{
		snprintf(tmp,TEMP_BUF_SIZE,AVT_EVENT_ITEM,"TransportStatus",strstatus);
		strResult+=tmp;
		memset(tmp,0,TEMP_BUF_SIZE);
		if(strcmp(strstatus,"PLAYING") == 0 && uri)
		{
			snprintf(tmp,TEMP_BUF_SIZE,AVT_EVENT_ITEM,"CurrentTrackURI",strstatus);
			strResult+=tmp;
			memset(tmp,0,TEMP_BUF_SIZE);

			snprintf(tmp,TEMP_BUF_SIZE,AVT_EVENT_ITEM,"CurrentTrack","1");
			strResult+=tmp;
			memset(tmp,0,TEMP_BUF_SIZE);
		}
	}
	if(stravturi)
	{
		snprintf(tmp,TEMP_BUF_SIZE,AVT_EVENT_ITEM,"AVTransportURI",stravturi);
		strResult+=tmp;
		memset(tmp,0,TEMP_BUF_SIZE);
		FREEARRAY(uri);
		SAFEDUP(uri,stravturi);
	}



	if(strduration)
	{
		snprintf(tmp,TEMP_BUF_SIZE,AVT_EVENT_ITEM,"CurrentTrackDuration",strduration);
		strResult+=tmp;
		memset(tmp,0,TEMP_BUF_SIZE);
		FREEARRAY(duration);
		SAFEDUP(duration,strduration);
	}


	strResult += AVT_INSTANCE_END;
	strResult += AVT_EVENT_END;
	IXML_Document *PropSet = NULL;

	int nret = 0;
	
	IUpnpUtil::AddToPropertySet(&PropSet,"LastChange",strResult.c_str());
	if(PropSet)
	{
		SetPropSet(PropSet);
		Trace("NortifyStateChange result= %d\n",nret);
		//ixmlDocument_free(PropSet);
	}
	else
	{
		Trace("SetProp error\n");
		nret = -1;
	}

	if(stravturi)
	{
		env->ReleaseStringUTFChars(javtUri,stravturi);
		stravturi = NULL;
	}

	if(strstate)
	{
		env->ReleaseStringUTFChars(jstate,strstate);
		strstate = NULL;
	}

	if(strstatus)
	{
		env->ReleaseStringUTFChars(jstatus,strstatus);
		strstatus = NULL;
	}
	if(strduration)
	{
		env->ReleaseStringUTFChars(jduration,strduration);
		strduration = NULL;
	}

	

	return nret;
}




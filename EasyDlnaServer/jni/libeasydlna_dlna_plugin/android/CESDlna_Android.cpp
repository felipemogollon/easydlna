#define LOG_TAG "easydlna-dlna_android"

#include "CESDlna_Android.h"

#include "CESDMServer_Android.h"
#include "CESDMRender_Android.h"
#include "CESDMController_Android.h"

#define JAVA_EASYDLNAUTIL_CLASS_NAME "com/easydlna/application/EasyDlnaUtil"
#define JAVA_DLNASERVICE_CLASS_NAME_DMP "com/easydlna/dlna/service/DlnaService"
#define JAVA_DLNASERVICE_CLASS_NAME_DMS "com/easydlna/dlna/service/DlnaService"

//#define PLUGIN_DIR "/system/lib"
#define MODEL_NAME "easydlna"

jmethodID MetaDataInfo::jConstructId = NULL;
jfieldID MetaDataInfo::jId = NULL;
jfieldID MetaDataInfo::jParentId = NULL;
jfieldID MetaDataInfo::jChildCount = NULL;
jfieldID MetaDataInfo::jtitle = NULL;
jfieldID MetaDataInfo::jprotocalInfo = NULL;
jfieldID MetaDataInfo::jresourceUrl = NULL;
jfieldID MetaDataInfo::jresourceRtspUrl = NULL;
jfieldID MetaDataInfo::jthumbnailUrl = NULL;
jfieldID MetaDataInfo::jmediaClass = NULL;
jfieldID MetaDataInfo::jSize = NULL;
jfieldID MetaDataInfo::jrealPath = NULL;
jfieldID MetaDataInfo::jTotalMatches = NULL;


CDlnaService::CDlnaService():
m_pDlnaFrame(NULL),
m_pDMC(NULL),
m_pDMR(NULL),
m_pDMS(NULL)
{
}
CDlnaService::~CDlnaService()
{
	if(m_pDlnaFrame)
	{
		IDlnaFrame::DestroyDlnaFrame(m_pDlnaFrame);
		m_pDlnaFrame = NULL;
	}

}
void CDlnaService::InitContentInfo(JNIEnv* env)
{
    jclass jContentInfoClass = env->FindClass(JAVA_CONTENT_CLASS_NAME);

	MetaDataInfo::jId = env->GetFieldID(jContentInfoClass,"id",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jParentId = env->GetFieldID(jContentInfoClass,"parentId",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jChildCount = env->GetFieldID(jContentInfoClass,"childCount","I");
	MetaDataInfo::jtitle = env->GetFieldID(jContentInfoClass,"title",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jprotocalInfo = env->GetFieldID(jContentInfoClass,"protocalInfo",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jresourceUrl = env->GetFieldID(jContentInfoClass,"resourceUrl",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jresourceRtspUrl = env->GetFieldID(jContentInfoClass,"resourceRtspUrl",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jthumbnailUrl = env->GetFieldID(jContentInfoClass,"thumbnailUrl",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jmediaClass = env->GetFieldID(jContentInfoClass,"mediaClasString",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jSize = env->GetFieldID(jContentInfoClass,"size","J");
	MetaDataInfo::jrealPath = env->GetFieldID(jContentInfoClass,"realPath",JAVA_STRING_CLASS_NAME);
	MetaDataInfo::jConstructId = env->GetMethodID(jContentInfoClass,"<init>","()V");
	MetaDataInfo::jTotalMatches = env->GetFieldID(jContentInfoClass,"totalMatches","I");

	return;
}
int CDlnaService::CreateDlnaFrame(const char* strip)
{
	if(!m_pDlnaFrame)
		m_pDlnaFrame =	IDlnaFrame::Instantiate("libeasydlna_dlna_plugin.so",strip,0);
	if(m_pDlnaFrame == NULL)
	{
		Trace(
			"fail to init DlnaFrame from %s at %s\n","libeasydlna_dlna_plugin.so",strip);
		return -1;
	}
	return 0;
}
int CDlnaService::Destroy(JNIEnv *env)
{
	DestroyDMC(env);
	DestroyDMR(env);
	DestroyDMS(env);
	FREEARRAY(CESDMRender_Android::m_RenderName);
	FREEARRAY(CESDMServer_Android::m_ServerName);	
	delete(this);
	return 0;
}
int CDlnaService::CreateDMC(JNIEnv* env)
{
	if(m_pDMC)
		return 0;
	g_pDlnaService->InitContentInfo(env);
	m_pDMC = new CESDMController_Android;
	m_pDMC->InitDMC(env,m_pDlnaFrame);		
	return 0;
}
int CDlnaService::DestroyDMC(JNIEnv* env)
{
	if(!m_pDMC)
	{
		return 0;
	}
	m_pDMC->DestroyDMC(env);
	m_pDMC = NULL;		
	return 0;
}
int CDlnaService::CreateDMR(JNIEnv* env)
{
	if(m_pDMR)
		return 0;
	g_pDlnaService->InitContentInfo(env);
	m_pDMR = new CESDMRender_Android;
	m_pDMR->InitDMR(env,m_pDlnaFrame);

	return 0;
}
int CDlnaService::DestroyDMR(JNIEnv* env)
{
	if(!m_pDMR)
	{
		return 0;
	}
	
	m_pDMR->DestroyDMR(env);
	m_pDMR = NULL;

	return 0;
}

int CDlnaService::CreateDMS(JNIEnv* env)
{
	if(m_pDMS)
		return 0;
	g_pDlnaService->InitContentInfo(env);
	m_pDMS = new CESDMServer_Android;
	
	
	m_pDMS->InitDMS(env,m_pDlnaFrame);
	return 0;
}
int CDlnaService::DestroyDMS(JNIEnv* env)
{
	if(!m_pDMS)
	{
		return 0;
	}
	m_pDMS->DestroyDMS(env);
	m_pDMS = NULL;
	
	return 0;
}

JavaVM* g_jvm = NULL;
CDlnaService* g_pDlnaService = NULL;

int startDLNA(JNIEnv*env,jclass jthiz,jstring jstrIp,jint port)
{
	if(g_pDlnaService)		
		return 0;	
	
	int nret = 0;
	icstring strip = env->GetStringUTFChars(jstrIp,NULL);
	if(!strip)
		return -1;
	
	g_pDlnaService = new CDlnaService;

	nret = g_pDlnaService->CreateDlnaFrame(strip);
	if(nret != 0)
	{
		g_pDlnaService->Destroy(env);		
		g_pDlnaService = NULL;
	}
	env->ReleaseStringUTFChars(jstrIp,strip);
	return nret;
}
int stopDLNA(JNIEnv*env,jclass jthiz)
{
	if(g_pDlnaService)
	{
		g_pDlnaService->Destroy(env);		
		g_pDlnaService = NULL;	
	}
	
	return 0;

}
jint RegisterNativeMethods(JNIEnv* env,const char* pstrClassName,JNINativeMethod* pMethodArray,int nMethodCount)
{	
	jclass clazz = env->FindClass(pstrClassName);	
	if(clazz)		
		return env->RegisterNatives(clazz,pMethodArray,nMethodCount);	
	return JNI_ERR;
}
JNIEnv * GetJNIEnv()
{	
	JNIEnv* env = NULL;	
	g_jvm->AttachCurrentThread(&env,NULL);	
	return env;
}
void DetachJNIThread()
{	
	g_jvm->DetachCurrentThread();
}
//add more code to enable DMS/DMP/DMC' start and stop function here
static JNINativeMethod gMethods[] = {
		{"native_startDLNA", "(Ljava/lang/String;I)I",(void *)startDLNA},	
		{"native_stopDLNA","()I",(void *)stopDLNA},
};
jint JNI_OnLoad(JavaVM* vm,void * reserved)
{	
	JNIEnv *env = NULL;	
	jint result = -1;	
	if(vm->GetEnv((void**)&env,JNI_VERSION_1_4) != JNI_OK)	
	{		
		Trace("ERROR:GetEnv failed\n");		
		goto bail;
	}	
	if(RegisterNativeMethods(env,JAVA_EASYDLNAUTIL_CLASS_NAME,gMethods,2) != 0)	
	{		
		Trace("fail to register Easyshare native methods\n");
		env->ExceptionClear();
		goto bail;
	}
	if(CESDMController_Android::RegisterDMCService(env) !=0 )	
	{	
		Trace("fail to register DMC native methods\n");
		env->ExceptionClear();
		goto bail;
	}
	if(CESDMRender_Android::RegisterDMRService(env) !=0 )
	{
		Trace("fail to register DMR native methods\n");
		env->ExceptionClear();
		goto bail;
	}
	if(CESDMServer_Android::RegisterDMSService(env) !=0 )
	{
		Trace("fail to register DMS native methods\n");
		env->ExceptionClear();
		goto bail;
	}
	result = JNI_VERSION_1_4;	
	Trace("easydlna_android_dlna_jni has been loaded success\n");	
	g_jvm = vm;

bail:	
	return result;
}


#ifndef __CESDLNA_ANDROID_H__
#define __CESDLNA_ANDROID_H__
#ifdef USE_NDK
	#include <jni.h>
#else
	#include "JNIHelp.h"
#endif
#include "IDlnaFrame.h"
/*#include "IDMRender.h"*/
/*#include "IDMController.h"*/
/*#include "IDMServer.h"*/

class CESDMController_Android;
class CESDMRender_Android;
class CESDMServer_Android;

#define JAVA_CONTENT_CLASS_NAME "com/easydlna/dlna/service/ContentInfo"


#define JAVA_STRING_CLASS_NAME "Ljava/lang/String;"


#define JAVA_DMCSERVICE_CLASS_NAME "com/easydlna/dlna/service/DMCService"

#define JAVA_DLNADEVICE_CLASS_NAME "com/easydlna/dlna/service/DlnaDevice"

#define JAVA_DMCCALLBACK_CLASS_NAME "com/easydlna/dlna/service/DMCService$IDMCObserver"

#define JAVA_DMSSERVICE_CLASS_NAME "com/easydlna/dlna/service/DMSService"

#define JAVA_DMSCALLBACK_CLASS_NAME "com/easydlna/dlna/service/DMSService$IDMSObserver"


#define CONTAINER_TYPE_STRORAGE "object.container.storageFolder"
#define CONTAINER_TYPE "object.container"


struct MetaDataInfo
{
	char* pmyId;
	char* pparentId;
	int childCount;
	bool modifiable;
	bool searchable;
	bool bDirectoy;

	char* pTitle;
	char* pProtocalInfo;
	char* pResourceUrl;
	char* pResourceRtspUrl;
	char* pThumbnailUrl;
	char* pMediaclass;
	int64_t size;
	int updateId;

	static jmethodID jConstructId;
	static jfieldID jId;
	static jfieldID jParentId;
	static jfieldID jChildCount;
	static jfieldID jTotalMatches;
	static jfieldID jtitle;
	static jfieldID jprotocalInfo;
	static jfieldID jresourceUrl;
    static jfieldID jresourceRtspUrl;
	static jfieldID jthumbnailUrl;
	static jfieldID jmediaClass;
	static jfieldID jrealPath;
	static jfieldID jSize;

	MetaDataInfo()
	{
		pmyId=pparentId= NULL;
		childCount = updateId = 0;
		modifiable = searchable = bDirectoy = false;

		pTitle = pProtocalInfo = pResourceUrl = pResourceRtspUrl = NULL;
		pMediaclass = NULL;
		pThumbnailUrl = NULL;
		size = 0;


	}



};

class CDlnaService
{
public:
	CDlnaService();
	~CDlnaService();
	void InitContentInfo(JNIEnv* env);
	int CreateDlnaFrame(const char* strip);
	int Destroy(JNIEnv *env);

	int CreateDMC(JNIEnv* env);
	int DestroyDMC(JNIEnv* env);

	int CreateDMR(JNIEnv* env);
	int DestroyDMR(JNIEnv* env);

	int CreateDMS(JNIEnv* env);
	int DestroyDMS(JNIEnv* env);

public:
	IDlnaFrame* m_pDlnaFrame;
	CESDMController_Android* m_pDMC;
	CESDMRender_Android* m_pDMR;
	CESDMServer_Android * m_pDMS;
};
jint RegisterNativeMethods(JNIEnv* env,const char* pstrClassName,
									JNINativeMethod* pMethodArray,int nMethodCount);
JNIEnv * GetJNIEnv();
void DetachJNIThread();
extern CDlnaService* g_pDlnaService;
extern JavaVM* g_jvm;
#endif

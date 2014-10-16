#ifndef __CETDEFINE_H__
#define __CETDEFINE_H__

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#ifdef OS_ANDROID
	#include "upnpconfig.h"
	#include "upnp.h"
	#include "UpnpGlobal.h"
	#include "ithread.h"
	#include "ixml.h"
	#include "upnptools.h"
	#include "ThreadPool.h"
	#include "FreeList.h"
#ifdef USE_NDK
	#include <list>
	using std::list; 

	#include <string>
	using std::string; 

	#include <map>
	#include <utility>
	using std::map;
	using std::make_pair;
	using std::pair; 
	#include <android/log.h>
#else
	#include "stlport/list"
	using std::list; 

	#include "stlport/string"
	using std::string; 

	#include "stlport/map"
	#include "stlport/utility"
	using std::map;
	using std::make_pair;
	using std::pair; 
	#include "utils/Log.h"
#endif
	
#else
	#include <upnpconfig.h>
	#include <upnp.h>
	#include <UpnpGlobal.h>
	#include <ithread.h> 
	#include <ixml.h> 
	#include <upnptools.h> 
	#include <ThreadPool.h> 
	#include <FreeList.h>

	#include <list>
	using std::list; 
	#include <string>
	using std::string; 
	#include <map>
	#include <utility>
	using std::map;
	using std::make_pair;
	using std::pair; 

#endif
#ifndef LOG_TAG
	#define LOG_TAG NULL
#endif

#ifndef LOGE
#define LOGE(fmt,args...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,fmt,##args)
#define LOGD(fmt,args...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,fmt,##args)
#define LOGI(fmt,args...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,fmt,##args)
#define LOGW(fmt,args...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,fmt,##args)
#define Trace LOGE
#endif

#ifndef IN
#define IN
#define OUT
#endif 

#ifndef NULL
#define NULL (0)
#endif 

#ifndef FREE
#define FREE(ptr) \
	do \
{ \
	if(ptr) \
	free(ptr); \
	ptr = NULL; \
}while(0)

#define FREEARRAY(ptr) FREE(ptr)

#define DELETE(ptr) \
	do \
{ \
	if(ptr) \
	delete (ptr); \
	ptr = NULL;	 \
}while(0)

#define DELETEARRAY(ptr) \
	do \
{ \
	if(ptr) \
	delete[] (ptr); \
	ptr = NULL;	 \
}while(0)


#define SAFEDUP(src,dst) \
	do \
{ \
	if(dst) src = strdup(dst);\
else src = NULL; \
} while(0) 

#endif 

enum ERROR_TYPE 
{	
	ERROR_SUCCESS    =      0,
	ERROR_INVALID_HANDLE =  -100,
	ERROR_INVALID_PARAM   = -101,
	ERROR_OUTOF_HANDLE   =  -102,


	ERROR_OUTOF_CONTEXT   = -103,

	ERROR_OUTOF_MEMORY    = -104,
	ERROR_INIT           =  -105,

	ERROR_BUFFER_TOO_SMALL =-106,
	ERROR_INVALID_DESC     =-107,
	ERROR_INVALID_URL      =-108,
	ERROR_INVALID_SID      =-109,
	ERROR_INVALID_DEVICE   =-110,
	ERROR_INVALID_SERVICE  =-111,
	ERROR_BAD_RESPONSE     =-113,
	ERROR_BAD_REQUEST      =-114,
	ERROR_INVALID_ACTION   =-115,
	ERROR_FINISH           =-116,
	ERROR_INIT_FAILED      =-117,
	ERROR_URL_TOO_BIG      =-118,
	ERROR_BAD_HTTPMSG      =-119,
	ERROR_ALREADY_REGISTERED =-120,
	ERROR_NETWORK_ERROR    =-200,
	ERROR_SOCKET_WRITE     =-201,
	ERROR_SOCKET_READ      =-202,
	ERROR_SOCKET_BIND      =-203,
	ERROR_SOCKET_CONNECT   =-204,
	ERROR_OUTOF_SOCKET     =-205,
	ERROR_LISTEN           =-206,
	ERROR_TIMEDOUT         =-207,
	ERROR_SOCKET_ERROR	    =-208,
	ERROR_FILERROR_WRITERROR_ERROR =-209,
	ERROR_CANCELED         =-210,
	ERROR_EVENT_PROTOCOL   =-300,
	ERROR_SUBSCRIBERROR_UNACCEPTED =-301,
	ERROR_UNSUBSCRIBERROR_UNACCEPTED =-302,
	ERROR_NOTIFY_UNACCEPTED      =-303,
	ERROR_INVALID_ARGUMENT       =-501,
	ERROR_FILERROR_NOT_FOUND         =-502,
	ERROR_FILERROR_READ_ERROR        =-503,
	ERROR_EXT_NOT_XML            =-504,

	ERROR_NO_WEB_SERVER         =-505,
	ERROR_OUTOF_BOUNDS	      =-506,
	ERROR_NOT_FOUND	      =-507,
	ERROR_INTERNAL_ERROR         =-911,
	SOAP_ERROR_INVALID_ACTION    =401,
	SOAP_ERROR_INVALID_ARGS      =402,
	SOAP_ERROR_OUT_OF_SYNC       =403,
	SOAP_ERROR_INVALID_VAR       =404,
	SOAP_ERROR_ACTION_FAILED     =501, 
}; 

enum EventType
{

	ET_CONTROL_ACTION_REQUEST,

	ET_CONTROL_ACTION_COMPLETE,

	ET_CONTROL_GET_VAR_REQUEST,

	ET_CONTROL_GET_VAR_COMPLETE,

	ET_DISCOVERY_ADVERTISEMENT_ALIVE,

	ET_DISCOVERY_ADVERTISEMENT_BYEBYE,

	ET_DISCOVERY_SEARCH_RESULT,

	ET_DISCOVERY_SEARCH_TIMEOUT,

	ET_EVENT_SUBSCRIPTION_REQUEST,

	ET_EVENT_RECEIVED,

	ET_EVENT_RENEWAL_COMPLETE,

	ET_EVENT_SUBSCRIBE_COMPLETE,

	ET_EVENT_UNSUBSCRIBE_COMPLETE,

	ET_EVENT_AUTORENEWAL_FAILED,

	ET_EVENT_SUBSCRIPTION_EXPIRED,

	//Self added event type;
	ET_EVENT_TICKCOUNT,

};
class CEvent; 
typedef int  (*EventCallBack) (IN EventType eventType,IN CEvent& event,IN void *Cookie); 


struct CDevice
{
	char* pstrUDN; 
	char* pstrDescDocURL; 

	char* pstrFriendlyName;
	char* pstrManufacturer;

	char* pstrPresURL ;
	char* pstrRelativePresUrl;
	char* pstrDeviceType;
	char*      pBaseUrl;

	int  AdvrTimeOut;
	int  onLine;
	int  errorCode;
	CDevice()
	{
		pstrUDN = pstrDescDocURL = pstrFriendlyName  =NULL;
		pstrDeviceType = pstrManufacturer = pstrPresURL = pstrRelativePresUrl = NULL;
		pBaseUrl = NULL;
		AdvrTimeOut = -1;
		onLine = TRUE;
		errorCode = 0;
	}
	CDevice(CDevice & other)
	{
		SAFEDUP(pstrUDN,other.pstrUDN);
		SAFEDUP(pstrDescDocURL,other.pstrDescDocURL);
		SAFEDUP(pstrFriendlyName,other.pstrFriendlyName);
		SAFEDUP(pstrDeviceType,other.pstrDeviceType);
		SAFEDUP(pstrRelativePresUrl,other.pstrRelativePresUrl);
		SAFEDUP(pstrManufacturer,other.pstrManufacturer);
		SAFEDUP(pstrPresURL,other.pstrPresURL);
		SAFEDUP(pBaseUrl,other.pBaseUrl);
		AdvrTimeOut = other.AdvrTimeOut;
		onLine		= other.onLine;
		errorCode	= other.errorCode;
	}
	~CDevice()
	{
		FREE(pstrUDN);
		FREE(pstrDescDocURL);
		FREE(pstrFriendlyName);
		FREE(pstrRelativePresUrl);
		FREE(pstrDeviceType);
		FREE(pstrManufacturer);
		FREE(pstrPresURL);
		FREE(pBaseUrl);
	}
}; 

struct WebFileInfo
{
	int   index;
	char* url;
	char* realpath;
	long long   readpos;
	long long  filesize;
	int   closed;
	WebFileInfo()
	{
		index = -1;
		url = realpath = NULL;
		readpos = 0;
		filesize = 0LL;
		closed = 0;
	}

};
//#ifdef OS_ANDROID
//typedef List<CDevice*>CListDevices;
//#else
typedef list<CDevice*>CListDevices;
//#endif 












#endif

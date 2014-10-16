#ifndef __IUpnpUtil_H__
#define __IUpnpUtil_H__

#include <pthread.h>
#include "IUpnpDefine.h"


class IUpnpUtil
{

	// log relative
public:
	enum TRACE_LEVEL
	{
		TRACE_VERBOSE = 0,
		TRACE_DEBUG   = 1,
		TRACE_WARNING = 2,
		TRACE_ERROR   = 3,
		TRACE_INFO	  = 4,
	};
	//static void TraceOut(TRACE_LEVEL level , const char* fmt, ...);
	//Read write lock relative
public:	
	class CRWLock
	{
	public:
		CRWLock();
		~CRWLock();
	public:
		int ReadLock();
		int ReadUnlock();
		int WriteLock();
		int WriteUnlock();

	private:
		pthread_mutex_t m_readMutex;
		pthread_mutex_t m_writeMutex;
		int m_readCounter;
	};




	//quick rw lock relative
	class CQuickRWLock
	{
	public:
		CQuickRWLock(CRWLock* pLock, BOOL bLockRead = TRUE)
		{
			m_pRWLock = pLock;
			m_bLockRead = bLockRead;
			if(bLockRead)
				m_pRWLock->ReadLock();
			else
				m_pRWLock->WriteLock();
		}
		~CQuickRWLock()
		{
			if(m_bLockRead)
				m_pRWLock->ReadUnlock();
			else
				m_pRWLock->WriteUnlock();
		}
	private:
		CRWLock* m_pRWLock;
		BOOL m_bLockRead;
	};



//parse xml relative
public:
	static char * GetElementValue(IN IXML_Element *element);

	static IXML_NodeList *GetFirstServiceList(IN IXML_Document *doc); 

	static char *GetFirstDocumentItem(IN IXML_Document *doc, IN const char *item); 

	static char *GetFirstElementItem(IN IXML_Element *element, IN const char *item);

	static int FindAndParseService (IN IXML_Document *DescDoc,IN char* location,IN char *serviceType,
									 OUT char **serviceId,OUT char **eventURL,OUT char **controlURL);

public:
	static ERROR_TYPE ResolveURL(IN const char * BaseURL,IN const char * RelURL, OUT char ** AbsURL);

	
	static IXML_Document* MakeAction(IN const char * ActionName, IN const char * ServType,IN int NumArg,
										 IN const char * Arg,IN ...);

	static ERROR_TYPE AddToAction(IN OUT IXML_Document ** ActionDoc, IN const char * ActionName, 
									  IN const char * ServType, IN const char * ArgName,IN const char * ArgVal);

	static IXML_Document* MakeActionResponse(IN const char * ActionName,IN const char * ServType,
												IN int NumArg,IN const char * Arg, IN ...);

	static ERROR_TYPE AddToActionResponse(IN OUT IXML_Document ** ActionResponse, IN const char * ActionName,
										IN const char * ServType,IN const char * ArgName,IN const char * ArgVal);

	static ERROR_TYPE AddToPropertySet(IN OUT IXML_Document **PropSet,IN const char * ArgName, IN const char * ArgVal);

	static IXML_Document* CreatePropertySet(IN int NumArg,IN const char* Arg,IN ...);

	static const char * GetErrorMessage(int errorcode);
	
	static timespec* getTimeout(struct timespec* spec, unsigned numMs);

	static const char* GetValueFromXML(IXML_Document * pDocXml,const char* valuename);
	static const char* GetAttributeValueFromXML(IXML_Document* pDocXml,const char* valuename,
												const char* attributeName);
};	

/*#ifndef TRACE_UTIL
	#define TRACE_UTIL
	#define TRACEERROR (IUpnpUtil::TRACE_ERROR)
	#define TRACEINFO (IUpnpUtil::TRACE_INFO)
	#define TraceError(fmt,args...) IUpnpUtil::TraceOut(TRACEERROR,fmt,##args)
	#define TraceInfo(fmt,args...) IUpnpUtil::TraceOut(TRACEINFO,fmt,##args)
	#define Trace TraceError
#endif*/


#define LOCKREAD(mutlock) \
	do \
{ \
	(mutlock)->ReadLock(); \
}while(0)

#define UNLOCKREAD(mutlock)\
	do \
{ \
	(mutlock)->ReadUnlock();\
}while(0)


#define LOCKWRITE(mutlock) \
	do \
{ \
	(mutlock)->WriteLock(); \
}while(0)

#define UNLOCKWRITE(mutlock)\
	do \
{ \
	(mutlock)->WriteUnlock();\
}while(0)

#define QUICKWRITELOCK(pLock) \
	IUpnpUtil::CQuickRWLock quickWriteLock(pLock,FALSE)

#define QUICKREADLOCK(pLock) \
	IUpnpUtil::CQuickRWLock quickReadLock(pLock,TRUE)





#endif


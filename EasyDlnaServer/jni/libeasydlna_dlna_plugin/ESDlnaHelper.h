#ifndef __ESDLNAHELPER_H__
#define __ESDLNAHELPER_H__

#include "IDlnaService.h"
#include <time.h>


#define UNKNOWN_SERVICE_ID "Unknown Service ID"
#define UNKNOWN_SERVICE_ACTION "Unknown Service Action"

#define INVALID_VARIABLE "Invalid Variable"

inline char* GetUDNByRunTime(const char* baseuuid)
{
	time_t nowTime = time(NULL);
	if(nowTime == -1)
	{
		return NULL;
	}
	else
	{
		struct tm * needTime = gmtime(&nowTime);
		char * uuidBuf = (char*)malloc(sizeof(char)*64);
		memset (uuidBuf,0,sizeof(char)*64);

		snprintf (uuidBuf, sizeof(char)*64, "%s-%02x%02x%02x%02x%02x%02x", baseuuid,
			(needTime->tm_year), (needTime->tm_mon), (needTime->tm_mday),
			(needTime->tm_hour), (needTime->tm_min), (needTime->tm_sec));

		Trace("create udn by time =[%s]\n",uuidBuf);

		return uuidBuf;
	}
}


inline BOOL HitStateValTable(const char* valName,StateVarableEntry table[],int len)
{
	for (int i = 0; i < len; i++)
	{
		if(strcmp(table[i].VarName,valName) == 0)
			return TRUE;
	}

	return FALSE;

}
inline void NoActiveErrorPrint(const char* actionName)
{
	Trace(
		"no active device specified for action[%s]\n",actionName);
	return;
}
inline void NoServiceStateValErrorPrint(const char* valName)
{
	Trace(
		"no state variable name called [%s] exists for this service\n",valName);
	return;
}

inline void PrintGetVarError(CEventStateVarRequest & getStateVarRequest,const char* pErrorDescription)
{
	strncpy(getStateVarRequest.ErrStr,pErrorDescription,LINE_SIZE);
	getStateVarRequest.ErrCode = UPNP_SOAP_E_INVALID_VAR;
}
inline void PrintActionError(CEventActionRequest & actionRequest,const char* pErrorDescription)
{
	strncpy(actionRequest.ErrStr,pErrorDescription,LINE_SIZE);
	actionRequest.ActionResult = NULL;
	actionRequest.ErrCode = UPNP_SOAP_E_INVALID_ACTION;

	return;
}


#endif


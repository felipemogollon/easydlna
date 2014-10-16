#include "CESConnectionManager.h"
#include "ESDlnaHelper.h"

CESConnetionManager::CESConnetionManager()
:m_strActive(NULL)
{
	m_protocolinfo =	"http-get:*:audio/mpegurl:*,";
	m_protocolinfo += "http-get:*:audio/mp3:*,";
	m_protocolinfo += "http-get:*:audio/mpeg:*,";
	m_protocolinfo += "http-get:*:audio/x-ms-wma:*,";
	m_protocolinfo += "http-get:*:audio/wma:*,";
	m_protocolinfo += "http-get:*:audio/mpeg3:*,";
	m_protocolinfo += "http-get:*:video/x-ms-wmv:*,";
	m_protocolinfo += "http-get:*:video/x-ms-asf:*,";
	m_protocolinfo += "http-get:*:video/x-ms-avi:*,";
	m_protocolinfo += "http-get:*:video/mpeg:*";
}

CESConnetionManager::~CESConnetionManager()
{
	#ifdef OS_ANDROID
		
	#else
		m_protocolinfo.clear();
	#endif
	FREEARRAY(m_strActive);
}
int	CESConnetionManager::GetPropSet(IXML_Document*& PropSet)
{
	//#ifdef OS_ANDROID
	//ERROR_TYPE nret = IUpnpUtil::AddToPropertySet(&PropSet,"SinkProtocolInfo",(const char*)m_protocolinfo);
	//#else
	ERROR_TYPE nret = IUpnpUtil::AddToPropertySet(&PropSet,"SinkProtocolInfo",m_protocolinfo.c_str());	
	//#endif
	if(nret != ERROR_SUCCESS)
		return nret;
	
	nret = IUpnpUtil::AddToPropertySet(&PropSet,"CurrentConnetcionIDs","0");
	if(nret != ERROR_SUCCESS)
		return nret;

	nret = IUpnpUtil::AddToPropertySet(&PropSet,"A_ARG_TYPE_Direction","Output");
	if(nret != ERROR_SUCCESS)
		return nret;

	return 0;


}
istring CESConnetionManager::GetStatusValue(const char* valName)
{
	if(!HitStateValTable(valName,CMServiceTable,VAR_TABLE_LENGTH(CMServiceTable)))
	{
		Trace(
			"no state variable name called [%s] exists for this service\n",valName);

		return NULL;
	}
	
	istring retVal = NULL;

	if(strcmp(valName,"SourceProtocolInfo") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"SinkProtocolInfo") == 0)
	{
		//#ifdef OS_ANDROID
		//retVal = strdup((const char*)m_protocolinfo);
		//#else
		retVal = strdup(m_protocolinfo.c_str());	
		//#endif
	}
	else if(strcmp(valName,"CurrentConnectionIDs") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"A_ARG_TYPE_ConnectionStatus") == 0)
	{
		retVal = strdup("Unknown");
	}
	else if(strcmp(valName,"A_ARG_Type_ConnectionManager") == 0)
	{
		SAFEDUP(retVal,m_strActive);
	}
	else if(strcmp(valName,"A_ARG_TYPE_Direction") == 0)
	{
		retVal = strdup("Output");
	}
	else if(strcmp(valName,"A_ARG_TYPE_ProtocolInfo") == 0)
	{
		//#ifdef OS_ANDROID
		//retVal = strdup((const char*)m_protocolinfo);
		//#else
		retVal = strdup(m_protocolinfo.c_str());
		//#endif
	}
	else if(strcmp(valName,"A_ARG_TYPE_ConnectionID") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"A_ARG_TYPE_AVTransportID") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"A_ARG_TYPE_RcsID") == 0)
	{
		retVal = strdup("-1");
	}
	
	return retVal;
}
int CESConnetionManager::GetProtocalInfo(istring & source,istring &sink)
{
	source = strdup("");
	
	//#ifdef OS_ANDROID
	//sink = strdup((const char*)m_protocolinfo);
	//#else
	sink = strdup(m_protocolinfo.c_str());
	//#endif
	
	return 0;
}

int CESConnetionManager::GetCurrentConnectionIDs(istring & connectionids)
{
	connectionids = strdup("");
	return 0;
}

int CESConnetionManager::GetCurrentConnectionInfo(ui4 connectionid,ui4& rcsid,ui4&avtransportid,
															   istring& protocolinfo,istring& peerconnectionmanager,
															   istring& peerconnectionid,istring&direction,
															   istring&status)
{

	rcsid = -1;
	avtransportid = -1;
	protocolinfo = strdup("");
	peerconnectionmanager = strdup("");
	peerconnectionid = strdup("-1");
	direction = strdup("Input");
	status = strdup("Unkown");


	return 0;
}


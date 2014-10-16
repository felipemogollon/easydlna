#include "IUpnp.h"
#include "CUpnpControl.h"
#include "CUpnpDevice.h"


IUpnpControlPoint* g_control = NULL;
int g_controlCount = 0;

//we can support multiple devices now

ERROR_TYPE IUpnp::Start(const char* ip,unsigned short usPort)
{
	return (ERROR_TYPE)UpnpInit(ip,usPort);
}
ERROR_TYPE IUpnp::Finish()
{
	return (ERROR_TYPE)UpnpFinish();
}
IUpnpControlPoint* IUpnp::CreateUpnpControlPoint()
{
	if(!g_control)
		g_control =  new CUpnpControl;
	
	++g_controlCount;
	return (IUpnpControlPoint* )g_control;
}
ERROR_TYPE IUpnp::DestroyIUpnp(IUpnp* pDestroyObject)
{
	if(pDestroyObject == g_control)
	{
		if((--g_controlCount) == 0)
			DELETE(g_control);
	}
	else 
	{
		DELETE(pDestroyObject);
	}

	return ERROR_SUCCESS;
}
IUpnpDevice* IUpnp::CreateUpnpDevice()
{
	return (IUpnpDevice*)new CUpnpDevice;
}
char* IUpnp::GetUpnpIp()
{
	return UpnpGetServerIpAddress();
}
unsigned short IUpnp::GetUpnpPort()
{
	return UpnpGetServerPort();
}


ERROR_TYPE IUpnp::SetMaxContentLength(size_t contentLength)
{
	return (ERROR_TYPE)UpnpSetMaxContentLength(contentLength);
}
ERROR_TYPE IUpnp::DownloadUrlItem(IN const char *url,OUT char **outBuf,OUT char *contentType)
{
	return (ERROR_TYPE)UpnpDownloadUrlItem(url,outBuf,contentType);
}

ERROR_TYPE IUpnp::OpenHttpGet(IN const char *url,IN OUT void **handle,IN OUT char **contentType,
							  IN OUT int *contentLength, IN OUT int *httpStatus,IN int timeout)
{
	return (ERROR_TYPE)UpnpOpenHttpGet(url,handle,contentType,contentLength,httpStatus,timeout);
}							  

ERROR_TYPE IUpnp::OpenHttpGetProxy(IN const char *url,IN const char *proxy_str,IN OUT void **handle,
								   IN OUT char **contentType,IN OUT int *contentLength, 
								   IN OUT int *httpStatus,IN int timeout)
{
	return (ERROR_TYPE)UpnpOpenHttpGetProxy(url,proxy_str,handle,contentType,contentLength,httpStatus,timeout);
}

ERROR_TYPE IUpnp::OpenHttpGetEx(IN const char *url, IN OUT void **handle, IN OUT char **contentType, 
								IN OUT int *contentLength, IN OUT int *httpStatus,	IN int lowRange,
								IN int highRange,IN int timeout)
{
	return (ERROR_TYPE)UpnpOpenHttpGetEx(url,handle,contentType,contentLength,httpStatus,lowRange,highRange,timeout);
}

ERROR_TYPE IUpnp::ReadHttpGet(IN void *handle,IN OUT char *buf,IN OUT unsigned int *size,IN int timeout)
{
	return (ERROR_TYPE)UpnpReadHttpGet(handle,buf,(size_t*)size,timeout);
}
ERROR_TYPE IUpnp::HttpGetProgress(IN void *handle,OUT unsigned int *length,OUT unsigned int *total)
{
	return (ERROR_TYPE)UpnpHttpGetProgress(handle,(size_t*)length,(size_t*)total);
}
ERROR_TYPE IUpnp::CancelHttpGet(IN void *handle)
{
	return (ERROR_TYPE)UpnpCancelHttpGet(handle);
}
ERROR_TYPE IUpnp::CloseHttpGet(IN void *handle)
{
	return (ERROR_TYPE)UpnpCloseHttpGet(handle);
}

ERROR_TYPE IUpnp::OpenHttpPost(IN const char *url, IN OUT void **handle,IN const char *contentType, 
							   IN int contentLength, IN int timeout)
{
	return (ERROR_TYPE)UpnpOpenHttpPost(url,handle,contentType,contentLength,timeout);
}

ERROR_TYPE IUpnp::WriteHttpPost(IN void *handle,IN char *buf, IN unsigned int *size,IN int timeout)
{
	return (ERROR_TYPE)UpnpWriteHttpPost(handle,buf,(size_t*)size,timeout);
}
ERROR_TYPE IUpnp::CloseHttpPost(IN void *handle,IN OUT int *httpStatus,IN int timeout)
{
	return (ERROR_TYPE)UpnpCloseHttpPost(handle,httpStatus,timeout);
}
ERROR_TYPE IUpnp::DownloadXmlDoc(IN const char *url,OUT IXML_Document **xmlDoc)
{
	return (ERROR_TYPE)UpnpDownloadXmlDoc(url,xmlDoc);
}


ERROR_TYPE IUpnp::SetWebServerRootDir(IN const char* rootDir)
{
	return (ERROR_TYPE)UpnpSetWebServerRootDir(rootDir);
}
ERROR_TYPE IUpnp::SetVirtualDirCallbacks(IN UpnpVirtualDirCallbacks *callbacks)
{
	//return (ERROR_TYPE)UpnpSetVirtualDirCallbacks(callbacks);
	UpnpVirtualDir_set_GetInfoCallback(callbacks->get_info);
	UpnpVirtualDir_set_OpenCallback(callbacks->open);
	UpnpVirtualDir_set_ReadCallback(callbacks->read);
	UpnpVirtualDir_set_WriteCallback(callbacks->write);
	UpnpVirtualDir_set_SeekCallback(callbacks->seek);
	UpnpVirtualDir_set_CloseCallback(callbacks->close);
	return (ERROR_TYPE)0;
}

// ERROR_TYPE IUpnp::SetVirtualDirCallbacks(IN UpnpVirtualDirCallbacks *callbacks)
// {
// 	return (ERROR_TYPE)UpnpSetVirtualDirCallbacks(callbacks);
// }

ERROR_TYPE IUpnp::EnableWebserver(IN int enable)
{
	return (ERROR_TYPE)UpnpEnableWebserver(enable);
}

BOOL IUpnp::IsWebserverEnabled()
{
	return UpnpIsWebserverEnabled() == 1?TRUE:FALSE;
}

ERROR_TYPE IUpnp::AddVirtualDir(const char *dirName)
{
	return (ERROR_TYPE)UpnpAddVirtualDir(dirName);
}

ERROR_TYPE IUpnp::RemoveVirtualDir(IN const char *dirName)
{
	return (ERROR_TYPE)UpnpRemoveVirtualDir(dirName);
}

ERROR_TYPE IUpnp::RemoveAllVirtualDirs()
{
	UpnpRemoveAllVirtualDirs();
	
	return ERROR_SUCCESS;
}


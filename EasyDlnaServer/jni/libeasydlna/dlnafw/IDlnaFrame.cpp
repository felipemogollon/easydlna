#define LOG_TAG "easydlna-dlnaframe"
#include "IDlnaFrame.h"
#include "IUpnpUtil.h"
#include "IUpnp.h"
#include <dirent.h>
#include <dlfcn.h>

struct CPlugIn
{
	char* pName;
	void* pHandle;

	GetSupportedDlnaComponentsPtr  ptrGetComponents;
	GetDMControllerPtr				ptrGetDMController;
	GetDMPlayerPtr					ptrGetDMPlayer;
	GetDMRenderPtr					ptrGetDMRender;
	GetDMServerPtr					ptrGetDMServer;

	CPlugIn()
	{
		pName = NULL;
		pHandle = NULL;
	}
	int IsMatch(const char* pModelName)
	{
		const char* pMyModelName = pName+strlen("lib");
		return strncmp(pModelName,pMyModelName,strlen(pModelName));
	}
	~CPlugIn()
	{
		dlclose(pHandle);
		FREE(pName);
	}
};
//#ifdef OS_ANDROID
//	typedef List<CPlugIn*>CListPlugin;
//#else
	typedef list<CPlugIn*>CListPlugin;
//#endif

int dllFilter(const struct dirent* a)
{
	#ifdef PLUGIN_MATCH_NAME
	#undef PLUGIN_MATCH_NAME
	#endif
	#define PLUGIN_MATCH_NAME "dlna_plugin.so"
	#define PLGUIN_NAME_LEN strlen(PLUGIN_MATCH_NAME)
	
	const char* dlName = a->d_name;
	if(strlen(dlName) < 3 || strlen(dlName) < PLGUIN_NAME_LEN)	
		return 0;

	
	const char* dlPrefixName = dlName + strlen(dlName)-PLGUIN_NAME_LEN;
   
	
	if(strncmp("lib",a->d_name,3) == 0 && 
	   strncmp(PLUGIN_MATCH_NAME,dlPrefixName,PLGUIN_NAME_LEN) == 0)
	{
		
		Trace("Find a matched plugin %s\n",dlName);
		return 1;
	}
	return 0;
}
#if defined(OS_ANDROID) || defined(PLATFORM_64)
int dllComapre(const void **a, const void **b)
#else
int dllComapre(const dirent **a, const dirent **b)
#endif
{
	
	return strcmp(((struct dirent*)(*a))->d_name,((struct dirent*)(*b))->d_name);
}



class CDlnaFrame:public IDlnaFrame
{
public:
	CDlnaFrame(const char* pPlugindirName);
	virtual	~CDlnaFrame(){FREE(m_dirName);}

public:
	virtual int	GetSupportedDlnaComponnents(const char* modelName,char** & pComponentNameList);

	virtual IDMController* GetDMController(const char* modelName);

	virtual IDMPlayer* GetDMPlayer(const char* modelName);

	virtual IDMRender* GetDMRender(const char* modelName);

	virtual IDMServer* GetDMServer(const char* modelName);
	int  LoadSpecifiedPlugin(const char* pluginName);
	void LoadAllPlugins();
	virtual void Release();

private:
	int  LoadAllFunctions(CPlugIn *pPlugin);
	CPlugIn* FindPluginByModelName(const char* modelName);
private:
	char* m_dirName;
	
	CListPlugin m_ListPlugin;
};

CDlnaFrame::CDlnaFrame(const char* pPlugindirName)
 :m_dirName(NULL)
{

	if(pPlugindirName != NULL)
		m_dirName = strdup(pPlugindirName);

}
int CDlnaFrame::LoadAllFunctions(CPlugIn *pPlugin)
{
	pPlugin->ptrGetComponents = (GetSupportedDlnaComponentsPtr)dlsym(pPlugin->pHandle,
																FUNC_GetSupportedDlnaComponents);
	if(pPlugin->ptrGetComponents == NULL)
		return -1;

	pPlugin->ptrGetDMController = (GetDMControllerPtr)dlsym(pPlugin->pHandle,FUNC_GetDMController);
	if(pPlugin->ptrGetDMController == NULL)
		return -1;

	pPlugin->ptrGetDMPlayer = (GetDMPlayerPtr)dlsym(pPlugin->pHandle,FUNC_GetDMPlayer);
	if(pPlugin->ptrGetDMPlayer == NULL)
		return -1;

	pPlugin->ptrGetDMRender = (GetDMRenderPtr)dlsym(pPlugin->pHandle,FUNC_GetDMRender);
	if(pPlugin->ptrGetDMRender == NULL)
		return -1;

	pPlugin->ptrGetDMServer = (GetDMServerPtr)dlsym(pPlugin->pHandle,FUNC_GetDMServer);
	if(pPlugin->ptrGetDMController == NULL)
		return -1;
	
	return 0;
}
int CDlnaFrame::LoadSpecifiedPlugin(const char* pluginName)
{
  int nret = -1;
  void *pHandle = dlopen(pluginName,/*RTLD_LAZY*/RTLD_NOW|RTLD_GLOBAL);
  if(!pHandle)
  {
	Trace("fail to load specified plugin: %s\n",pluginName);
  }
  else
  {	
		CPlugIn* pPlugin = new CPlugIn;
		pPlugin->pHandle = pHandle;
		if(LoadAllFunctions(pPlugin) != 0)
		{
			Trace("fail to load functions from %s\n",pluginName);
					
			DELETE(pPlugin);
		}
		else
		{
			Trace("succeed to load %s\n",pluginName);

			 pPlugin->pName = strdup(pluginName);
			 m_ListPlugin.push_back(pPlugin);
			 nret = 0;
		}
				
	}
  return nret;
}

void CDlnaFrame::LoadAllPlugins()
{
	struct dirent **pDirNameList = NULL;
	
	#if defined(OS_ANDROID) || defined(PLATFORM_64)
	int nret = scandir(m_dirName,&pDirNameList,dllFilter,(int (*)(const dirent**, const dirent**))dllComapre);	
	#else
	int nret = scandir(m_dirName,&pDirNameList,dllFilter,(int (*)(const void*, const void*))dllComapre);
	#endif
	if(nret > 0)
	{
		int prefixlen = strlen(m_dirName);
		BOOL bNeedAppendSlash = (m_dirName[strlen(m_dirName)-1] != '/');
		for (int i = 0; i < nret; i++)
		{
			char* pFullPath = NULL;
			if(bNeedAppendSlash)
				pFullPath = new char[prefixlen + 1 + strlen(pDirNameList[i]->d_name) + 1];
			else
				pFullPath = new char[prefixlen + strlen(pDirNameList[i]->d_name) + 1];

			memset(pFullPath,0,sizeof(pFullPath));

			strcat(pFullPath,m_dirName);
			if(bNeedAppendSlash)
				strcat(pFullPath,"/");

			strcat(pFullPath,pDirNameList[i]->d_name);

			void *pHandle = dlopen(pFullPath,/*RTLD_LAZY*/RTLD_NOW|RTLD_GLOBAL);
			if(!pHandle)
			{
				Trace(
						"fail to load %s\n",pDirNameList[i]->d_name);
			}
			else
			{	
				CPlugIn* pPlugin = new CPlugIn;
				pPlugin->pHandle = pHandle;
				if(LoadAllFunctions(pPlugin) != 0)
				{
					Trace(
						"fail to load functions from %s\n",pDirNameList[i]->d_name);
					
					DELETE(pPlugin);
				}
				else
				{
					Trace(
						"succeed to load %s\n",pDirNameList[i]->d_name);

					pPlugin->pName = strdup(pDirNameList[i]->d_name);

					m_ListPlugin.push_back(pPlugin);
				}
				
			}
			
			FREE(pDirNameList[i]);
			DELETEARRAY(pFullPath);
		}	

		FREEARRAY(pDirNameList);
	}
	else
		Trace("fail to load plugin from %s\n",m_dirName);

}


void CDlnaFrame::Release()
{
	CListPlugin::iterator itbegin = m_ListPlugin.begin();
	CListPlugin::iterator itend = m_ListPlugin.end();
	for (itbegin;itbegin!=itend;++itbegin)
	{
		CPlugIn *pPlugin = (CPlugIn*)(*itbegin);
		/*if(pPlugin->pHandle)
			dlclose(pPlugin->pHandle);*/
		DELETE(pPlugin);
	}
	
	m_ListPlugin.clear();
}

static CDlnaFrame* g_pFrame = NULL;
CPlugIn* CDlnaFrame::FindPluginByModelName(const char* modelName)
{
	CListPlugin::iterator itbegin = m_ListPlugin.begin();
	CListPlugin::iterator itend = m_ListPlugin.end();

	for (itbegin;itbegin!=itend;++itbegin)
	{
		CPlugIn *pPlugin = (CPlugIn*)(*itbegin);
		if(pPlugin->IsMatch(modelName) == 0)
			return pPlugin;
	}
	return NULL;
}
IDlnaFrame* IDlnaFrame::Instantiate(const char* pPluginDirName,const char*pIp,unsigned short port)
{
	if(g_pFrame)
	{
	  Trace("Instantiate:already created\n");
	  return g_pFrame;
	}

	if(IUpnp::Start(pIp,port) != ERROR_SUCCESS)
	{
		Trace("fail to start Iupnp at [%s@%d]\n",pIp,port);
		return NULL;
	}
	if(strstr(pPluginDirName,".so") != NULL)
	{
		g_pFrame = new CDlnaFrame(NULL);  
		if(g_pFrame->LoadSpecifiedPlugin(pPluginDirName) != 0)
		{
		  DestroyDlnaFrame(g_pFrame);
		  Trace("failed to load specified plugin:%s\n",pPluginDirName);
		  return NULL;
		}
		else
		Trace("Succeed to load specified plugin:%s\n",pPluginDirName);
	}
	else
	{
		g_pFrame = new CDlnaFrame(pPluginDirName);
		g_pFrame->LoadAllPlugins();
	}
	return g_pFrame;
}

ERROR_TYPE  IDlnaFrame::DestroyDlnaFrame(IDlnaFrame* ptrDlnaFrame)
{
	g_pFrame->Release();
	DELETE(g_pFrame);

	IUpnp::Finish();
	
	return ERROR_SUCCESS;
}


int	CDlnaFrame::GetSupportedDlnaComponnents(const char* modelName,char** & pComponentNameList)
{
	CPlugIn *pPlugin = FindPluginByModelName(modelName);
	if(pPlugin == NULL)
		return -1;
	return (*pPlugin->ptrGetComponents)(modelName,pComponentNameList);

}

IDMController* CDlnaFrame::GetDMController(const char* modelName)
{
	CPlugIn *pPlugin = FindPluginByModelName(modelName);
	if(pPlugin == NULL)
		return NULL;
	return (*pPlugin->ptrGetDMController)(modelName);
}

IDMPlayer* CDlnaFrame::GetDMPlayer(const char* modelName)
{
	CPlugIn *pPlugin = FindPluginByModelName(modelName);
	if(pPlugin == NULL)
		return NULL;
	return (*pPlugin->ptrGetDMPlayer)(modelName);
}

IDMRender* CDlnaFrame::GetDMRender(const char* modelName)
{
	CPlugIn *pPlugin = FindPluginByModelName(modelName);
	if(pPlugin == NULL)
		return NULL;
	return (*pPlugin->ptrGetDMRender)(modelName);
}

IDMServer* CDlnaFrame::GetDMServer(const char* modelName)
{
	CPlugIn *pPlugin = FindPluginByModelName(modelName);
	if(pPlugin == NULL)
		return NULL;
	return (*pPlugin->ptrGetDMServer)(modelName);
}

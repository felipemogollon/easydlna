#include "IUpnpUtil.h"
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#ifndef HEADER_LENGTH
#define HEADER_LENGTH 2000
#endif

#ifndef LOG_TAG
#define LOG_TAG NULL  //"easydlna-util"
#endif


static IXML_Document *
makeAction( IN int response,
            IN const char *ActionName,
            IN const char *ServType,
            IN int NumArg,
            IN const char *Arg,
            IN va_list ArgList )
{
    const char *ArgName;
    const char *ArgValue;
    char *ActBuff;
    int Idx = 0;
    IXML_Document *ActionDoc;
    IXML_Node *node;
    IXML_Element *Ele;
    IXML_Node *Txt = NULL;

    if( ActionName == NULL || ServType == NULL ) {
        return NULL;
    }

    ActBuff = ( char * )malloc( HEADER_LENGTH );
    if( ActBuff == NULL ) {
        return NULL;
    }

    if( response ) {
        sprintf( ActBuff,
            "<u:%sResponse xmlns:u=\"%s\">\r\n</u:%sResponse>",
            ActionName, ServType, ActionName );
    } else {
        sprintf( ActBuff,
            "<u:%s xmlns:u=\"%s\">\r\n</u:%s>",
            ActionName, ServType, ActionName );
    }

    if( ixmlParseBufferEx( ActBuff, &ActionDoc ) != IXML_SUCCESS ) {
        free( ActBuff );
        return NULL;
    }

    free( ActBuff );

    if( ActionDoc == NULL ) {
        return NULL;
    }

    if( NumArg > 0 ) {
        //va_start(ArgList, Arg);
        ArgName = Arg;
        for ( ; ; ) {
            ArgValue = va_arg( ArgList, const char * );

            if( ArgName != NULL ) {
                node = ixmlNode_getFirstChild( ( IXML_Node * ) ActionDoc );
                Ele = ixmlDocument_createElement( ActionDoc, ArgName );
                if( ArgValue ) {
                    Txt =
                        ixmlDocument_createTextNode( ActionDoc, ArgValue );
                    ixmlNode_appendChild( ( IXML_Node * ) Ele, Txt );
                }

                ixmlNode_appendChild( node, ( IXML_Node * ) Ele );
            }

            if (++Idx < NumArg) {
                ArgName = va_arg( ArgList, const char * );
            } else {
                break;
            }
        }
        //va_end(ArgList);
    }

    return ActionDoc;
}

IUpnpUtil::CRWLock::CRWLock()
{
	pthread_mutex_init(&m_readMutex,NULL);
	pthread_mutex_init(&m_writeMutex,NULL);
	m_readCounter = 0;
}
IUpnpUtil::CRWLock::~CRWLock()
{
	pthread_mutex_destroy(&m_readMutex);
	pthread_mutex_destroy(&m_writeMutex);	
}
int IUpnpUtil::CRWLock::ReadLock()
{

	pthread_mutex_lock(&m_readMutex);
	if(++m_readCounter > 1)
	{
		pthread_mutex_unlock(&m_readMutex);
		return m_readCounter;
	}
	else
		pthread_mutex_lock(&m_writeMutex);

	pthread_mutex_unlock(&m_readMutex);
	return 1;	
}
int IUpnpUtil::CRWLock::ReadUnlock()
{
	pthread_mutex_lock(&m_readMutex);
	if(--m_readCounter > 0)
	{
		pthread_mutex_unlock(&m_readMutex);
		return m_readCounter;
	}
	else
		pthread_mutex_unlock(&m_writeMutex);

	pthread_mutex_unlock(&m_readMutex);

	return 0;
}
int IUpnpUtil::CRWLock::WriteLock()
{
	pthread_mutex_lock(&m_writeMutex);
	return 0;
}
int IUpnpUtil::CRWLock::WriteUnlock()
{
	pthread_mutex_unlock(&m_writeMutex);
	return 0;	
}



/*void IUpnpUtil::TraceOut(TRACE_LEVEL level , const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
#ifdef OS_ANDROID
	char buf[1024] = {'\0'};
	vsnprintf(buf, 1024, fmt, ap);
	LOGE(buf);
#else
	vfprintf(stderr, fmt,ap);
#endif
	va_end(ap);

	return;
};*/


char * IUpnpUtil::GetElementValue(IN IXML_Element *element)
{
	IXML_Node *child = ixmlNode_getFirstChild((IXML_Node *)element);

	char *temp = NULL;

	if( ( child != 0 ) && (ixmlNode_getNodeType(child) == eTEXT_NODE)) {
		temp = strdup( ixmlNode_getNodeValue(child));
	}

	return temp;
}
IXML_NodeList * IUpnpUtil::GetFirstServiceList(IN IXML_Document *doc)
{
	IXML_NodeList *ServiceList = NULL;
	IXML_NodeList *servlistnodelist = NULL;
	IXML_Node *servlistnode = NULL;

	servlistnodelist =
		ixmlDocument_getElementsByTagName( doc, "serviceList" );
	if( servlistnodelist && ixmlNodeList_length( servlistnodelist ) ) {

		/*
		we only care about the first service list, from the root device 
		*/
		servlistnode = ixmlNodeList_item( servlistnodelist, 0 );

		/*
		create as list of DOM nodes 
		*/
		ServiceList =
			ixmlElement_getElementsByTagName( ( IXML_Element * )
			servlistnode, "service" );
	}

	if( servlistnodelist )
		ixmlNodeList_free( servlistnodelist );

	return ServiceList;
}
char * IUpnpUtil::GetFirstDocumentItem(IN IXML_Document *doc, IN const char *item)
{
	IXML_NodeList *nodeList = NULL;
	IXML_Node *textNode = NULL;
	IXML_Node *tmpNode = NULL;

	char *ret = NULL;

	nodeList = ixmlDocument_getElementsByTagName( doc, ( char * )item );

	if( nodeList ) {
		if( ( tmpNode = ixmlNodeList_item( nodeList, 0 ) ) ) {
			textNode = ixmlNode_getFirstChild( tmpNode );
		    if(textNode != NULL)
					ret = strdup( ixmlNode_getNodeValue( textNode ) );
		}
	}

	if( nodeList )
		ixmlNodeList_free( nodeList );
	return ret;
}
char * IUpnpUtil::GetFirstElementItem(IN IXML_Element *element, IN const char *item)
{
	IXML_NodeList *nodeList = NULL;
	IXML_Node *textNode = NULL;
	IXML_Node *tmpNode = NULL;

	char *ret = NULL;

	nodeList = ixmlElement_getElementsByTagName( element, (char *)item);

	if( nodeList == NULL ) {
		Trace("Error finding %s in XML Node\n", item);
		return NULL;
	}

	if((tmpNode = ixmlNodeList_item( nodeList, 0))  == NULL)
	 {
		Trace("Error finding %s value in XML Node\n", item);
		ixmlNodeList_free( nodeList );
		return NULL;
	}

	textNode = ixmlNode_getFirstChild(tmpNode);
	if(textNode != NULL)
		ret = strdup( ixmlNode_getNodeValue(textNode));

	if( !ret ) {
		Trace("Error allocating memory for %s in XML Node\n",item);
		ixmlNodeList_free( nodeList );
		return NULL;
	}

	ixmlNodeList_free( nodeList );

	return ret;
}
int  IUpnpUtil::FindAndParseService (IN IXML_Document *DescDoc,IN char* location,IN char *serviceType,OUT char **serviceId, 
									 OUT char **eventURL,OUT char **controlURL)
{
	int i,
		length,
		found = 0;
	int ret;
	char *tempServiceType = NULL;
	char *baseURL = NULL;
	char *base;
	char *relcontrolURL = NULL,
		*releventURL = NULL;
	IXML_NodeList *serviceList = NULL;
	IXML_Element *service = NULL;

	baseURL = GetFirstDocumentItem(DescDoc,"URLBase");

	if( baseURL )
		base = baseURL;
	else
		base = location;

	serviceList = GetFirstServiceList(DescDoc);
	length = ixmlNodeList_length(serviceList);
	for( i = 0; i < length; i++ )
	{
		service = (IXML_Element*)ixmlNodeList_item(serviceList, i);
		
		tempServiceType = GetFirstElementItem((IXML_Element*)service,"serviceType");

		if(strcmp(tempServiceType, serviceType) == 0)
		{
			Trace("Found service: %s\n",serviceType);
			
			*serviceId = GetFirstElementItem(service, "serviceId");
			Trace("serviceId: %s\n", (*serviceId));
			
			relcontrolURL = GetFirstElementItem(service,"controlURL");
			releventURL =  GetFirstElementItem(service, "eventSubURL");

			*controlURL = (char*)malloc(strlen( base ) + strlen(relcontrolURL) + 1);
			
			if( *controlURL ) 
			{
				ret = UpnpResolveURL(base, relcontrolURL, *controlURL);
				if( ret != UPNP_E_SUCCESS )
					Trace( "Error generating controlURL from %s + %s\n",base, relcontrolURL);
			}

			*eventURL = (char*)malloc(strlen(base) + strlen(releventURL) + 1);

			if( *eventURL ) 
			{
				ret = UpnpResolveURL(base, releventURL, *eventURL);
				if( ret != UPNP_E_SUCCESS )
					Trace("Error generating eventURL from %s + %s\n", base,releventURL);
			}

			if( relcontrolURL )
				free( relcontrolURL );
			if( releventURL )
				free( releventURL );
			relcontrolURL = releventURL = NULL;

			found = 1;
			break;
		}

		if( tempServiceType )
			free( tempServiceType );
		tempServiceType = NULL;
	}

	if( tempServiceType )
		free( tempServiceType );
	if( serviceList )
		ixmlNodeList_free( serviceList );
	if( baseURL )
		free( baseURL );

	return found;
}



ERROR_TYPE IUpnpUtil::ResolveURL(IN const char * BaseURL,IN const char * RelURL, OUT char ** AbsURL)
{
	int nMallocLen = (strlen(BaseURL) + strlen(RelURL) + 1)*sizeof(char);
	*AbsURL = (char*)malloc(nMallocLen);
	memset(*AbsURL,0,nMallocLen);
	int nret = UpnpResolveURL(BaseURL,RelURL,*AbsURL);
	if(nret != UPNP_E_SUCCESS)
		FREE(*AbsURL);
	return (ERROR_TYPE)nret;
}


IXML_Document* IUpnpUtil::MakeAction(IN const char * ActionName, IN const char * ServType,IN int NumArg,
								 IN const char * Arg,IN ...)
{
	va_list ArgList;
   IXML_Document *out = NULL;

   va_start( ArgList, Arg );
   out = makeAction( 0, ActionName, ServType, NumArg, Arg, ArgList );
   va_end( ArgList );
    
	return out;
}

ERROR_TYPE IUpnpUtil::AddToAction(IN OUT IXML_Document ** ActionDoc, IN const char * ActionName, 
							  IN const char * ServType, IN const char * ArgName,IN const char * ArgVal)
{
	return (ERROR_TYPE)UpnpAddToAction(ActionDoc,ActionName,ServType,ArgName,ArgVal);
}

IXML_Document* IUpnpUtil::MakeActionResponse(IN const char * ActionName,IN const char * ServType,
										 IN int NumArg,IN const char * Arg, IN ...)
{
	va_list ArgList;
   IXML_Document *out = NULL;

   va_start( ArgList, Arg );
   out = makeAction( 1, ActionName, ServType, NumArg, Arg, ArgList );
   va_end( ArgList );

   return out;
}

ERROR_TYPE IUpnpUtil::AddToActionResponse(IN OUT IXML_Document ** ActionResponse, IN const char * ActionName,
									  IN const char * ServType,IN const char * ArgName,IN const char * ArgVal)
{
	return (ERROR_TYPE)UpnpAddToActionResponse(ActionResponse,ActionName,ServType,ArgName,ArgVal);
}

ERROR_TYPE IUpnpUtil::AddToPropertySet(IN OUT IXML_Document **PropSet,IN const char * ArgName, IN const char * ArgVal)
{
	return (ERROR_TYPE)UpnpAddToPropertySet(PropSet,ArgName,ArgVal);
}

IXML_Document* IUpnpUtil::CreatePropertySet(IN int NumArg,IN const char* Arg,IN ...)
{
	 va_list ArgList;
    int Idx = 0;
    char BlankDoc[] = "<e:propertyset xmlns:e=\"urn:schemas-"
        "upnp-org:event-1-0\"></e:propertyset>";
    const char *ArgName,
     *ArgValue;
    IXML_Node *node;
    IXML_Element *Ele;
    IXML_Element *Ele1;
    IXML_Node *Txt;
    IXML_Document *PropSet;

    if( ixmlParseBufferEx( BlankDoc, &PropSet )!= IXML_SUCCESS) 
     {
      return NULL;
	 }

    if( NumArg < 1 ) 
    {
      return NULL;
    }

    va_start( ArgList, Arg );
    ArgName = Arg;

    while( Idx++ != NumArg ) {
        ArgValue = va_arg( ArgList, const char * );

        if( ArgName != NULL /*&& ArgValue != NULL */  ) {
            node = ixmlNode_getFirstChild( ( IXML_Node * ) PropSet );
            Ele1 = ixmlDocument_createElement( PropSet, "e:property" );
            Ele = ixmlDocument_createElement( PropSet, ArgName );
            if( ArgValue ) {
                Txt = ixmlDocument_createTextNode( PropSet, ArgValue );
                ixmlNode_appendChild( ( IXML_Node * ) Ele, Txt );
            }

            ixmlNode_appendChild( ( IXML_Node * ) Ele1,
                                  ( IXML_Node * ) Ele );
            ixmlNode_appendChild( node, ( IXML_Node * ) Ele1 );
        }

        ArgName = va_arg( ArgList, const char * );

    }
    va_end(ArgList);
    
    return PropSet;
}
timespec* IUpnpUtil::getTimeout(struct timespec* spec, unsigned numMs)
{
	struct timeval current;
	gettimeofday(&current, NULL);
	spec->tv_sec = current.tv_sec + ((numMs + current.tv_usec / 1000) / 1000);
	spec->tv_nsec = ((current.tv_usec / 1000 + numMs) % 1000) * 1000000;
	return spec;
}
const char * IUpnpUtil::GetErrorMessage(int errorcode)
{
	return UpnpGetErrorMessage(errorcode);
}
const char* IUpnpUtil::GetAttributeValueFromXML(IXML_Document* pDocXml,const char* valuename,
												const char* attributeName)
{
	IXML_Element* elementResult = 
		ixmlDocument_getElementById(pDocXml,valuename);

	if(!elementResult)
	{
		Trace("fail to parse xml get element [%s]\n",valuename);

		return NULL;
	}
	else
	{
		return ixmlElement_getAttribute(elementResult,attributeName);
	}

}
const char* IUpnpUtil::GetValueFromXML(IXML_Document * pDocXml,const char* valuename)
{
	IXML_Element* elementResult = 
		ixmlDocument_getElementById(pDocXml,valuename);

	if(!elementResult)
	{
		Trace("fail to parse xml get get [%s]\n",valuename);

		return NULL;
	}
	else
	{
		return ixmlNode_getNodeValue(ixmlNode_getFirstChild((IXML_Node*)elementResult));	

	}
}


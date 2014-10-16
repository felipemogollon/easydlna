#ifndef __IDLNACONTENTDIRECTORY_H__
#define __IDLNACONTENTDIRECTORY_H__

#include "IDlnaService.h"

#define DLNA_CONTENTDIRECTORY "urn:schemas-upnp-org:service:ContentDirectory:1"
#define DLNA_CONTENTDIRECTORY_ID "urn:upnp-org:serviceId:ContentDirectory"

#define UPNPCLASS_CONTAINER "object.container"
#define UPNPCLASS_CONTAINER_STORAGE "object.container.storageFolder"

#define UPNPCLASS_ITEM "object.item"
#define UPNPCLASS_ITEM_VIDEO "object.item.videoItem"
#define UPNPCLASS_ITEM_AUDIO "object.item.audioItem"
#define UPNPCLASS_ITEM_PHOTO "object.item.imageItem.photo"
#define UPNPCLASS_ITEM_PLAYLIST "object.item.playlistItem"
#define UPNPCLASS_ITEM_TEXT "object.item.textItem"

#define DIDL_HEAD \
"<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" "\
	"xmlns:dc=\"http://purl.org/dc/elements/1.1/\" " \
	"xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\">" 
#define DIDL_END "</DIDL-Lite>"



#define CONTAINER_HEAD \
	"<container id=\"%lld\" parentID=\"%lld\" childCount=\"%d\" restricted=\"false\">"

#define CONTAINER_HEAD_STRING \
	"<container id=\"%s\" parentID=\"%s\" childCount=\"%d\" restricted=\"false\">"
	
#define CONTAINER_END "</container>"

#define ITEM_HEAD \
"<item id = \"%lld\" parentID= \"%lld\" restricted=\"false\" >"

#define ITEM_HEAD_STRING \
"<item id = \"%s\" parentID= \"%s\" restricted=\"false\" >"

#define ITEM_END "</item>"

#define CLASS_ITEM "<upnp:class>%s</upnp:class>"

#define TITLE_ITEM \
	"<dc:title>%s</dc:title>"

#define RES_ITEM \
	"<res protocolInfo=\"%s\" size=\"%lld\">"\
	"%s"\
	"</res>"

#define ALBUM_ITEM \
	"<upnp:albumArtURI>"\
	"%s"\
	"</upnp:albumArtURI>"

struct EntryResourceInfo
{
	istring contenturi;
	istring importuri;
	istring protocolinfo;

	EntryResourceInfo()
	{
		contenturi = importuri = NULL;
		protocolinfo = NULL;
	}
	~EntryResourceInfo()
	{
		FREEARRAY(contenturi);
		FREEARRAY(importuri);
		FREEARRAY(protocolinfo);
	}
};
struct CDEntry
{
	icstring upnpclass;
	istring title;
	icstring mediaclass;
	i64    objectid;
	i64    parentid;
	bool   searchable;
	
	int	   rescounts;
	EntryResourceInfo** entryres;

	CDEntry()
	{
		upnpclass = NULL;
		title = NULL;
		mediaclass = NULL;
		objectid = parentid = -1LL;
		searchable = true;
		rescounts = 0;
		entryres = NULL;
	}
	~CDEntry()
	{
		FREEARRAY(title);
		for (int i = 0; i < rescounts; i++)
		{
			DELETE(entryres[i]);
		}
		DELETEARRAY(entryres);
		
	}
};

struct MimeType
{
	icstring postfix;
	icstring mediaclass;
	icstring protocolinfo;
};

static StateVarableEntry CDServiceTable[] = \
{
	{"TransferIDs",TRUE},
	{"SystemUpdateID",TRUE},
	{"ContainerUpdateIDs",TRUE},
	{"A_ARG_TYPE_ObjectID",FALSE},
	{"A_ARG_TYPE_Result",FALSE},
	{"A_ARG_TYPE_SearchCriteria",FALSE},
	{"A_ARG_TYPE_BrowseFlag",FALSE},
	{"A_ARG_TYPE_Filter",FALSE},
	{"A_ARG_TYPE_SortCriteria",FALSE},
	{"A_ARG_TYPE_Index",FALSE},
	{"A_ARG_TYPE_Count",FALSE},
	{"A_ARG_TYPE_UpdateID",FALSE},
	{"A_ARG_TYPE_TransferID",FALSE},
	{"A_ARG_TYPE_TransferStatus",FALSE},
	{"A_ARG_TYPE_TransferLength",FALSE},
	{"A_ARG_TYPE_TransferTotal",FALSE},
	{"A_ARG_TYPE_TagValueList",FALSE},
	{"A_ARG_TYPE_URI",FALSE},
	{"SearchCapabilities",FALSE},
	{"SortCapabilities",FALSE}


};

class IDlnaContentDirectory:public IDlnaService
{

public:			
#define ACTION_GETSEARCHCAP			"GetSearchCapabilities"
#define ACTION_GETSORTCAP			"GetSortCapabilities"
#define ACTION_GETSYSTEMUPDATEID	"GetSystemUpdateID"
#define ACTION_BROWSE				"Browse"
#define ACTION_SEARCH				"Search"
#define ACTION_CREATEOBJECT			"CreateObject"
#define ACTION_DESTROYOBJECT		"DestroyObject"
#define ACTION_UPDATEOBJECT			"UpdateObject"
#define ACTION_IMPORTRES			"ImportResource"
#define ACTION_EXPORTRES			"ExportResource"
#define ACTION_STOPTRANSFERRES		"StopTransferResource"
#define ACTION_GETPROGRESS			"GetTransferProgress"
#define ACTION_DELETERES			"DeleteResource"
#define ACTION_CREATEREF			"CreateReference"



#define ACTION_BROWSE_CHILDREN      "BrowseDirectChildren"
#define ACTION_BROWSE_METADATA		"BrowseMetadata"

public:
	IDlnaContentDirectory()
	{
		m_dlnaServiceType = DLNA_ST_CONTENTDIRECTORY;
	}

	virtual ~IDlnaContentDirectory(){};
	
	virtual const char* GetErrorMessage(int errorCode)
	{
		switch(errorCode)
		{
		case 701:
			return "No such object";
		case 702:
			return "Invalid currentTagValue";
		case 703:
			return "Invalid newTagValue";
		case 704:
			return "Required tag";
		case 705:
			return "Read only tag";
		case 706:
			return "Parameter Mismatch";
		case 708:
			return "Unsupported or invalid search criteria";
		case 709:
			return "Unsupported or invalid sort criteria";
			break;
		case 710:
			return "No such container";
		case 711:
			return "Restricted object";
		case 712:
			return "Bad metadata";
		case 713:
			return "Restricted parent object";
		case 714:
			return "No such source resource";
		case 715:
			return "Resources access denied";
		case 716:
			return "Transfer busy";
		case 717:
			return "No such file transfer";
		case 718:
			return "No such destination resource";
		case 719:
			return "Destination resource access denied";
		case 720:
			return "Cannot process the request";
		    break;
		default:
			return IDlnaService::GetErrorMessage(errorCode);
		    break;
		}
	}

public:
	virtual int GetSearchCapabilities(istring & searchcaps) = 0;

	virtual int GetSortCapabilities(istring & sortcaps) = 0;
	
	virtual  int GetSystemUpdateID(ui4 & id) = 0;

	virtual int Browse(icstring id,icstring browseflag,icstring filter,
						ui4 startingindex,ui4 requestedcount,icstring sort,
						istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid) = 0;

	virtual int Search(icstring containerid,icstring searchcriteria,icstring filter,
						ui4 startingindex,ui4 requestedcount,icstring sortcriteria,
						istring& result,ui4& numberreturned,ui4& totalmatches,
						ui4& updateid){return ERROR_INVALID_ACTION;};

	virtual int CreateObject(icstring containerid,icstring elements,
							istring& objectid,istring& result)
						{return ERROR_INVALID_ACTION;};

	virtual int DestroyObject(icstring objectid){return ERROR_INVALID_ACTION;};

	virtual int UpdateObject(icstring objectid,icstring currenttagvalue,
							icstring newtagvalue){return ERROR_INVALID_ACTION;};

	virtual  int ImportResource(icstring sourceuri,icstring destinationuri,
								istring& transferid)
							{return ERROR_INVALID_ACTION;};

	virtual  int ExportResource(icstring sourceuri,icstring destinationuri,
								istring& transferid)
							{return ERROR_INVALID_ACTION;};

	virtual	 int StopTransferResource(icstring transferid){return ERROR_INVALID_ACTION;};

	virtual  int GetTransferProgress(icstring transferid,istring& transferstatus,
								     istring& transferlength,istring& transfertotal)
							{return ERROR_INVALID_ACTION;};

	virtual	 int DeleteResource(icstring resourceuri){return ERROR_INVALID_ACTION;};

	virtual  int CreateReference(icstring containerid,icstring objectid,istring& newid)
							{return ERROR_INVALID_ACTION;};

	

};

#define CDS_DESCRIPTION \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">"\
"<specVersion>"\
"<major>1</major>"\
"<minor>0</minor>"\
"</specVersion>"\
"<actionList>"\
"<action>" \
"<name>Browse</name>"  \
"<argumentList>" \
"<argument>" \
"<name>ObjectID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>BrowseFlag</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_BrowseFlag</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Filter</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>StartingIndex</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RequestedCount</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>SortCriteria</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Result</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NumberReturned</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TotalMatches</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>UpdateID</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>CreateObject</name>"  \
"<argumentList>" \
"<argument>" \
"<name>ContainerID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Elements</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>ObjectID</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Result</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>CreateReference</name>"  \
"<argumentList>" \
"<argument>" \
"<name>ContainerID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>ObjectID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NewID</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>DeleteResource</name>"  \
"<argumentList>" \
"<argument>" \
"<name>ResourceURI</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>DestroyObject</name>"  \
"<argumentList>" \
"<argument>" \
"<name>ObjectID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>ExportResource</name>"  \
"<argumentList>" \
"<argument>" \
"<name>SourceURI</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>DestinationURI</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TransferID</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>GetSearchCapabilities</name>"  \
"<argumentList>" \
"<argument>" \
"<name>SearchCaps</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>SearchCapabilities</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>GetSortCapabilities</name>"  \
"<argumentList>" \
"<argument>" \
"<name>SortCaps</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>SortCapabilities</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>GetSystemUpdateID</name>"  \
"<argumentList>" \
"<argument>" \
"<name>Id</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>SystemUpdateID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>GetTransferProgress</name>"  \
"<argumentList>" \
"<argument>" \
"<name>TransferID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TransferStatus</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TransferStatus</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TransferLength</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TransferLength</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TransferTotal</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TransferTotal</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>ImportResource</name>"  \
"<argumentList>" \
"<argument>" \
"<name>SourceURI</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>DestinationURI</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TransferID</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>Search</name>"  \
"<argumentList>" \
"<argument>" \
"<name>ContainerID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>SearchCriteria</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_SearchCriteria</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Filter</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>StartingIndex</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RequestedCount</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>SortCriteria</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Result</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NumberReturned</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TotalMatches</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>UpdateID</name>"  \
"<direction>out</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>StopTransferResource</name>"  \
"<argumentList>" \
"<argument>" \
"<name>TransferID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"<action>" \
"<name>UpdateObject</name>"  \
"<argumentList>" \
"<argument>" \
"<name>ObjectID</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentTagValue</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TagValueList</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NewTagValue</name>"  \
"<direction>in</direction>"  \
"<relatedStateVariable>A_ARG_TYPE_TagValueList</relatedStateVariable>"  \
"</argument>" \
"</argumentList>"  \
"</action>"  \
"</actionList>" \
"<serviceStateTable>"\
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_BrowseFlag</name>"  \
"<dataType>string</dataType>" \
"<allowedValueList>" \
"<allowedValue>BrowseMetadata</allowedValue>" \
"<allowedValue>BrowseDirectChildren</allowedValue>" \
"</allowedValueList>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_Index</name>"  \
"<dataType>ui4</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_TransferStatus</name>"  \
"<dataType>string</dataType>" \
"<allowedValueList>" \
"<allowedValue>COMPLETED</allowedValue>" \
"<allowedValue>ERROR</allowedValue>" \
"<allowedValue>IN_PROGRESS</allowedValue>" \
"<allowedValue>STOPPED</allowedValue>" \
"</allowedValueList>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>SearchCapabilities</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>SortCapabilities</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_SortCriteria</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_Filter</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_UpdateID</name>"  \
"<dataType>ui4</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"yes\">"\
"<name>TransferIDs</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_TransferTotal</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_ObjectID</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"yes\">"\
"<name>ContainerUpdateIDs</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_TransferLength</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_Count</name>"  \
"<dataType>ui4</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_TransferID</name>"  \
"<dataType>ui4</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"yes\">"\
"<name>SystemUpdateID</name>"  \
"<dataType>ui4</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_Result</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_TagValueList</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_URI</name>"  \
"<dataType>uri</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">"\
"<name>A_ARG_TYPE_SearchCriteria</name>"  \
"<dataType>string</dataType>" \
"</stateVariable>" \
"</serviceStateTable>" \
"</scpd>"




#define CDS_DESCRIPTION_LEN strlen(CDS_DESCRIPTION)







#endif

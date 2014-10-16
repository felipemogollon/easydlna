#ifndef __IDLNACONNECTIONMANAGER_H__
#define __IDLNACONNECTIONMANAGER_H__

#include "IDlnaService.h"

#define DLNA_CONNECTIONMANAGER "urn:schemas-upnp-org:service:ConnectionManager:1"
#define DLNA_CONNECTIONMANAGER_ID "urn:upnp-org:serviceId:ConnectionManager"

static StateVarableEntry CMServiceTable[] = \
{
	{"SourceProtocolInfo",TRUE},
	{"SinkProtocolInfo",TRUE},
	{"CurrentConnectionIDs",TRUE},
	{"A_ARG_TYPE_ConnectionStatus",FALSE},
	{"A_ARG_TYPE_ConnectionManager",FALSE},
	{"A_ARG_TYPE_Direction",FALSE},
	{"A_ARG_TYPE_ProtocolInfo",FALSE},
	{"A_ARG_TYPE_ConnectionID",FALSE},
	{"A_ARG_TYPE_AVTransportID",FALSE},
	{"A_ARG_TYPE_RcsID",FALSE},
};


class IDlnaConnetionManager:public IDlnaService
{

#define ACTION_GETPROTOCALINFO			"GetProtocolInfo"
#define ACTION_PREPAREFORCONNECTION		"PrepareForConnection"
#define ACTION_CONNECTIONCOMPLETE		"ConnectionComplete"
#define ACTION_GETCONNECTIONIDS			"GetCurrentConnectionIDs"
#define ACTION_GETCONNECTIONINFO		 "GetCurrentConnectionInfo"

public:
	IDlnaConnetionManager(){m_dlnaServiceType = DLNA_ST_CONNECTIONMANAGER;};	
	virtual ~IDlnaConnetionManager(){};

public:
	virtual const char* GetErrorMessage(int errorCode)
	{
		switch(errorCode)
		{
		case 701:
			return "Incompatible protocol info";
			break;
		case 702 :
			return "Incompatible directions";
			break;
		case 703:
			return "Insufficient network resources";
		    break;
		case 704:
			return "Local restrictions";
		    break;
		case 705:
			return "Access denied";
			break;
		case 706:
			return "Invalid connection reference";
		case 707:
			return "Not in network";
			break;
		default:
			return IDlnaService::GetErrorMessage(errorCode);
		    break;
		}
	}


public:
	virtual int GetProtocalInfo(istring & source,istring &sink) = 0;

	virtual int PrepareForConnection(icstring remoteprotocalinfo,icstring peerconnctionmanager,
									icstring peerconnectionid,icstring direction,
									ui4 & connectionid,ui4 avtransportid,ui4& rcsid)
							{return ERROR_INVALID_ACTION;};

	virtual int ConnectionComplete(ui4 connectionid){return ERROR_INVALID_ACTION;};

	virtual int GetCurrentConnectionIDs(istring & connectionids) = 0;

	virtual int GetCurrentConnectionInfo(ui4 connectionid,ui4& rcsid,ui4&avtransportid,
										istring& protocolinfo,istring& peerconnectionmanager,
										istring& peerconnectionid,istring&direction,
										istring&status) = 0;

};

#define CMS_DESCRIPTION \
	"<?xml version=\"1.0\" encoding=\"utf-8\"?>" \
	"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">" \
	"  <specVersion>" \
	"    <major>1</major>" \
	"    <minor>0</minor>" \
	"  </specVersion>" \
	"  <actionList>" \
	"    <action>" \
	"      <name>GetCurrentConnectionInfo</name>" \
	"      <argumentList>" \
	"        <argument>" \
	"          <name>ConnectionID</name>" \
	"          <direction>in</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>RcsID</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>AVTransportID</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>ProtocolInfo</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>PeerConnectionManager</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>PeerConnectionID</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>Direction</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>Status</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>" \
	"        </argument>" \
	"      </argumentList>" \
	"    </action>" \
	"    <action>" \
	"      <name>GetProtocolInfo</name>" \
	"      <argumentList>" \
	"        <argument>" \
	"          <name>Source</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>SourceProtocolInfo</relatedStateVariable>" \
	"        </argument>" \
	"        <argument>" \
	"          <name>Sink</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>SinkProtocolInfo</relatedStateVariable>" \
	"        </argument>" \
	"      </argumentList>" \
	"    </action>" \
	"    <action>" \
	"      <name>GetCurrentConnectionIDs</name>" \
	"      <argumentList>" \
	"        <argument>" \
	"          <name>ConnectionIDs</name>" \
	"          <direction>out</direction>" \
	"          <relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>" \
	"        </argument>" \
	"      </argumentList>" \
	"    </action>" \
	"  </actionList>" \
	"  <serviceStateTable>" \
	"    <stateVariable sendEvents=\"no\">" \
	"      <name>A_ARG_TYPE_ProtocolInfo</name>" \
	"      <dataType>string</dataType>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"no\">" \
	"      <name>A_ARG_TYPE_ConnectionStatus</name>" \
	"     <dataType>string</dataType>" \
	"      <allowedValueList>" \
	"        <allowedValue>OK</allowedValue>" \
	"        <allowedValue>ContentFormatMismatch</allowedValue>" \
	"        <allowedValue>InsufficientBandwidth</allowedValue>" \
	"        <allowedValue>UnreliableChannel</allowedValue>" \
	"        <allowedValue>Unknown</allowedValue>" \
	"      </allowedValueList>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"no\">" \
	"      <name>A_ARG_TYPE_AVTransportID</name>" \
	"      <dataType>i4</dataType>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"no\">" \
	"      <name>A_ARG_TYPE_RcsID</name>" \
	"      <dataType>i4</dataType>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"no\">" \
	"      <name>A_ARG_TYPE_ConnectionID</name>" \
	"      <dataType>i4</dataType>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"no\">" \
	"      <name>A_ARG_TYPE_ConnectionManager</name>" \
	"      <dataType>string</dataType>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"yes\">" \
	"      <name>SourceProtocolInfo</name>" \
	"      <dataType>string</dataType>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"yes\">" \
	"      <name>SinkProtocolInfo</name>" \
	"      <dataType>string</dataType>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"no\">" \
	"      <name>A_ARG_TYPE_Direction</name>" \
	"      <dataType>string</dataType>" \
	"      <allowedValueList>" \
	"        <allowedValue>Input</allowedValue>" \
	"        <allowedValue>Output</allowedValue>" \
	"      </allowedValueList>" \
	"    </stateVariable>" \
	"    <stateVariable sendEvents=\"yes\">" \
	"      <name>CurrentConnectionIDs</name>" \
	"      <dataType>string</dataType>" \
	"    </stateVariable>" \
	"  </serviceStateTable>" \
	"</scpd>"

#define CMS_DESCRIPTION_LEN  strlen (CMS_DESCRIPTION)









#endif


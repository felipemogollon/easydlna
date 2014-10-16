#include "IDlnaAVTransport.h"
#include "IDlnaConnectionManager.h"
#include "IDlnaRenderingControl.h"
#include "IDlnaContentDirectory.h"

#include "CESDlnaDeviceHandler.h"
#include "ESDlnaHelper.h"




#define CHECK_ERROR(checkvalue,actionRequest,realname) \
	do  \
{ \
	if(!checkvalue) \
	{ \
	Trace("invalid parameter name of[%s]\n",realname);\
	actionRequest.ErrCode = ERROR_INVALID_PARAM; \
	return ERROR_SUCCESS; \
	} \
} while(0)


CESDlnaDeviceHandler::CESDlnaDeviceHandler()
:m_IUpnp(NULL),
m_pRenderingControl(NULL),
m_pConnectionManager(NULL),
m_pContentDirectory(NULL),
m_pAVTransport(NULL)
{

}
CESDlnaDeviceHandler::~CESDlnaDeviceHandler()
{

}
ERROR_TYPE CESDlnaDeviceHandler::HandleDlnaActionRequest(CEventActionRequest & actionReq)
{
	if(strcmp(actionReq.ServiceID,DLNA_CONNECTIONMANAGER_ID) == 0)
	{
		return HandleCMSAction(actionReq);
	}
	else if(strcmp(actionReq.ServiceID,DLNA_RENDERINGCONTROL_ID) == 0)
	{
		return HandleRCSAction(actionReq);
	}
	else if(strcmp(actionReq.ServiceID,DLNA_AVTRANSPORT_ID) == 0)
	{
		return HandleAVTAction(actionReq);
	}
	else if(strcmp(actionReq.ServiceID,DLNA_CONTENTDIRECTORY_ID) == 0)
	{
		return HandleCDSAction(actionReq);
		
	}
	else
		PrintActionError(actionReq,UNKNOWN_SERVICE_ID);

	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleDlnaGetVarRequest(CEventStateVarRequest & eventGetVar)
{
	if(strcmp(eventGetVar.ServiceID,DLNA_CONNECTIONMANAGER_ID) == 0)
	{
		return HandleCMSGetVar(eventGetVar);
	}
	else if(strcmp(eventGetVar.ServiceID,DLNA_RENDERINGCONTROL_ID) == 0)
	{
		return HandleRCSGetVar(eventGetVar);
	}
	else if(strcmp(eventGetVar.ServiceID,DLNA_AVTRANSPORT_ID) == 0)
	{
		return HandleAVTGetVar(eventGetVar);
	}
	else if(strcmp(eventGetVar.ServiceID,DLNA_CONTENTDIRECTORY_ID) == 0)
	{
		return HandleCDSGetVar(eventGetVar);
	}
	else
		PrintGetVarError(eventGetVar,INVALID_VARIABLE);

	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleDlnaSubRequest(CEventSubscriptionRequest & eventSubsReq)
{
	if(strcmp(eventSubsReq.ServiceId ,DLNA_CONNECTIONMANAGER_ID) == 0)
	{
		return HandleCMSSubRequest(eventSubsReq);
	}
	else if(strcmp(eventSubsReq.ServiceId,DLNA_RENDERINGCONTROL_ID) == 0)
	{
		return HandleRCSSubRequest(eventSubsReq);
	}
	else if(strcmp(eventSubsReq.ServiceId,DLNA_AVTRANSPORT_ID) == 0)
	{
		return HandleAVTSubRequest(eventSubsReq);
	}
	else if(strcmp(eventSubsReq.ServiceId,DLNA_CONTENTDIRECTORY_ID) == 0)
	{
		return HandleCDSSubRequest(eventSubsReq);
	}
	else
	{
		//PrintGetVarError(eventGetVar,INVALID_VARIABLE);
	}
	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleCMSAction(CEventActionRequest &actionReq)
{
	if(actionReq.ActionName == NULL)
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);
		return ERROR_SUCCESS;
	}

	int nret = 0;

	if(strcmp(actionReq.ActionName,ACTION_GETPROTOCALINFO) == 0)
	{
		istring strsrcinfo = NULL,strsinkinfo = NULL;
		nret = m_pConnectionManager->GetProtocalInfo(strsrcinfo,strsinkinfo);

		if(nret == 0)
		{
			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_GETPROTOCALINFO,DLNA_CONNECTIONMANAGER,2,
				"Source",strsrcinfo,"Sink",strsinkinfo);

			FREEARRAY(strsrcinfo);
			FREEARRAY(strsinkinfo);
		}


		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_PREPAREFORCONNECTION) == 0)
	{
		icstring remoteprotocalinfo = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"RemoteProtocalInfo");
		CHECK_ERROR(remoteprotocalinfo,actionReq,"RemoteProtocalInfo");

		icstring peerconnectionmanager = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"PeerConnectionManager");
		CHECK_ERROR(peerconnectionmanager,actionReq,"PeerConnectionManager");

		icstring peerconnectionid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"PeerConnectionID");
		CHECK_ERROR(peerconnectionid,actionReq,"PeerConnectionID");

		icstring direction = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"Direction");
		CHECK_ERROR(direction,actionReq,"Direction");

		ui4 connectionid = 0, avtransportid = 0,rcsid = 0;
		nret = m_pConnectionManager->PrepareForConnection(remoteprotocalinfo,peerconnectionmanager,
			peerconnectionid,direction,connectionid,avtransportid,rcsid);

		if(nret == 0)
		{
			char cid[32]={'\0'},cavtid[32]={'\0'},crcsid[32]={'\0'};

			snprintf(cid,32,"%d",connectionid);
			snprintf(cavtid,32,"%d",avtransportid);
			snprintf(crcsid,32,"%d",rcsid);

			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_PREPAREFORCONNECTION,DLNA_CONNECTIONMANAGER,3,
				"ConnectionID",cid,"AVTransportID",cavtid,"RcsID",crcsid);

		}

		actionReq.ErrCode = nret;

	}
	else if(strcmp(actionReq.ActionName,ACTION_CONNECTIONCOMPLETE) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"ConnectionID");
		CHECK_ERROR(cid,actionReq,"ConnectionID");

		ui4 connectionid = atoi(cid);

		nret = m_pConnectionManager->ConnectionComplete(connectionid);
		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_CONNECTIONCOMPLETE,DLNA_CONNECTIONMANAGER,0,NULL,NULL);
		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_GETCONNECTIONIDS) == 0)
	{
		istring connectionids = NULL;
		nret = m_pConnectionManager->GetCurrentConnectionIDs(connectionids);

		if(nret == 0)
		{
			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_PREPAREFORCONNECTION,ACTION_GETCONNECTIONIDS,1,
				"ConnectionIDs",connectionids);

			FREEARRAY(connectionids);
		}



		actionReq.ErrCode = nret;

	}
	else if(strcmp(actionReq.ActionName,ACTION_GETCONNECTIONINFO) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"ConnectionID");
		CHECK_ERROR(cid,actionReq,"ConnectionID");

		ui4 connectionid = atoi(cid);
		ui4 rcsid = -1,avtransportid = -1;
		istring protocolinfo = NULL,peerconnectionmanager = NULL,
			peerconnectionid = NULL,direction = NULL,status = NULL;

			nret = m_pConnectionManager->GetCurrentConnectionInfo(connectionid,
			rcsid,avtransportid,
			protocolinfo,peerconnectionmanager,
			peerconnectionid,direction,
			status);

		if(nret == 0)
		{
			char cavtid[32]={'\0'},crcsid[32]={'\0'};
			snprintf(cavtid,32,"%d",avtransportid);
			snprintf(crcsid,32,"%d",rcsid);

			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_GETCONNECTIONINFO,ACTION_GETCONNECTIONIDS,7,
				"RcsID",crcsid,"AVTransportID",cavtid,
				"ProtocolInfo",protocolinfo,"PeerConnectionManager",peerconnectionmanager,
				"PeerConnectionID",peerconnectionid,"Direction",direction,"Status",status);


			FREEARRAY(protocolinfo);
			FREEARRAY(peerconnectionmanager);
			FREEARRAY(peerconnectionid);
			FREEARRAY(direction);
			FREEARRAY(status);

		}

		actionReq.ErrCode = nret;

	}
	else
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);
	}


	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleRCSAction(CEventActionRequest &actionReq)
{
	if(actionReq.ActionName == NULL)
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);

		return ERROR_SUCCESS;
	}

	int nret = 0;

	if(strcmp(actionReq.ActionName,ACTION_LISTPRESETS) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");

		ui4 id = atoi(cid);
		istring presetlist = NULL;
		if(m_pRenderingControl)
			nret = m_pRenderingControl->ListPresets(id,presetlist);
		else if(m_pRenderingControl)
			nret = m_pRenderingControl->ListPresets(id,presetlist);

		if(nret == 0)
		{
			actionReq.ActionRequest = 
				IUpnpUtil::MakeActionResponse(ACTION_LISTPRESETS,DLNA_RENDERINGCONTROL,1,
				"CurrentPresetNameList",presetlist);

			FREEARRAY(presetlist);
		}

		actionReq.ErrCode = nret;

	}
	else if(strcmp(actionReq.ActionName,ACTION_SELECTPRESETS) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");

		icstring setlist = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"PresetName");

		CHECK_ERROR(setlist,actionReq,"PresetName");

		ui4 id = atoi(cid);
		nret = m_pRenderingControl->SelectPreset(id,setlist);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_SELECTPRESETS,DLNA_RENDERINGCONTROL,0,NULL,NULL);

		actionReq.ErrCode = nret;

	}
	else
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);
	}


	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleAVTAction(CEventActionRequest &actionReq)
{
	if(actionReq.ActionName == NULL)
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);
		return ERROR_SUCCESS;
	}
	int nret = 0;
	if(strcmp(actionReq.ActionName,ACTION_SETAVTRANSURI) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");

		ui4 id = atoi(cid);

		icstring uri = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"CurrentURI");
		CHECK_ERROR(uri,actionReq,"CurrentURI");

		icstring metadata = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"CurrentURIMetaData");
		//CHECK_ERROR(metadata,actionReq,"CurrentURIMetaData");

		nret = m_pAVTransport->SetAVTransportURI(id,uri,metadata);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_SETAVTRANSURI,DLNA_AVTRANSPORT,0,NULL,NULL);

		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_SETNEXTAVTRANSURI) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");

		ui4 id = atoi(cid);

		icstring uri = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"NextURI");
		CHECK_ERROR(uri,actionReq,"NextURI");

		icstring metadata = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"NextURIMetaData");
		//CHECK_ERROR(metadata,actionReq,"NextURIMetaData");

		nret = m_pAVTransport->SetNextAVTransportURI(id,uri,metadata);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_SETNEXTAVTRANSURI,DLNA_AVTRANSPORT,0,NULL,NULL);

		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_GETMEDIAINFO) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");

		ui4 id = atoi(cid);

		ui4 nrtracks = 0;
		istring mediaduration = NULL,currenturi = NULL,currenturimetadata = NULL,
			nexturi = NULL,nexturimetadata = NULL,playmedium = NULL,recordmedium = NULL,
			writestatus = NULL;
		
		nret = m_pAVTransport->GetMediaInfo(id,nrtracks,mediaduration, currenturi,
			currenturimetadata,nexturi,nexturimetadata,
			playmedium,recordmedium,writestatus);

		if(nret == 0)
		{
			char sid[32] = {'\0'};
			snprintf(sid,32,"%d",id);

			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_GETMEDIAINFO,DLNA_AVTRANSPORT,9,
				"NrTracks",sid,"MediaDuration",mediaduration,"CurrentURI",currenturi,
				"CurrentURIMetaData",currenturimetadata,"NextURI",nexturi,
				"NextURIMetaData",nexturimetadata,"PlayMedium",playmedium,
				"RecordMedium",recordmedium,"WriteStatus",writestatus);

			FREEARRAY(mediaduration);
			FREEARRAY(currenturi);
			FREEARRAY(currenturimetadata);
			FREEARRAY(nexturi);
			FREEARRAY(nexturimetadata);
			FREEARRAY(playmedium);
			FREEARRAY(recordmedium);
			FREEARRAY(writestatus);
		}



		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_GETRANSINFO) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");

		ui4 id = atoi(cid);
		istring currenttransportstate = NULL,currenttransportstatus = NULL,currentspeed=NULL;

		nret = m_pAVTransport->GetTransportInfo(id,currenttransportstate,
			currenttransportstatus,currentspeed);

		if(nret == 0)
		{
			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_GETRANSINFO,DLNA_AVTRANSPORT,3,
				"CurrentTransportState",currenttransportstate,
				"CurrentTransportStatus",currenttransportstatus,
				"CurrentSpeed",currentspeed);


			FREEARRAY(currenttransportstate);
			FREEARRAY(currenttransportstatus);
			FREEARRAY(currentspeed);
		}


		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_GETPOSINFO) == 0)
	{

		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		ui4 track = 0;
		istring trackduration = NULL, trackmetadata = NULL,
			trackuri = NULL,reltime = NULL,abstime = NULL;
		i4 relcount = 0,abscount = 0;

			nret = m_pAVTransport->GetPositionInfo(id,track,trackduration,trackmetadata,
			trackuri,reltime,abstime,relcount,abscount);

		if(nret == 0)
		{
			char ctrack[32] = {'\0'},crelcount[32]={'\0'},cabscount[32]={'\0'};
			snprintf(ctrack,32,"%d",track);
			snprintf(crelcount,32,"%d",relcount);
			snprintf(cabscount,32,"%d",abscount);

			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_GETPOSINFO,DLNA_AVTRANSPORT,8,
				"Track",ctrack,"TrackDuration",trackduration,"TrackMetaData",trackmetadata,
				"TrackURI",trackuri,"RelTime",reltime,"AbsTime",abstime,"RelCount",crelcount,
				"AbsCount",cabscount);


			FREEARRAY(trackduration);
			FREEARRAY(trackmetadata);
			FREEARRAY(trackuri);
			FREEARRAY(reltime);
			FREEARRAY(abstime);
		}


		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_GETDEVICECAPS) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		istring playmedia = NULL,recmedia = NULL,recqualitymodes = NULL;

		nret = m_pAVTransport->GetDeviceCapabilities(id,playmedia,recmedia,recqualitymodes);

		if(nret == 0)
		{
			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_GETDEVICECAPS,DLNA_AVTRANSPORT,3,
				"PlayMedia",playmedia,"RecMedia",recmedia,"TrackMetaData","RecQualityModes",recqualitymodes);

			FREEARRAY(playmedia);
			FREEARRAY(recmedia);
			FREEARRAY(recqualitymodes);
		}


		actionReq.ErrCode = nret;

	}
	else if(strcmp(actionReq.ActionName,ACTION_GETTRANSSETTINGS) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		istring playmode = NULL,recqualitymode  = NULL;

		nret = m_pAVTransport->GetTransportSettings(id,playmode,recqualitymode);

		if(nret == 0)
		{
			actionReq.ActionResult = 
				IUpnpUtil::MakeActionResponse(ACTION_GETTRANSSETTINGS,DLNA_AVTRANSPORT,2,
				"PlayMode",playmode,"RecQualityMode",recqualitymode);

			FREEARRAY(playmode);
			FREEARRAY(recqualitymode);
		}


		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_STOP) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		nret = m_pAVTransport->Stop(id);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_STOP,DLNA_AVTRANSPORT,0,NULL,NULL);

		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_PLAY) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		icstring speed = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"Speed");
		CHECK_ERROR(speed,actionReq,"Speed");

		nret = m_pAVTransport->Play(id,speed);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_PLAY,DLNA_AVTRANSPORT,0,NULL,NULL);
		actionReq.ErrCode = nret;

	}
	else if(strcmp(actionReq.ActionName,ACTION_PAUSE) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);
		
		nret = m_pAVTransport->Pause(id);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_PAUSE,DLNA_AVTRANSPORT,0,NULL,NULL);
		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_RECORD) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);


		nret = m_pAVTransport->Record(id);
		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_RECORD,DLNA_AVTRANSPORT,0,NULL,NULL);
		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_SEEK) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		icstring unit = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"Unit");
		CHECK_ERROR(unit,actionReq,"Unit");

		icstring target = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"Target");
		CHECK_ERROR(target,actionReq,"Target");


		nret = m_pAVTransport->Seek(id,unit,target);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_SEEK,DLNA_AVTRANSPORT,0,NULL,NULL);
		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_NEXT) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		nret = m_pAVTransport->Next(id);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_NEXT,DLNA_AVTRANSPORT,0,NULL,NULL);
		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_PREVIOUS) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		nret = m_pAVTransport->Previous(id);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_PREVIOUS,DLNA_AVTRANSPORT,0,NULL,NULL);
		actionReq.ErrCode = nret;
	}	
	else if(strcmp(actionReq.ActionName,ACTION_SETPLAYMODE) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		icstring newplaymode = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"NewPlayMode");
		CHECK_ERROR(newplaymode,actionReq,"NewPlayMode");


		nret = m_pAVTransport->SetPlayMode(id,newplaymode);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_SETPLAYMODE,DLNA_AVTRANSPORT,0,NULL,NULL);

		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_SETRECQAMODE) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		icstring newmode = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"NewRecordQualityMode");
		CHECK_ERROR(newmode,actionReq,"NewRecordQualityMode");

		nret = m_pAVTransport->SetRecordQualityMode(id,newmode);

		actionReq.ActionResult=
			IUpnpUtil::MakeActionResponse(ACTION_SETRECQAMODE,DLNA_AVTRANSPORT,0,NULL,NULL);
		actionReq.ErrCode = nret;

	}
	else if(strcmp(actionReq.ActionName,ACTION_GETTRANSACTIONS) == 0)
	{
		icstring cid = 
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"InstanceID");
		CHECK_ERROR(cid,actionReq,"InstanceID");
		ui4 id = atoi(cid);

		istring actions = NULL;

		nret = m_pAVTransport->GetCurrentTransportActions(id,actions);

		if(nret == 0)
		{
			actionReq.ActionResult = 
				IUpnpUtil::MakeAction(ACTION_SETRECQAMODE,DLNA_AVTRANSPORT,1,
				"Actions",actions);

			FREEARRAY(actions);
		}

		actionReq.ErrCode = nret;
	}
	else
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);
	}


	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleCDSAction(CEventActionRequest &actionReq)
{
	if(actionReq.ActionName == NULL)
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);
		return ERROR_SUCCESS;
	}
	int nret = 0;
	if(strcmp(actionReq.ActionName,ACTION_GETSEARCHCAP) == 0)
	{
		istring out=NULL;
		nret = m_pContentDirectory->GetSearchCapabilities(out);
		if(nret == 0)
		{
			actionReq.ActionResult=
				IUpnpUtil::MakeActionResponse(ACTION_GETSEARCHCAP,DLNA_CONTENTDIRECTORY,
				1,"SearchCaps",out);
			FREEARRAY(out);
		}
		
		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_GETSORTCAP) == 0)
	{
		istring out=NULL;
		nret = m_pContentDirectory->GetSortCapabilities(out);
		if(nret == 0)
		{
			actionReq.ActionResult=
				IUpnpUtil::MakeActionResponse(ACTION_GETSORTCAP,DLNA_CONTENTDIRECTORY,
				1,"SortCaps",out);
			FREEARRAY(out);
		}

		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_GETSYSTEMUPDATEID) == 0)
	{
		ui4 id = 0;
		nret = m_pContentDirectory->GetSystemUpdateID(id);
		if(nret == 0)
		{
			char cid[32] = {'\0'};
			snprintf(cid,32,"%d",id);
			actionReq.ActionResult=
				IUpnpUtil::MakeActionResponse(ACTION_GETSORTCAP,DLNA_CONTENTDIRECTORY,
				1,"Id",cid);
		}
		actionReq.ErrCode = nret;
	}
	else if(strcmp(actionReq.ActionName,ACTION_BROWSE) == 0)
	{
		icstring id =
			IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"ObjectID");
		CHECK_ERROR(id,actionReq,"Id");
		
		icstring browseflag = IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"BrowseFlag");
		CHECK_ERROR(browseflag,actionReq,"BrowseFlag");

		icstring filter = IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"Filter");
			
			
		icstring sort = IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"SortCriteria");
		
		icstring sindex = IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"StartingIndex");
		CHECK_ERROR(sindex,actionReq,"StartingIndex");
		
		ui4 startingindex = atoi(sindex);
		
		icstring sreqcount = IUpnpUtil::GetValueFromXML(actionReq.ActionRequest,"RequestedCount");
		CHECK_ERROR(sreqcount,actionReq,"RequestedCount");

		ui4 requestedcount = atoi(sreqcount);

		istring result = NULL;
		ui4 numberreturned = 0,totalmatches = 0,updateid = 0;

		nret = m_pContentDirectory->Browse(id,browseflag,filter,startingindex,requestedcount,sort,
									result,numberreturned,totalmatches, updateid);
		if(nret == 0)
		{
			char cnum[32] = {'\0'},ctotal[32]={'\0'},cupdate[32]={'\0'};
			snprintf(cnum,32,"%d",numberreturned);
			snprintf(ctotal,32,"%d",totalmatches);
			snprintf(cupdate,32,"%d",updateid);

			actionReq.ActionResult=
				IUpnpUtil::MakeActionResponse(ACTION_BROWSE,DLNA_CONTENTDIRECTORY,
				4,"Result",result,"NumberReturned",cnum,"TotalMatches",ctotal,
				"UpdateID",cupdate);
			
			FREEARRAY(result);
		}
		 actionReq.ErrCode = nret;
	}
	else
	{
		PrintActionError(actionReq,UNKNOWN_SERVICE_ACTION);
	}
	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleAVTSubRequest(CEventSubscriptionRequest & eventSubsReq)
{
	int nret = 0;
	IXML_Document* proset =NULL;
	nret = m_pAVTransport->GetPropSet(proset);
	CComandAcceptSubscriptionEx cmdAceptEx;

	cmdAceptEx.PropSet = proset;

	if(nret == 0)
	{
		cmdAceptEx.DevID = eventSubsReq.UDN;
		cmdAceptEx.ServID = eventSubsReq.ServiceId;
		strncpy(cmdAceptEx.SubsId,eventSubsReq.Sid,44);	

		ERROR_TYPE ret = m_IUpnp->ExecuteCommand(cmdAceptEx);
		if(ret != ERROR_SUCCESS)
		{
			Trace(
				"fail to accept subscription of device[%s]\n",cmdAceptEx.DevID);
		}
	}
	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleRCSSubRequest(CEventSubscriptionRequest & eventSubsReq)
{
	int nret = 0;
	IXML_Document* proset = NULL;
	nret = m_pRenderingControl->GetPropSet(proset);

	CComandAcceptSubscriptionEx cmdAceptEx;

	cmdAceptEx.PropSet = proset;

	if(nret == 0)
	{
		cmdAceptEx.DevID = eventSubsReq.UDN;
		cmdAceptEx.ServID = eventSubsReq.ServiceId;
		strncpy(cmdAceptEx.SubsId,eventSubsReq.Sid,44);

		ERROR_TYPE ret = m_IUpnp->ExecuteCommand(cmdAceptEx);
		if(ret != ERROR_SUCCESS)
		{
			Trace(
				"fail to accept subscription of device[%s]\n",cmdAceptEx.DevID);
		}
	}
	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleCMSSubRequest(CEventSubscriptionRequest & eventSubsReq)
{
	int nret = 0;
	IXML_Document* proset = NULL;
	nret = m_pConnectionManager->GetPropSet(proset);

	CComandAcceptSubscriptionEx cmdAceptEx;

	cmdAceptEx.PropSet = proset;

	if(nret == 0)
	{
		cmdAceptEx.DevID = eventSubsReq.UDN;
		cmdAceptEx.ServID = eventSubsReq.ServiceId;
		strncpy(cmdAceptEx.SubsId,eventSubsReq.Sid,44);
		ERROR_TYPE ret = m_IUpnp->ExecuteCommand(cmdAceptEx);
		if(ret != ERROR_SUCCESS)
		{
			Trace(
				"fail to accept subscription of device[%s]\n",cmdAceptEx.DevID);
		}
	}

	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleCDSSubRequest(CEventSubscriptionRequest & eventSubsReq)
{
	
	int nret = 0;
	IXML_Document* proset = NULL;
	nret = m_pContentDirectory->GetPropSet(proset);

	CComandAcceptSubscriptionEx cmdAceptEx;

	cmdAceptEx.PropSet = proset;

	if(nret == 0)
	{
		cmdAceptEx.DevID = eventSubsReq.UDN;
		cmdAceptEx.ServID = eventSubsReq.ServiceId;
		strncpy(cmdAceptEx.SubsId,eventSubsReq.Sid,44);

		ERROR_TYPE ret = m_IUpnp->ExecuteCommand(cmdAceptEx);
		if(ret != ERROR_SUCCESS)
		{
			Trace(
				"fail to accept subscription of device[%s]\n",cmdAceptEx.DevID);
		}
	}
	return ERROR_SUCCESS;
	
}

ERROR_TYPE CESDlnaDeviceHandler:: HandleCMSGetVar(CEventStateVarRequest &eventGetVar)
{
	eventGetVar.CurrentVal = m_pConnectionManager->GetStatusValue(eventGetVar.StateVarName);

	if(eventGetVar.CurrentVal == NULL)
	{
		PrintGetVarError(eventGetVar,INVALID_VARIABLE);
	}

	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleRCSGetVar(CEventStateVarRequest &eventGetVar)
{
	eventGetVar.CurrentVal = m_pRenderingControl->GetStatusValue(eventGetVar.StateVarName);

	if(eventGetVar.CurrentVal == NULL)
	{
		PrintGetVarError(eventGetVar,INVALID_VARIABLE);
	}
	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleAVTGetVar(CEventStateVarRequest &eventGetVar)
{
	eventGetVar.CurrentVal = m_pAVTransport->GetStatusValue(eventGetVar.StateVarName);

	if(eventGetVar.CurrentVal == NULL)
	{
		PrintGetVarError(eventGetVar,INVALID_VARIABLE);
	}

	return ERROR_SUCCESS;
}
ERROR_TYPE CESDlnaDeviceHandler::HandleCDSGetVar(CEventStateVarRequest &eventGetVar)
{
	eventGetVar.CurrentVal = m_pContentDirectory->GetStatusValue(eventGetVar.StateVarName);

	if(eventGetVar.CurrentVal == NULL)
	{
		PrintGetVarError(eventGetVar,INVALID_VARIABLE);
	}

	return ERROR_SUCCESS;
}






#define LOG_TAG "easydlna-esavtransport"
#include "CESAVTransport.h"
#include "ESDlnaHelper.h"


CESAVTransport::CESAVTransport()
:m_instanceid(0),
m_uri(NULL),
m_urimetadata(NULL),
m_strActive(NULL)
{
}
CESAVTransport::~CESAVTransport()
{
	FREEARRAY(m_uri);
	FREEARRAY(m_urimetadata);
	FREEARRAY(m_strActive);
}
void CESAVTransport::GetLastChange(string result)
{
  result += AVT_EVENT_HEAD;


  result += AVT_EVENT_END;

  return ;
}
int	CESAVTransport::GetPropSet(IXML_Document*& PropSet)
{
	string resultValue;
	GetLastChange(resultValue);
	//#ifdef OS_ANDROID
	//	ERROR_TYPE nret = IUpnpUtil::AddToPropertySet(&PropSet,"LastChange",(const char*)resultValue);
	//#else
		ERROR_TYPE nret = IUpnpUtil::AddToPropertySet(&PropSet,"LastChange",resultValue.c_str());
	//#endif
	if(nret != ERROR_SUCCESS)
		return nret;
	return 0;

}

istring CESAVTransport::GetStatusValue(const char *valName)
{
	if(!HitStateValTable(valName,AVTServiceTable,VAR_TABLE_LENGTH(AVTServiceTable)))
	{
		Trace(
			"no state variable name called [%s] exists for this service\n",valName);

		return NULL;
	}

	istring retVal = NULL;

	if(strcmp(valName,"TransportState") == 0)
	{
		retVal = strdup("STOPPED");
	}
	else if(strcmp(valName,"TransportStatus") == 0)
	{
		retVal = strdup("OK");
	}
	else if(strcmp(valName,"PlaybackStorageMedium") == 0)
	{
		retVal = strdup("UNKNOWN");
	}
	else if(strcmp(valName,"RecordStorageMedium") == 0)
	{
		retVal = strdup("NOT_IMOLEMENTED");
	}
	else if(strcmp(valName,"PossiblePlaybackStorageMedium") == 0)
	{
		retVal = strdup("UNKNOWN");
	}
	else if(strcmp(valName,"PossibleRecordStorageMedium") == 0)
	{
		retVal = strdup("NOT_IMOLEMENTED");
	}
	else if(strcmp(valName,"CurrentPlayMode") == 0)
	{
		retVal = strdup("NORMAL");
	}
	else if(strcmp(valName,"TransportPlaySpeed") == 0)
	{
		retVal = strdup("1");
	}
	else if(strcmp(valName,"RecordMediumWriteStatus") == 0)
	{
		retVal = strdup("NOT_IMOLEMENTED");
	}
	else if(strcmp(valName,"CurrentRecordQualityMode") == 0)
	{
		retVal = strdup("NOT_IMOLEMENTED");
	}
	else if(strcmp(valName,"PossibleRecordQualityModes") == 0)
	{
		retVal = strdup("NOT_IMOLEMENTED");
	}
	else if(strcmp(valName,"NumberOfTracks") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"CurrentTrack") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"CurrentTrackDuration") == 0)
	{
		retVal = strdup("00::00:00");
	}
	else if(strcmp(valName,"CurrentMediumDuration") == 0)
	{
		retVal = strdup("00::00:00");
	}
	else if(strcmp(valName,"CurrentTrackMetaData") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"CurrentTrackURI") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"AVTransportURI") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"AVTransportURIMetaData") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"NextAVTransportURI") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"NextAVTransportURIMetaData") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"RelativeTimePosition") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"RelativeCounterPosition") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"AbsoluteCounterPosition") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"CurrentTransportActions") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"LastChange") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"A_ARG_TYPE_SeekMode") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"A_ARG_TYPE_SeekTarget") == 0)
	{
		retVal = strdup("ABS_TIME");
	}
	else if(strcmp(valName,"A_ARG_TYPE_InstanceID") == 0)
	{
		retVal = strdup("-1");
	}
	return retVal;
}
int CESAVTransport::SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata)
{
	m_instanceid = instanceid;
	SAFEDUP(m_uri,uri);
	SAFEDUP(m_urimetadata,urimetadata);

	return 0;
}
int CESAVTransport::GetMediaInfo(ui4 instanceid,ui4& nrtracks,istring& mediaduration,istring& currenturi,
											  istring& currenturimetadata,istring &nexturi,istring& nexturimetadata,
											  istring& playmedium,istring& recordmedium,istring& writestatus)
{
	if(instanceid != m_instanceid)
		return 718;
	nrtracks = 0;
	mediaduration =strdup("NOT IMPLEMEMTED");
	SAFEDUP(currenturi,m_uri);
	SAFEDUP(currenturimetadata,m_urimetadata);
	nexturi = strdup("");
	nexturimetadata = strdup("");
	playmedium = strdup("NETWORK");
	recordmedium = strdup("NOT IMPLEMEMTED");
	writestatus = strdup("NOT IMPLEMEMTED");

	return 0;
}
int CESAVTransport::GetTransportInfo(ui4 instanceid,istring& currenttransportstate,istring& currenttransportstatus,
												  istring& currentspeed)
{
	if(instanceid != m_instanceid)
		return 718;

	currenttransportstate = strdup("STOPPED");
	currenttransportstatus = strdup("OK");
	currentspeed = strdup("1");

	return 0;
}
int CESAVTransport::GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
												 istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount)
{
	if(instanceid != m_instanceid)
		return 718;

	track = 0;
	trackduration = strdup("12+:32::51");
	SAFEDUP(trackmetadata,m_urimetadata);
	SAFEDUP(trackuri,m_uri);
	reltime = strdup("NOT IMPLEMENTED");
	abstime = strdup("NOT IMPLEMENTED");
	relcount = 0x7fffffff;
	abscount = 0x7fffffff; // maximum of i4 data type means not supported abscount
	return 0;
}
int CESAVTransport::GetDeviceCapabilities(ui4 instanceid,istring& playmedia,istring& recmedia,istring& recqualitymodes)
{
	if(instanceid != m_instanceid)
		return 718;
	playmedia = strdup("NETWORK");
	recmedia = strdup("NOT IMPLEMENTED");
	recqualitymodes = strdup("NOT IMPLEMENTED");
	return 0;
}
int CESAVTransport::GetTransportSettings(ui4 instanceid,istring& playmode,istring& recqualitymode)
{
	if(instanceid != m_instanceid)
		return 718;
	playmode = strdup("NORMAL");
	recqualitymode = strdup("NOT IMPLEMENTED");
	return 0;
}
int CESAVTransport::Stop(ui4 instanceid)
{
	if(instanceid != m_instanceid)
		return 718;
	Trace("DMRender:AVS::Stop play [%d]\n",instanceid);
	return 0;
}

int CESAVTransport::Play(ui4 instanceid,icstring speed)
{
	if(instanceid != m_instanceid)
		return 718;
	Trace("DMRender:AVS::Start play [%d] at speed[%s]\n",instanceid,speed);

	return 0;
}
int CESAVTransport::Seek(ui4 instanceid,icstring unit,icstring target)
{
	if(instanceid != m_instanceid)
		return 718;
	Trace(
		"DMRender:AVS::Seek [%d] at unit [%s] to target[%s]\n",
		instanceid,unit,target);

	return 0;
}
int CESAVTransport::Next(ui4 instanceid)
{
	if(instanceid != m_instanceid)
		return 718;

	Trace("DMRender:AVS::Stop play [%d]\n",instanceid);

	return 0;
}

int CESAVTransport::Previous(ui4 instanceid)
{
	if(instanceid != m_instanceid)
		return 718;
	Trace("DMRender:AVS::Stop play [%d]\n",instanceid);

	return 0;
}


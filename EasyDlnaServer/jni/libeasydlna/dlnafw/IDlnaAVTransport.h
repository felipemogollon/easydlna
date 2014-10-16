#ifndef __IDLNAAVTRANSPORT_H__
#define __IDLNAAVTRANSPORT_H__

#include "IDlnaService.h"


#define DLNA_AVTRANSPORT "urn:schemas-upnp-org:service:AVTransport:1"
#define DLNA_AVTRANSPORT_ID "urn:upnp-org:serviceId:AVTransport"



#define AVT_LASTCHANGE_HEAD\
	"<LastChange>"
#define AVT_LAST_CHANGE_END\
	"</LastChange>"

#define AVT_EVENT_HEAD\
	"<Event xmlns = \"urn:schemas-upnp-org:metadata-1-0/AVT/\">"

#define AVT_EVENT_END\
	"</Event>"


#define AVT_EVENT_ITEM\
	"<%s val=\"%s\"/>"

#define AVT_INSTANCE_HEAD\
	"<InstanceID val=\"%s\">"

#define AVT_INSTANCE_END\
	"</InstanceID>"

struct AVTState
{
	const char* InstanceID;

	const char* TransportState;
	const char* TransportStatus;
	const char* AVTransportURI;

	const char* CurrentTrackDuration;
	const char* CurrentTrackURI;
	const char* CurrentTrack;
	const char* NumberOfTracks;

	AVTState()
	{
		InstanceID = TransportState = TransportStatus = NULL;
		AVTransportURI = CurrentTrackDuration = CurrentTrackURI = NULL;
		CurrentTrack = NumberOfTracks = NULL;
	}
};








static StateVarableEntry AVTServiceTable[] = \
{
	{"TransportState",FALSE},
	{"TransportStatus",FALSE},
	{"PlaybackStorageMedium",FALSE},
	{"RecordStorageMedium",FALSE},
	{"PossiblePlaybackStorageMedium",FALSE},
	{"PossibleRecordStorageMedium",FALSE},
	{"CurrentPlayMode",FALSE},
	{"TransportPlaySpeed",FALSE},
	{"RecordMediumWriteStatus",FALSE},
	{"CurrentRecordQualityMode",FALSE},
	{"PossibleRecordQualityModes",FALSE},
	{"NumberOfTracks",FALSE},
	{"CurrentTrack",FALSE},
	{"CurrentTrackDuration",FALSE},
	{"CurrentMediumDuration",FALSE},
	{"CurrentTrackMetaData",FALSE},
	{"CurrentTrackURI",FALSE},
	{"AVTransportURI",FALSE},
	{"AVTransportURIMetaData",FALSE},
	{"NextAVTransportURI",FALSE},
	{"NextAVTransportURIMetaData",FALSE},
	{"RelativeTimePosition",FALSE},
	{"RelativeCounterPosition",FALSE},
	{"AbsoluteCounterPosition",FALSE},
	{"CurrentTransportActions",FALSE},
	{"LastChange",TRUE},
	{"A_ARG_TYPE_SeekMode",FALSE},
	{"A_ARG_TYPE_SeekTarget",FALSE},
	{"A_ARG_TYPE_InstanceID",FALSE}
};



class IDlnaAVTransport: public IDlnaService
{
public:
#define ACTION_SETAVTRANSURI		"SetAVTransportURI"
#define ACTION_SETNEXTAVTRANSURI	"SetNextAVTransportURI"
#define ACTION_GETMEDIAINFO			"GetMediaInfo"
#define ACTION_GETRANSINFO			"GetTransportInfo"
#define ACTION_GETPOSINFO			"GetPositionInfo"
#define ACTION_GETDEVICECAPS		"GetDeviceCapabilities"
#define ACTION_GETTRANSSETTINGS		"GetTransportSettings"
#define ACTION_STOP					"Stop"
#define ACTION_PLAY					"Play"
#define ACTION_PAUSE				"Pause"
#define ACTION_RECORD				"Record"
#define ACTION_SEEK					"Seek"
#define ACTION_NEXT					"Next"
#define ACTION_PREVIOUS				"Previous"
#define ACTION_SETPLAYMODE			"SetPlayMode"
#define ACTION_SETRECQAMODE			"SetRecordQualityMode"
#define ACTION_GETTRANSACTIONS		"GetCurrentTransportActions"

public:
	IDlnaAVTransport()
	{
		m_dlnaServiceType = DLNA_ST_AVTRANSPORT;
	}
	virtual ~IDlnaAVTransport(){};
	virtual const char* GetErrorMessage(int errorCode)
	{
		switch(errorCode)
		{
		case 701:
			return "Transition not available";
			break;
		case 702:
			return "No contents";
		case 703:
			return "Read error";
		case 704:
			return "Format not supported for playback";
		case 705:
			return "Transport is locked";
			break;
		case 706:
			return "Write error";
		case 707:
			return "Media is protected or not writable";
		case 708:
			return "Format not supported for recording";
		case 709:
			return "Media is full";
		case 710:
			return "Seek mode not supported";
		case 711:
			return "Illegal seek target";
		case 712:
			return "Play mode not supported";
		case 713:
			return "Record quality not supported";
		case 714:
			return "Illegal MIME-type";
			break;
		case 715:
			return "Content 'BUSY'";
			break;
		case 716:
			return "Resource not found";
			break;
		case 717:
			return "Play speed not supported";
		case 718:
			return "Invalid InstanceID";
			break;
		default:
			return IDlnaService::GetErrorMessage(errorCode);
			break;
		}
	}

public:
	virtual  int SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata) = 0;

	virtual  int SetNextAVTransportURI(ui4 instanceid,icstring nexturi,icstring nexturimetadata)
										{return ERROR_INVALID_ACTION;};

	virtual  int GetMediaInfo(ui4 instanceid,ui4& nrtracks,istring& mediaduration,istring& currenturi,
							istring& currenturimetadata,istring &nexturi,istring& nexturimetadata,
							istring& playmedium,istring& recordmedium,istring& writestatus)	= 0;

	virtual  int GetTransportInfo(ui4 instanceid,istring& currenttransportstate,istring& currenttransportstatus,
									istring& currentspeed) = 0;

	virtual  int GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
								istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount ) = 0;

	virtual  int GetDeviceCapabilities(ui4 instanceid,istring& playmedia,istring& recmedia,istring& recqualitymodes) = 0;

	virtual  int GetTransportSettings(ui4 instanceid,istring& playmode,istring& recqualitymode) = 0;

	virtual  int Stop(ui4 instanceid)	= 0;

	virtual  int Play(ui4 instanceid,icstring speed)	= 0;

	virtual  int Pause(ui4 instanceid){return ERROR_INVALID_ACTION;};
	virtual  int Record(ui4 instanceid){return ERROR_INVALID_ACTION;};

	virtual  int Seek(ui4 instanceid,icstring unit,icstring target)  = 0;

	virtual  int Next(ui4 instanceid) = 0;

	virtual  int Previous(ui4 instanceid) = 0;

	virtual  int SetPlayMode(ui4 instanceid,icstring newplaymode){return ERROR_INVALID_ACTION;};

	virtual  int SetRecordQualityMode(ui4 instanceid,icstring newrecordqualitymode){return ERROR_INVALID_ACTION;};

	virtual  int GetCurrentTransportActions(ui4 instanceid,istring& actions){return ERROR_INVALID_ACTION;};
};


#define AVT_DESCRIPTION \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"  \
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">"\
"<specVersion>"\
"<major>1</major>"\
"<minor>0</minor>"\
"</specVersion>"\
"<actionList>"\
"<action>" \
"<name>GetCurrentTransportActions</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Actions</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentTransportActions</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>GetDeviceCapabilities</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>PlayMedia</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>PossiblePlaybackStorageMedia</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RecMedia</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>PossibleRecordStorageMedia</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RecQualityModes</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>PossibleRecordQualityModes</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>GetMediaInfo</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NrTracks</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>NumberOfTracks</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>MediaDuration</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentMediaDuration</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentURI</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>AVTransportURI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentURIMetaData</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NextURI</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>NextAVTransportURI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NextURIMetaData</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>NextAVTransportURIMetaData</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>PlayMedium</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>PlaybackStorageMedium</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RecordMedium</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>RecordStorageMedium</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>WriteStatus</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>RecordMediumWriteStatus</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>GetPositionInfo</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Track</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentTrack</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TrackDuration</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentTrackDuration</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TrackMetaData</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentTrackMetaData</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>TrackURI</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentTrackURI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RelTime</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>RelativeTimePosition</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>AbsTime</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>AbsoluteTimePosition</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RelCount</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>RelativeCounterPosition</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>AbsCount</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>AbsoluteCounterPosition</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>GetTransportInfo</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentTransportState</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>TransportState</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentTransportStatus</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>TransportStatus</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentSpeed</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>TransportPlaySpeed</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>GetTransportSettings</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>PlayMode</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentPlayMode</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>RecQualityMode</name>"  \
"<direction>out</direction>" \
"<relatedStateVariable>CurrentRecordQualityMode</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>Next</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>Pause</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>Play</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Speed</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>TransportPlaySpeed</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>Previous</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>Seek</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Unit</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_SeekMode</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>Target</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_SeekTarget</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>SetAVTransportURI</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentURI</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>AVTransportURI</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>CurrentURIMetaData</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>SetPlayMode</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"<argument>" \
"<name>NewPlayMode</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>CurrentPlayMode</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"<action>" \
"<name>Stop</name>"  \
"<argumentList>" \
"<argument>" \
"<name>InstanceID</name>"  \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>"  \
"</argument>" \
"</argumentList>" \
"</action>" \
"</actionList>" \
"<serviceStateTable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>TransportStatus</name>"  \
"<dataType>string</dataType>" \
"<allowedValueList>" \
"<allowedValue>OK</allowedValue>" \
"<allowedValue>ERROR_OCCURRED</allowedValue>" \
"<allowedValue> vendor-defined </allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>NextAVTransportURI</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>NextAVTransportURIMetaData</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentTrackMetaData</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>RelativeCounterPosition</name>"  \
"<dataType>i4</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>A_ARG_TYPE_InstanceID</name>"  \
"<dataType>ui4</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>A_ARG_TYPE_SeekTarget</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>PlaybackStorageMedium</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>UNKNOWN</allowedValue>" \
"<allowedValue>DV</allowedValue>" \
"<allowedValue>MINI-DV</allowedValue>" \
"<allowedValue>VHS</allowedValue>" \
"<allowedValue>W-VHS</allowedValue>" \
"<allowedValue>S-VHS</allowedValue>" \
"<allowedValue>D-VHS</allowedValue>" \
"<allowedValue>VHSC</allowedValue>" \
"<allowedValue>VIDEO8</allowedValue>" \
"<allowedValue>HI8</allowedValue>" \
"<allowedValue>CD-ROM</allowedValue>" \
"<allowedValue>CD-DA</allowedValue>" \
"<allowedValue>CD-R</allowedValue>" \
"<allowedValue>CD-RW</allowedValue>" \
"<allowedValue>VIDEO-CD</allowedValue>" \
"<allowedValue>SACD</allowedValue>" \
"<allowedValue>MD-AUDIO</allowedValue>" \
"<allowedValue>MD-PICTURE</allowedValue>" \
"<allowedValue>DVD-ROM</allowedValue>" \
"<allowedValue>DVD-VIDEO</allowedValue>" \
"<allowedValue>DVD-R</allowedValue>" \
"<allowedValue>DVD+RW</allowedValue>" \
"<allowedValue>DVD-RW</allowedValue>" \
"<allowedValue>DVD-RAM</allowedValue>" \
"<allowedValue>DVD-AUDIO</allowedValue>" \
"<allowedValue>DAT</allowedValue>" \
"<allowedValue>LD</allowedValue>" \
"<allowedValue>HDD</allowedValue>" \
"<allowedValue>MICRO-MV</allowedValue>" \
"<allowedValue>NETWORK</allowedValue>" \
"<allowedValue>NONE</allowedValue>" \
"<allowedValue>NOT_IMPLEMENTED</allowedValue>" \
"<allowedValue> vendor-defined </allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>RelativeTimePosition</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>PossibleRecordStorageMedia</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentPlayMode</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>NORMAL</allowedValue>" \
"<allowedValue>REPEAT_ALL</allowedValue>" \
"<allowedValue>INTRO</allowedValue>" \
"</allowedValueList>"\
"<defaultValue>NORMAL</defaultValue>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>TransportPlaySpeed</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>1</allowedValue>" \
"<allowedValue> vendor-defined </allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>PossiblePlaybackStorageMedia</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>AbsoluteTimePosition</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentTrack</name>"  \
"<dataType>ui4</dataType>"  \
"<allowedValueRange>"\
"<minimum>0</minimum>"\
"<maximum>4294967295</maximum>" \
"<step>1</step>"\
"</allowedValueRange>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentTrackURI</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentTransportActions</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>NumberOfTracks</name>"  \
"<dataType>ui4</dataType>"  \
"<allowedValueRange>"\
"<minimum>0</minimum>"\
"<maximum>4294967295</maximum>"\
"</allowedValueRange>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>AVTransportURI</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>AbsoluteCounterPosition</name>"  \
"<dataType>i4</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentRecordQualityMode</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>0:EP</allowedValue>" \
"<allowedValue>1:LP</allowedValue>" \
"<allowedValue>2:SP</allowedValue>" \
"<allowedValue>0:BASIC</allowedValue>" \
"<allowedValue>1:MEDIUM</allowedValue>" \
"<allowedValue>2:HIGH</allowedValue>" \
"<allowedValue>NOT_IMPLEMENTED</allowedValue>" \
"<allowedValue> vendor-defined </allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentMediaDuration</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>A_ARG_TYPE_SeekMode</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>ABS_TIME</allowedValue>" \
"<allowedValue>REL_TIME</allowedValue>" \
"<allowedValue>ABS_COUNT</allowedValue>" \
"<allowedValue>REL_COUNT</allowedValue>" \
"<allowedValue>TRACK_NR</allowedValue>" \
"<allowedValue>CHANNEL_FREQ</allowedValue>" \
"<allowedValue>TAPE-INDEX</allowedValue>" \
"<allowedValue>FRAME</allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>AVTransportURIMetaData</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>RecordStorageMedium</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>UNKNOWN</allowedValue>" \
"<allowedValue>DV</allowedValue>" \
"<allowedValue>MINI-DV</allowedValue>" \
"<allowedValue>VHS</allowedValue>" \
"<allowedValue>W-VHS</allowedValue>" \
"<allowedValue>S-VHS</allowedValue>" \
"<allowedValue>D-VHS</allowedValue>" \
"<allowedValue>VHSC</allowedValue>" \
"<allowedValue>VIDEO8</allowedValue>" \
"<allowedValue>HI8</allowedValue>" \
"<allowedValue>CD-ROM</allowedValue>" \
"<allowedValue>CD-DA</allowedValue>" \
"<allowedValue>CD-R</allowedValue>" \
"<allowedValue>CD-RW</allowedValue>" \
"<allowedValue>VIDEO-CD</allowedValue>" \
"<allowedValue>SACD</allowedValue>" \
"<allowedValue>MD-AUDIO</allowedValue>" \
"<allowedValue>MD-PICTURE</allowedValue>" \
"<allowedValue>DVD-ROM</allowedValue>" \
"<allowedValue>DVD-VIDEO</allowedValue>" \
"<allowedValue>DVD-R</allowedValue>" \
"<allowedValue>DVD+RW</allowedValue>" \
"<allowedValue>DVD-RW</allowedValue>" \
"<allowedValue>DVD-RAM</allowedValue>" \
"<allowedValue>DVD-AUDIO</allowedValue>" \
"<allowedValue>DAT</allowedValue>" \
"<allowedValue>LD</allowedValue>" \
"<allowedValue>HDD</allowedValue>" \
"<allowedValue>MICRO-MV</allowedValue>" \
"<allowedValue>NETWORK</allowedValue>" \
"<allowedValue>NONE</allowedValue>" \
"<allowedValue>NOT_IMPLEMENTED</allowedValue>" \
"<allowedValue> vendor-defined </allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>RecordMediumWriteStatus</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>WRITABLE</allowedValue>" \
"<allowedValue>PROTECTED</allowedValue>" \
"<allowedValue>NOT_WRITABLE</allowedValue>" \
"<allowedValue>UNKNOWN</allowedValue>" \
"<allowedValue>NOT_IMPLEMENTED</allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"yes\">" \
"<name>LastChange</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>CurrentTrackDuration</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>TransportState</name>"  \
"<dataType>string</dataType>"  \
"<allowedValueList>" \
"<allowedValue>STOPPED</allowedValue>" \
"<allowedValue>PAUSED_PLAYBACK</allowedValue>" \
"<allowedValue>PAUSED_RECORDING</allowedValue>" \
"<allowedValue>PLAYING</allowedValue>" \
"<allowedValue>RECORDING</allowedValue>" \
"<allowedValue>TRANSITIONING</allowedValue>" \
"<allowedValue>NO_MEDIA_PRESENT</allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>PossibleRecordQualityModes</name>"  \
"<dataType>string</dataType>"  \
"</stateVariable>" \
"</serviceStateTable>" \
"</scpd>"

#define AVT_DESCRIPTION_LEN  strlen (AVT_DESCRIPTION)






















#endif


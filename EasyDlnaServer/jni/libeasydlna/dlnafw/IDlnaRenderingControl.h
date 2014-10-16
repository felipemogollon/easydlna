#ifndef __IDLNARENDERINGCONTROL_H__
#define __IDLNARENDERINGCONTROL_H__

#include "IDlnaService.h"

#define DLNA_RENDERINGCONTROL "urn:schemas-upnp-org:service:RenderingControl:1"
#define DLNA_RENDERINGCONTROL_ID "urn:upnp-org:serviceId:RenderingControl"

static StateVarableEntry RCServiceTable[] = \
{
	{"PresetNameList",FALSE},
	{"LastChange",TRUE},
	{"Brightness",FALSE},
	{"Contrast",FALSE},
	{"Sharpness",FALSE},
	{"RedVideoGain",FALSE},
	{"GreenVideoGain",FALSE},
	{"BlueVideoGain",FALSE},
	{"RedVideoBlackLevel",FALSE},
	{"GreenVideoBlackLevel",FALSE},
	{"BlueVideoBlackLevel",FALSE},
	{"ColorTemperature",FALSE},
	{"HorizontalKeyStone",FALSE},
	{"VerticalKeyStone",FALSE},
	{"Mute",FALSE},
	{"Volume",FALSE},
	{"VolumeDB",FALSE},
	{"Loudness",FALSE},
	{"A_ARG_TYPE_Channel",FALSE},
	{"A_ARG_TYPE_InstanceID",FALSE},
	{"A_ARG_TYPE_PresetName",FALSE}
};

class IDlnaRenderingControl:public IDlnaService
{
public:
#define ACTION_LISTPRESETS "ListPresets"
#define ACTION_SELECTPRESETS "SelectPreset"


public:
	IDlnaRenderingControl(){m_dlnaServiceType = DLNA_ST_RENDERINGCONTROL;};
	virtual ~IDlnaRenderingControl(){};
	
public:
	virtual const char* GetErrorMessage(int errorCode)
	{
		switch(errorCode)
		{
		case 701:
			return "Invalid Name";
		case 702:
			return "Invalid InstanceID";
		default:
			return IDlnaService::GetErrorMessage(errorCode);
		    break;
		}
	}
	
	virtual int ListPresets(ui4 instanceid,istring& currentpresetnamelist) = 0;
	virtual int SelectPreset(ui4 instanceid,icstring presetname) = 0;

	virtual int GetBrightness(ui4 instanceid,ui2&currentbrightness){return ERROR_INVALID_ACTION;};
	virtual int SetBrightness(ui4 instanceid,ui2 desiredbrightness){return ERROR_INVALID_ACTION;};
	
	virtual int GetContrast(ui4 instanceid,ui2& currentcontrast){return ERROR_INVALID_ACTION;};
	virtual int SetContrast(ui4 instanceid,ui2 desiredcontrast){return ERROR_INVALID_ACTION;};
	
	virtual int GetSharpness(ui4 instanceid,ui2& currentsharpness){return ERROR_INVALID_ACTION;};
	virtual int SetSharpness(ui4 instanceid,ui2 desiredsharpness){return ERROR_INVALID_ACTION;};
	
	virtual int GetRedVideoGain(ui4 instanceid,ui2& currentredvideogain){return ERROR_INVALID_ACTION;};
	virtual int SetRedVideoGain(ui4 instanceid,ui2 desiredredvideogain){return ERROR_INVALID_ACTION;};
	
	virtual int GetGreenVideoGain(ui4 instanceid,ui2& currentgreenvideogain){return ERROR_INVALID_ACTION;};
	virtual int SetGreenVideoGain(ui4 instanceid,ui2 desiredgreenvideogain){return ERROR_INVALID_ACTION;};
	
	virtual int GetBlueVideoGain(ui4 instanceid,ui2& currentbluevideogain){return ERROR_INVALID_ACTION;};
	virtual int SetBlueVideoGain(ui4 instanceid,ui2 desiredbluevideogain){return ERROR_INVALID_ACTION;};

	virtual int GetRedVideoBlackLevel(ui4 instanceid,ui2&currentredvideoblacklevel){return ERROR_INVALID_ACTION;};
	virtual int SetRedVideoBlackLevel(ui4 instanceid,ui2 desiredredvideoblacklevel){return ERROR_INVALID_ACTION;};
	
	virtual int GetGreenVideoBlackLevel(ui4 instanceid,ui2&currentgreenvideoblacklevel){return ERROR_INVALID_ACTION;};
	virtual int SetGreenVideoBlackLevel(ui4 instanceid,ui2 desiredgreenvideoblacklevel){return ERROR_INVALID_ACTION;};
	
	virtual int GetBlueVideoBlackLevel(ui4 instanceid,ui2&currentbluevideoblacklevel){return ERROR_INVALID_ACTION;};
	virtual int SetBlueVideoBlackLevel(ui4 instanceid,ui2 desiredbluevideoblacklevel){return ERROR_INVALID_ACTION;};

	virtual int GetColorTemperature(ui4 instanceid,ui2& currentcolortemperature){return ERROR_INVALID_ACTION;};
	virtual int SetColorTemperature(ui4 instanceid,ui2 desiredcolortemperature){return ERROR_INVALID_ACTION;};

	virtual int GetHorizontalKeyStone(ui4 instanceid,i2& currenthorizontalkeystone){return ERROR_INVALID_ACTION;};
	virtual int SetHorizontalKeyStone(ui4 instanceid,i2 desiredhorizontalkeystone){return ERROR_INVALID_ACTION;};

	virtual int GetVerticalKeyStone(ui4 instanceid,i2& currentverticalkeystone){return ERROR_INVALID_ACTION;};
	virtual int GetVerticalKeyStone(ui4 instanceid,i2 desiredverticalkeystone){return ERROR_INVALID_ACTION;};

	virtual int GetMute(ui4 instanceid,icstring channel,istring& currentmute){return ERROR_INVALID_ACTION;};
	virtual int SetMute(ui4 instanceid,icstring channel,icstring desiredmute){return ERROR_INVALID_ACTION;};

	virtual int GetVolume(ui4 instanceid,icstring channel,ui2&currentvolume){return ERROR_INVALID_ACTION;};
	virtual int SetVolume(ui4 instanceid,icstring channel,ui2 desiredvolume){return ERROR_INVALID_ACTION;};

	virtual int GetVolumneDB(ui4 instanceid,icstring channel,i2& currentvolume){return ERROR_INVALID_ACTION;};
	virtual int SetVolumneDB(ui4 instanceid,icstring channel,i2 desiredvolume){return ERROR_INVALID_ACTION;};
	virtual int GetVolumneDBRange(ui4 instanceid,icstring channel,
							i2& minvalue,i2&maxvlaue){return ERROR_INVALID_ACTION;};

	virtual int GetLoudness(ui4 instanceid,icstring channel,istring &currentloudness){return ERROR_INVALID_ACTION;};
	virtual int SetLoudness(ui4 instanceid,icstring channel,icstring desiredloudness){return ERROR_INVALID_ACTION;};


};
#define RCS_DESCRIPTION \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"  \
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">"  \
"<specVersion>" \
"<major>1</major>" \
"<minor>0</minor>" \
"</specVersion>" \
"<actionList>" \
"<action>"  \
"<name>GetBlueVideoBlackLevel</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentBlueVideoBlackLevel</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>BlueVideoBlackLevel</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetBlueVideoGain</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentBlueVideoGain</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>BlueVideoGain</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetBrightness</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentBrightness</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>Brightness</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetColorTemperature</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentColorTemperature</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>ColorTemperature</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetContrast</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentContrast</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>Contrast</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetGreenVideoBlackLevel</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentGreenVideoBlackLevel</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>GreenVideoBlackLevel</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetGreenVideoGain</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentGreenVideoGain</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>GreenVideoGain</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetHorizontalKeystone</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentHorizontalKeystone</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>HorizontalKeystone</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetLoudness</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentLoudness</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>Loudness</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetMute</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentMute</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>Mute</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetRedVideoBlackLevel</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentRedVideoBlackLevel</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>RedVideoBlackLevel</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetRedVideoGain</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentRedVideoGain</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>RedVideoGain</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetSharpness</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentSharpness</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>Sharpness</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetVerticalKeystone</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentVerticalKeystone</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>VerticalKeystone</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetVolume</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentVolume</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>Volume</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetVolumeDB</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentVolume</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>VolumeDB</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>GetVolumeDBRange</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>MinValue</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>VolumeDB</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>MaxValue</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>VolumeDB</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>ListPresets</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>CurrentPresetNameList</name>" \
"<direction>out</direction>" \
"<relatedStateVariable>PresetNameList</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SelectPreset</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>PresetName</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_PresetName</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetBlueVideoBlackLevel</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredBlueVideoBlackLevel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>BlueVideoBlackLevel</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetBlueVideoGain</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredBlueVideoGain</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>BlueVideoGain</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetBrightness</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredBrightness</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>Brightness</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetColorTemperature</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredColorTemperature</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>ColorTemperature</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetContrast</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredContrast</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>Contrast</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetGreenVideoBlackLevel</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredGreenVideoBlackLevel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>GreenVideoBlackLevel</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetGreenVideoGain</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredGreenVideoGain</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>GreenVideoGain</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetHorizontalKeystone</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredHorizontalKeystone</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>HorizontalKeystone</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetLoudness</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredLoudness</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>Loudness</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetMute</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredMute</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>Mute</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetRedVideoBlackLevel</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredRedVideoBlackLevel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>RedVideoBlackLevel</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetRedVideoGain</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredRedVideoGain</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>RedVideoGain</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetSharpness</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredSharpness</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>Sharpness</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetVerticalKeystone</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredVerticalKeystone</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>VerticalKeystone</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetVolume</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredVolume</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>Volume</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"<action>"  \
"<name>SetVolumeDB</name>" \
"<argumentList>"  \
"<argument>"  \
"<name>InstanceID</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>Channel</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>" \
"</argument>"  \
"<argument>"  \
"<name>DesiredVolume</name>" \
"<direction>in</direction>" \
"<relatedStateVariable>VolumeDB</relatedStateVariable>" \
"</argument>"  \
"</argumentList>"  \
"</action>"  \
"</actionList>" \
"<serviceStateTable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>GreenVideoGain</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>BlueVideoBlackLevel</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>VerticalKeystone</name>" \
"<dataType>i2</dataType>" \
"<allowedValueRange>" \
"<minimum>-32768</minimum>" \
"<maximum>32767</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>GreenVideoBlackLevel</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>Volume</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>Loudness</name>" \
"<dataType>boolean</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>A_ARG_TYPE_InstanceID</name>" \
"<dataType>ui4</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>RedVideoGain</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>ColorTemperature</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>65535</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>Sharpness</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>A_ARG_TYPE_PresetName</name>" \
"<dataType>string</dataType>" \
"<allowedValueList>" \
"<allowedValue>FactoryDefaults</allowedValue>" \
"<allowedValue>InstallationDefaults</allowedValue>" \
"<allowedValue>Vendor defined</allowedValue>" \
"</allowedValueList>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>RedVideoBlackLevel</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>BlueVideoGain</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>Mute</name>" \
"<dataType>boolean</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"yes\">" \
"<name>LastChange</name>" \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>A_ARG_TYPE_Channel</name>" \
"<dataType>string</dataType>" \
"<allowedValueList>" \
"<allowedValue>Master</allowedValue>" \
"<allowedValue>LF</allowedValue>" \
"<allowedValue>RF</allowedValue>" \
"</allowedValueList>"\
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>HorizontalKeystone</name>" \
"<dataType>i2</dataType>" \
"<allowedValueRange>" \
"<minimum>-32768</minimum>" \
"<maximum>32767</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>VolumeDB</name>" \
"<dataType>i2</dataType>" \
"<allowedValueRange>" \
"<minimum>-32768</minimum>" \
"<maximum>32767</maximum>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>PresetNameList</name>" \
"<dataType>string</dataType>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>Contrast</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"<stateVariable sendEvents=\"no\">" \
"<name>Brightness</name>" \
"<dataType>ui2</dataType>" \
"<allowedValueRange>" \
"<minimum>0</minimum>" \
"<maximum>100</maximum>" \
"<step>1</step>" \
"</allowedValueRange>" \
"</stateVariable>" \
"</serviceStateTable>" \
"</scpd>"

#define RCS_DESCRIPTION_LEN strlen (RCS_DESCRIPTION) 




















#endif


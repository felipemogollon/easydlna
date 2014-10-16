#include "CESRenderingControl.h"
#include "ESDlnaHelper.h"


CESRenderingControl::CESRenderingControl()
:m_strActive(NULL)
{

}
CESRenderingControl::~CESRenderingControl()
{
	FREEARRAY(m_strActive);
}
int	CESRenderingControl::GetPropSet(IXML_Document*& PropSet)
{

	ERROR_TYPE nret = IUpnpUtil::AddToPropertySet(&PropSet,"Brightness","0");
	if(nret != ERROR_SUCCESS)
		return nret;

	nret = IUpnpUtil::AddToPropertySet(&PropSet,"Contrast","0");
	if(nret != ERROR_SUCCESS)
		return nret;

	nret = IUpnpUtil::AddToPropertySet(&PropSet,"Sharpness","0");
	if(nret != ERROR_SUCCESS)
		return nret;

	return 0;

}
istring CESRenderingControl::GetStatusValue(const char *valName)
{
	if(!HitStateValTable(valName,RCServiceTable,VAR_TABLE_LENGTH(RCServiceTable)))
	{
		Trace(
			"no state variable name called [%s] exists for this service\n",valName);

		return NULL;
	}

	istring retVal = NULL;

	if(strcmp(valName,"PresetNameList") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"LastChange") == 0)
	{
		retVal = strdup("");
	}
	else if(strcmp(valName,"Brightness") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"Contrast") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"Sharpness") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"RedVideoGain") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"GreenVideoGain") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"BlueVideoGain") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"RedVideoBlackLevel") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"GreenVideoBlackLevel") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"BlueVideoBlackLevel") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"ColorTemperature") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"HorizontalKeyStone") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"VerticalKeyStone") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"Mute") == 0)
	{
		retVal = strdup("true");
	}
	else if(strcmp(valName,"Volume") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"VolumeDB") == 0)
	{
		retVal = strdup("0");
	}
	else if(strcmp(valName,"Loudness") == 0)
	{
		retVal = strdup("true");
	}
	else if(strcmp(valName,"A_ARG_TYPE_Channel") == 0)
	{
		retVal = strdup("Master");
	}
	else if(strcmp(valName,"A_ARG_TYPE_InstanceID") == 0)
	{
		retVal = strdup("-1");
	}
	else if(strcmp(valName,"A_ARG_TYPE_PresetName") == 0)
	{
		retVal = strdup("FactoryDefaults");
	}

	return retVal;
}
int CESRenderingControl::ListPresets(ui4 instanceid,istring& currentpresetnamelist)
{
	currentpresetnamelist = strdup("");
	return 0;
}
int CESRenderingControl::SelectPreset(ui4 instanceid,icstring presetname)
{

	return 0;
}

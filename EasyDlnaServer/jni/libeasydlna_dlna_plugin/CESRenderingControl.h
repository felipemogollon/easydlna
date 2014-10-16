#ifndef __CESRENDERINGCONTROL_H_
#define  __CESRENDERINGCONTROL_H_

#include "IDlnaRenderingControl.h"

class CESRenderingControl: public IDlnaRenderingControl
{
public:
	CESRenderingControl();
	virtual ~CESRenderingControl();

public:
	virtual int SetActive(const char* udn)
	{
		SAFEDUP(m_strActive,udn);
		return 0;			
	}
	virtual int GetPropSet(IXML_Document*& PropSet);
	virtual  istring GetStatusValue(const char* ValName);
	virtual int ListPresets(ui4 instanceid,istring& currentpresetnamelist);
	virtual int SelectPreset(ui4 instanceid,icstring presetname);


private:
	istring m_strActive;
};


















#endif


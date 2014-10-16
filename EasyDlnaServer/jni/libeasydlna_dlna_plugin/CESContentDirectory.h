#ifndef __CESCONTENTDIRECTORY_H__
#define __CESCONTENTDIRECTORY_H__

#include "IDlnaContentDirectory.h"

class CESContentDirectory: public IDlnaContentDirectory
{
public:
	CESContentDirectory();
	virtual ~CESContentDirectory();

public:
	virtual int SetActive(const char* udn)
	{
		SAFEDUP(m_strActive,udn);
		return 0;			
	}
	virtual int GetPropSet(IXML_Document*& PropSet){return ERROR_INVALID_ACTION;};
	virtual  istring GetStatusValue(const char* ValName){return NULL;};

	virtual int GetSearchCapabilities(istring & searchcaps);

	virtual int GetSortCapabilities(istring & sortcaps);

	virtual  int GetSystemUpdateID(ui4 & id);
	virtual int Browse(icstring id,icstring browseflag,icstring filter,
		ui4 startingindex,ui4 requestedcount,icstring sort,
		istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid);

private:
	istring m_strActive;
};


#endif


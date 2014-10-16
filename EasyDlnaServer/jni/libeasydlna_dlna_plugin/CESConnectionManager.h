#ifndef __CESCONNECTIONMANAGER_H__
#define __CESCONNECTIONMANAGER_H__

#include "IDlnaConnectionManager.h"

class CESConnetionManager: public IDlnaConnetionManager
{
public:
	CESConnetionManager();
	virtual ~CESConnetionManager();

	virtual int SetActive(const char* udn)
	{
		SAFEDUP(m_strActive,udn);
		return 0;
	}
	virtual  istring GetStatusValue(const char* ValName);
	virtual int GetProtocalInfo(istring & source,istring &sink);

	virtual int GetCurrentConnectionIDs(istring & connectionids);
	virtual int GetPropSet(IXML_Document*& PropSet);
	virtual int GetCurrentConnectionInfo(ui4 connectionid,ui4& rcsid,ui4&avtransportid,
		istring& protocolinfo,istring& peerconnectionmanager,
		istring& peerconnectionid,istring&direction,
		istring&status);

private:
	string  m_protocolinfo;
	istring m_strActive;
};











#endif

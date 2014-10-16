#ifndef __CESAVTRANSPORT_H__
#define __CESAVTRANSPORT_H__

#include "IDlnaAVTransport.h"

class CESAVTransport: public IDlnaAVTransport
{
public:
	CESAVTransport();
	virtual ~CESAVTransport();

	virtual int SetActive(const char* udn)
	{
		SAFEDUP(m_strActive,udn);
		return 0;			
	}
	virtual int GetPropSet(IXML_Document*& PropSet);
	virtual  istring GetStatusValue(const char* ValName);
	virtual  int SetAVTransportURI(ui4 instanceid,icstring uri,icstring urimetadata);
	virtual  int GetMediaInfo(ui4 instanceid,ui4& nrtracks,istring& mediaduration,istring& currenturi,
		istring& currenturimetadata,istring &nexturi,istring& nexturimetadata,
		istring& playmedium,istring& recordmedium,istring& writestatus);
	virtual  int GetTransportInfo(ui4 instanceid,istring& currenttransportstate,istring& currenttransportstatus,
		istring& currentspeed);
	virtual  int GetPositionInfo(ui4 instanceid,ui4& track,istring& trackduration,istring & trackmetadata,
		istring& trackuri,istring& reltime,istring&abstime,i4 & relcount,i4& abscount);
	virtual  int GetDeviceCapabilities(ui4 instanceid,istring& playmedia,istring& recmedia,istring& recqualitymodes);
	virtual  int GetTransportSettings(ui4 instanceid,istring& playmode,istring& recqualitymode);
	virtual  int Stop(ui4 instanceid);

	virtual  int Play(ui4 instanceid,icstring speed);
	virtual  int Seek(ui4 instanceid,icstring unit,icstring target);
	virtual  int Next(ui4 instanceid);

	virtual  int Previous(ui4 instanceid);

private:
		     void GetLastChange(string result);
private:
	ui4 m_instanceid;
	istring m_uri;
	istring m_urimetadata;
	istring m_strActive;
};










#endif


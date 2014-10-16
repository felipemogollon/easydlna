#include "CESContentDirectory.h"
#include "ESDlnaHelper.h"

CESContentDirectory::CESContentDirectory()
:m_strActive(NULL)
{

}

CESContentDirectory::~CESContentDirectory()
{
	FREEARRAY(m_strActive);
}
int CESContentDirectory::GetSearchCapabilities(istring & searchcaps)
{
	return ERROR_INVALID_ACTION;
}

int CESContentDirectory::GetSortCapabilities(istring & sortcaps)
{
	return ERROR_INVALID_ACTION;
}

int CESContentDirectory::GetSystemUpdateID(ui4 & id)
{
	return ERROR_INVALID_ACTION;
}
int CESContentDirectory::Browse(icstring id,icstring browseflag,icstring filter,
											 ui4 startingindex,ui4 requestedcount,icstring sort,
											 istring& result,ui4&numberreturned,ui4&totalmatches,ui4& updateid)
{
	return ERROR_INVALID_ACTION;
}

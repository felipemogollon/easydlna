#ifndef __CUPNPDEVICE_H__
#define __CUPNPDEVICE_H__

#include "IUpnp.h" 
#include "IUpnpUtil.h"

class CUpnpDevice: public IUpnpDevice
{
public:
	CUpnpDevice();
	virtual ~CUpnpDevice();
	virtual ERROR_TYPE Register(const CRegisterType & cType);
	virtual ERROR_TYPE UnRegister();

	virtual ERROR_TYPE AddUpnpDeviceObserver(IUpnpDeviceObserver* IObserver);
	virtual ERROR_TYPE RemoveUpnpDeviceObserver(IUpnpDeviceObserver* IObserver);
	
	virtual ERROR_TYPE ExecuteCommand(CCommand & command);
	
public:
	static void DeviceCallBack(EventType eventType, void *Event, void *Cookie );
	void  innerDeviceCallBack(EventType eventType, void *Event);


private:
	void LoopCallObserver(EventType eventType,CEvent & event);
private:
	UpnpDevice_Handle m_devicehandle;
	volatile int	  m_deviceRegisterCount;
	IUpnpDeviceObserver* m_observer;
	IUpnpUtil::CRWLock	m_rwLock;
};







#endif


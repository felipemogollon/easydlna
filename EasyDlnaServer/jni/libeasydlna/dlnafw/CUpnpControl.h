#ifndef __CUPNPCONTROL_H__
#define __CUPNPCONTROL_H__

#include "IUpnp.h"
#include "IUpnpUtil.h"



class CUpnpControl: public IUpnpControlPoint
{
public:
	 CUpnpControl();
	virtual ~CUpnpControl();

public :
	static void ClientCallBack(EventType eventType, void *Event, void *Cookie );
	void   innerClientCallBack(EventType eventType, void *Event);
	
	static void* ThreadTickCountGenerator(void *arg);
	void* TickCountGenerator();
public:

	virtual ERROR_TYPE Register(const CRegisterType & cType);
	virtual ERROR_TYPE UnRegister();

	virtual ERROR_TYPE AddUpnpControlPointObserver(IUpnpControlPointObserver* IObserver);
	virtual ERROR_TYPE RemoveUpnpControlPointObserver(IUpnpControlPointObserver* observer);
	virtual ERROR_TYPE ExecuteCommand(CCommand & command);

private:
	void LoopCallObserver(EventType eventType,CEvent & event);
	void Stop();
	timespec* getTimeout(struct timespec* spec, unsigned numMs);

private:
	pthread_cond_t m_quitCond;
	pthread_mutex_t m_quitMutex;
	ThreadPool * m_pThreadPool;
	int m_TickCountThreadId;
	BOOL m_bQuitTickCountThread;

	UpnpClient_Handle m_nUpnpHandle;
	volatile   int m_registerCount;
	IUpnpUtil::CRWLock	m_rwLock;
	CListIUpnpObserver  m_ListObserver;
};








#endif


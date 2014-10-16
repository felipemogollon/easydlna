#ifndef __IDMCONTROLLER_H__
#define __IDMCONTROLLER_H__

class IDlnaService;
class IDlnaConnetionManager;
class IDlnaAVTransport;
class IDlnaRenderingControl;
class IDlnaContentDirectory;

class IUpnp;


class IDMControllerObserver
{
public:
	virtual ~IDMControllerObserver(){}
	virtual int Nortify(CDevice & dlnaDevice) = 0;
};

class IDMController
{
public:
	IDMController(){};
	virtual ~IDMController(){};

public:
	virtual int SetContentDirectory(IDlnaContentDirectory* pIContentDirectory) = 0;
	virtual int SetAVTransport(IDlnaAVTransport* pIAVTransport) = 0;
	virtual int SetRenderingControl(IDlnaRenderingControl* pIRenderingControl) = 0;
	virtual int SetConnectionManager(IDlnaConnetionManager* pIConnectionManager) = 0;

	virtual int ScanNetwork(const char* scanContent,int TimeOut) = 0;

	virtual int SetObserver(IDMControllerObserver* pObserver) = 0;

	virtual IDlnaService* QueryService(const char* serviceType) = 0;
	
	virtual CDevice* QueryDlnaDevice(const char* udn) = 0;

	virtual IUpnp*      QueryUpnp() = 0;

	
	virtual int Release() = 0;



};















#endif


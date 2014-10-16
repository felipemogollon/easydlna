#ifndef __ESRESOURCE_H__
#define __ESRESOURCE_H__

#define AVT_SCPDURL "/dlna/Render/AVTransport_scpd.xml"
#define CMS_SCPDURL "/dlna/Render/ConnectionManager_scpd.xml"
#define RCS_SCPDURL "/dlna/Render/RenderingControl_scpd.xml"

#define DMS_AVT_SCPDURL "/dlna/Server/AVTransport_scpd.xml"
#define DMS_CMS_SCPDURL "/dlna/Server/ConnectionManager_scpd.xml"
#define DMS_CDS_SCPDURL "/dlna/Server/ContentDirectory_scpd.xml"



#define EMPTY_VALUE ""

#define CONTENT_TYPE "text/xml"


#define ES_RENDER_DESCRIPTION \
"<?xml version=\"1.0\" encoding=\"utf-8\" ?>"  \
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">" \
"<specVersion>" \
"<major>1</major>" \
"<minor>0</minor>" \
"</specVersion>" \
"<device>" \
"<deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>" \
"<presentationURL>/</presentationURL>" \
"<friendlyName>%s</friendlyName>" \
"<manufacturer>EasyDlna</manufacturer>"\
"<manufacturerURL>http://www.notyet.com</manufacturerURL>" \
"<modelDescription>Media Renderer Device</modelDescription>"\
"<modelName>EasyDlna Media Renderer</modelName>" \
"<modelURL>http://www.notyet.com/</modelURL>" \
"<UDN>%s</UDN>"\
"<serviceList>"\
"<service>"\
"<serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>"\
"<serviceId>urn:upnp-org:serviceId:AVTransport</serviceId>"\
"<SCPDURL>/dlna/Render/AVTransport_scpd.xml</SCPDURL>"\
"<controlURL>_urn:schemas-upnp-org:service:AVTransport_control</controlURL>"\
"<eventSubURL>_urn:schemas-upnp-org:service:AVTransport_event</eventSubURL>"\
"</service>"\
"<service>"\
"<serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>"\
"<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>" \
"<SCPDURL>/dlna/Render/ConnectionManager_scpd.xml</SCPDURL>" \
"<controlURL>_urn:schemas-upnp-org:service:ConnectionManager_control</controlURL>" \
"<eventSubURL>_urn:schemas-upnp-org:service:ConnectionManager_event</eventSubURL>" \
"</service>"\
"<service>"\
"<serviceType>urn:schemas-upnp-org:service:RenderingControl:1</serviceType>" \
"<serviceId>urn:upnp-org:serviceId:RenderingControl</serviceId>"\
"<SCPDURL>/dlna/Render/RenderingControl_scpd.xml</SCPDURL>" \
"<controlURL>_urn:schemas-upnp-org:service:RenderingControl_control</controlURL>" \
"<eventSubURL>_urn:schemas-upnp-org:service:RenderingControl_event</eventSubURL>"\
"</service>"\
"</serviceList>"\
"</device>"\
"</root>"


#define ES_SERVER_DESCRIPTION \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>" \
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">" \
"  <specVersion>" \
"    <major>1</major>" \
"    <minor>0</minor>" \
"  </specVersion>" \
"  <device>" \
"    <deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType>" \
"    <friendlyName>%s</friendlyName>" \
"    <manufacturer>EasyDlna</manufacturer>" \
"    <manufacturerURL>http://www.notyet.com</manufacturerURL>" \
"    <modelDescription>Media Server Device</modelDescription>" \
"    <modelName>Easydlna Media Server</modelName>" \
"    <modelURL>http://www.notyet.com</modelURL>" \
"    <UDN>%s</UDN>" \
"    <serviceList>" \
"      <service>" \
"        <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>" \
"        <serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>" \
"        <SCPDURL>/dlna/Server/ConnectionManager_scpd.xml</SCPDURL>" \
"        <controlURL>_urn:schemas-upnp-org:service:ConnectionManager_control</controlURL>" \
"        <eventSubURL>_urn:schemas-upnp-org:service:ConnectionManager_event</eventSubURL>" \
"      </service>" \
"      <service>" \
"        <serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>" \
"        <serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId>" \
"        <SCPDURL>/dlna/Server/ContentDirectory_scpd.xml</SCPDURL>" \
"        <controlURL>_urn:schemas-upnp-org:service:ContentDirectory_control</controlURL>" \
"        <eventSubURL>_urn:schemas-upnp-org:service:ContentDirectory_event</eventSubURL>" \
"      </service>" \
"      <service>" \
"		<serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>"\
"		<serviceId>urn:upnp-org:serviceId:AVTransport</serviceId>"\
"		<SCPDURL>/dlna/Server/AVTransport_scpd.xml</SCPDURL>"\
"		<controlURL>_urn:schemas-upnp-org:service:AVTransport_control</controlURL>"\
"		<eventSubURL>_urn:schemas-upnp-org:service:AVTransport_event</eventSubURL>"\
"      </service>" \
"    </serviceList>" \
"  </device>" \
"</root>"



#endif




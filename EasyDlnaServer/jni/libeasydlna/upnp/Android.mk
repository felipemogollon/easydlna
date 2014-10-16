LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/inet_pton.c \
 	src/api/upnpapi.c \
 	src/api/upnpdebug.c \
	src/api/UpnpString.c \
    src/api/upnptools.c \
 	src/gena/gena_callback2.c \
 	src/gena/gena_ctrlpt.c \
 	src/gena/gena_device.c \
 	src/genlib/client_table/client_table.c \
 	src/genlib/miniserver/miniserver.c \
 	src/genlib/net/sock.c \
 	src/genlib/net/http/httpparser.c \
 	src/genlib/net/http/httpreadwrite.c \
 	src/genlib/net/http/parsetools.c \
 	src/genlib/net/http/statcodes.c \
 	src/genlib/net/http/webserver.c \
 	src/genlib/net/uri/uri.c \
 	src/genlib/service_table/service_table.c \
 	src/genlib/util/membuffer.c \
 	src/genlib/util/strintmap.c \
 	src/genlib/util/upnp_timeout.c \
 	src/genlib/util/util.c \
 	src/soap/soap_common.c \
 	src/soap/soap_ctrlpt.c \
 	src/soap/soap_device.c \
 	src/ssdp/ssdp_ctrlpt.c \
 	src/ssdp/ssdp_device.c \
 	src/ssdp/ssdp_server.c \
 	src/urlconfig/urlconfig.c \
 	src/uuid/md5.c \
 	src/uuid/sysdep.c \
 	src/uuid/uuid.c \

LOCAL_MODULE := static_easydlna_subupnp

LOCAL_CFLAGS := -DPV_CPU_ARCH_VERSION=0  -DOS_ANDROID -g -DUSE_NDK #-DDEBUG -DDEBUG_UPNP

#LOCAL_LDLIBS:=-L$(SYSROOT)/usr/lib -llog

LOCAL_STATIC_LIBRARIES := libstatic_easydlna_ixml libstatic_easydlna_threadutil


LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../ixml/inc \
	$(LOCAL_PATH)/../threadutil/inc \
	$(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/src/inc \
 	$(LOCAL_PATH)/../build/inc \

include $(BUILD_STATIC_LIBRARY)

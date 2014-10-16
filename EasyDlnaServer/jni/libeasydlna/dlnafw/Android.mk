LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

DLNA_SRC:= \
    CUpnpControl.cpp \
	CUpnpDevice.cpp \
	IDlnaFrame.cpp \
	IUpnp.cpp \
	IUpnpUtil.cpp
	
LOCAL_SRC_FILES :=\
	${DLNA_SRC}
	
IXML_UPNP_INCLUDE=`pwd`/jni/libeasydlna/ixml/inc
THREADUTIL_UPNP_INCLUDE=`pwd`/jni/libeasydlna/threadutil/inc
UPNP_UPNP_INCLUDE=`pwd`/jni/libeasydlna/upnp/inc

LOCAL_CFLAGS+=-g -I${UPNP_UPNP_INCLUDE} -I${IXML_UPNP_INCLUDE}  -I${THREADUTIL_UPNP_INCLUDE}  
#LOCAL_LDLIBS:=-L${SYSROOT}/usr/lib -llog


LOCAL_CFLAGS+=-DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -Dlinux -D__GLIBC__ -D_GNU_SOURCE -DDEBUG 
LOCAL_CFLAGS+=-DOS_ANDROID -DUSE_NDK #-DDEBUG -DDEBUG_UPNP

LOCAL_STATIC_LIBRARIES := libstatic_easydlna_ixml libstatic_easydlna_threadutil \
                          libstatic_easydlna_subupnp


LOCAL_MODULE := static_easydlna_dlnafw


include $(BUILD_STATIC_LIBRARY)



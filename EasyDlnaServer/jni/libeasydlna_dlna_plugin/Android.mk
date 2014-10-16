LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
EASYSHARE_TOP := $(LOCAL_PATH)

ESDLNAPLUGIN_SRC:=\
	CESDMRender.cpp\
	CESDMServer.cpp \
	CESDMController.cpp\
	CESDlnaPlugin.cpp\
    CESDlnaDeviceHandler.cpp\
    CESConnectionManager.cpp\
    CESRenderingControl.cpp\
    CESAVTransport.cpp\
    CESContentDirectory.cpp

   
DLNAPLGUIN_ANDROID_SRC:=\
     android/CESDlna_Android.cpp \
	 android/CESDMController_Android.cpp\
	 android/CESDMRender_Android.cpp\
	 android/CESDMServer_Android.cpp\
	
LOCAL_SRC_FILES :=\
	${ESDLNAPLUGIN_SRC} \
	${DLNAPLGUIN_ANDROID_SRC}
	

IXML_UPNP_INCLUDE=`pwd`/jni/libeasydlna/ixml/inc
THREADUTIL_UPNP_INCLUDE=`pwd`/jni/libeasydlna/threadutil/inc
UPNP_UPNP_INCLUDE=`pwd`/jni/libeasydlna/upnp/inc
DLNAFW_UPNP_INCLUDE=`pwd`/jni/libeasydlna/dlnafw

LOCAL_CFLAGS+=-g -I${UPNP_UPNP_INCLUDE} -I${IXML_UPNP_INCLUDE}\
                 -I${THREADUTIL_UPNP_INCLUDE} -I${DLNAFW_UPNP_INCLUDE}

LOCAL_CFLAGS+=-DHAVE_CONFIG_H -D_U_="__attribute__((unused))" \
               -Dlinux -D__GLIBC__ -D_GNU_SOURCE -DUSE_NDK \
               #-DDEBUG -DDEBUG_UPNP
               
LOCAL_CFLAGS+=-DOS_ANDROID -DUSE_NDK -DHAVE_BOOLEAN
LOCAL_CFLAGS+=-DPLUGIN_DIR=\"libeasydlna_dlna_plugin.so\"
LOCAL_SHARED_LIBRARIES := libeasydlna
LOCAL_LDLIBS:=-L$(SYSROOT)/usr/lib -llog
TARGET_PRELINK_MODULE:=false

LOCAL_MODULE:= libeasydlna_dlna_plugin

#LOCAL_UNSTRIPPED_PATH := ${TARGET_ROOT_OUT_UNSTRIPPED}
		
include $(BUILD_SHARED_LIBRARY)



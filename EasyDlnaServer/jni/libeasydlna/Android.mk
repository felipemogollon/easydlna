LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

EASYDLNA_TOP := $(LOCAL_PATH)

#Modify libupnp into libeasydlna

LOCAL_WHOLE_STATIC_LIBRARIES := \
	libstatic_easydlna_ixml \
	libstatic_easydlna_threadutil \
	libstatic_easydlna_subupnp \
    libstatic_easydlna_dlnafw
	
LOCAL_MODULE := libeasydlna

LOCAL_STATIC_LIBRARIES := libstatic_easydlna_ixml \
 	libstatic_easydlna_threadutil \
 	libstatic_easydlna_subupnp \
    libstatic_easydlna_dlnafw

LOCAL_LDLIBS:=-L$(SYSROOT)/usr/lib -llog
LOCAL_PRELINK_MODULE := false

 
LOCAL_CFLAGS := -DPV_CPU_ARCH_VERSION=0  -DOS_ANDROID -DUSE_NDK\
                #-DDEBUG -DDEBUG_UPNP   #if you want debug info, uncomment this line
                
include $(BUILD_SHARED_LIBRARY)
include $(EASYDLNA_TOP)/ixml/Android.mk
include $(EASYDLNA_TOP)/threadutil/Android.mk
include $(EASYDLNA_TOP)/upnp/Android.mk
include $(EASYDLNA_TOP)/dlnafw/Android.mk



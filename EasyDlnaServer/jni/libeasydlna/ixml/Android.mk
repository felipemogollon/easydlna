LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/attr.c \
 	src/document.c \
 	src/element.c \
 	src/ixml.c \
 	src/ixmlmembuf.c \
	src/ixmldebug.c \
 	src/ixmlparser.c \
 	src/namedNodeMap.c \
 	src/node.c \
 	src/nodeList.c
 	
LOCAL_C_INCLUDES := \
       $(LOCAL_PATH)/../upnp/inc \
       $(LOCAL_PATH)/inc \
       $(LOCAL_PATH)/src/inc

LOCAL_MODULE := static_easydlna_ixml

LOCAL_CFLAGS := -DPV_CPU_ARCH_VERSION=0 -DOS_ANDROID -DUSE_NDK #-DDEBUG -DDEBUG_UPNP



include $(BUILD_STATIC_LIBRARY)

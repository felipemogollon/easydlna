LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/FreeList.c \
 	src/LinkedList.c \
 	src/ThreadPool.c \
 	src/TimerThread.c 

LOCAL_MODULE := static_easydlna_threadutil

LOCAL_CFLAGS := -DPV_CPU_ARCH_VERSION=0  -DOS_ANDROID  -DUSE_NDK #-DDEBUG -DDEBUG_UPNP

LOCAL_C_INCLUDES := \
       $(LOCAL_PATH)/../upnp/inc \
       $(LOCAL_PATH)/inc \
       $(LOCAL_PATH)/src/inc

LOCAL_STATIC_LIBRARIES := 

LOCAL_SHARED_LIBRARIES := 

include $(BUILD_STATIC_LIBRARY)

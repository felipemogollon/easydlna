LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_SRC_FILES += \
	src/com/easydlna/dlna/dmpservice/IDMCService.aidl \
	src/com/easydlna/dlna/dmpservice/IDMCCallback.aidl \
	src/com/easydlna/dlna/dmpservice/IDMRService.aidl \
	src/com/easydlna/dlna/dmpservice/IDMRCallback.aidl


LOCAL_PACKAGE_NAME := EasyDlnaServer
#include $(all-subdir-makefiles)
include $(BUILD_PACKAGE)


##build to jar

LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under)
LOCAL_MODULE := EasyDlnaAPI
LOCAL_CERTIFICATE := platform
LOCAL_DEX_PREOPT := false

include $(BUILD_STATIC_JAVA_LIBRARY)

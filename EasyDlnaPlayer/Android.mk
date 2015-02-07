LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
#LOCAL_JAVA_LIBRARIES := EasyDlnaAPI

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PACKAGE_NAME := EasyDlnaPlayer
LOCAL_CERTIFICATE := platform
LOCAL_DEX_PREOPT := false
LOCAL_STATIC_JAVA_LIBRARIES := libEasyDlnaAPI

include $(BUILD_PACKAGE)

include $(CLEAR_VARS)
LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libEasyDlnaAPI:libs/EasyDlnaAPI.jar
include $(BUILD_MULTI_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))

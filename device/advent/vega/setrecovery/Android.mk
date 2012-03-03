ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),p10an01)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := setrecovery.c
LOCAL_MODULE := setrecovery
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := $(call include-path-for, recovery)
LOCAL_STATIC_LIBRARIES := libmtdutils
include $(BUILD_EXECUTABLE)

endif

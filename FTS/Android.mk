LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := fts

include ../cflags.mk

LOCAL_C_INCLUDES := \
  					$(LOCAL_PATH)/../libs/khronos/openxr/OpenXR-SDK/include \
  					$(LOCAL_PATH)/src/main/cpp/include \

LOCAL_SRC_FILES	:= $(wildcard $(LOCAL_PATH)/src/main/cpp/*.cpp)

# include default libraries
LOCAL_LDLIBS 			:= -llog -landroid -lvulkan
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_SHARED_LIBRARIES := openxr_loader

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,libs/meta/OpenXR/Projects/AndroidPrebuilt/jni)

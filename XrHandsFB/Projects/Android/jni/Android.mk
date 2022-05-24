LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := xrhandsfb

include ../../../../cflags.mk

LOCAL_C_INCLUDES := \
  					$(LOCAL_PATH)/../../../../../3rdParty/khronos/openxr/OpenXR-SDK/include \
  					$(LOCAL_PATH)/../../../../../example_lib \

LOCAL_SRC_FILES		:= 	../../../Src/main.cpp \
						../../../Src/graphicsplugin_factory.cpp \
						../../../Src/graphicsplugin_opengles.cpp \
						../../../Src/graphicsplugin_vulkan.cpp \
						../../../Src/logger.cpp \
						../../../Src/openxr_program.cpp \
						../../../Src/pch.cpp \
						../../../Src/platformplugin_android.cpp \
						../../../Src/platformplugin_factory.cpp \

# include default libraries
LOCAL_LDLIBS 			:= -llog -landroid -lGLESv3 -lEGL -lvulkan
LOCAL_STATIC_LIBRARIES 	:= android_native_app_glue
LOCAL_SHARED_LIBRARIES := openxr_loader

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,OpenXR/Projects/AndroidPrebuilt/jni)

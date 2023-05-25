#pragma once

#include "pch.h"

static std::string vkResultString(VkResult res) {
    switch (res) {
        case VK_SUCCESS:
            return "SUCCESS";
        case VK_NOT_READY:
            return "NOT_READY";
        case VK_TIMEOUT:
            return "TIMEOUT";
        case VK_EVENT_SET:
            return "EVENT_SET";
        case VK_EVENT_RESET:
            return "EVENT_RESET";
        case VK_INCOMPLETE:
            return "INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "ERROR_INVALID_SHADER_NV";
        default:
            return std::to_string(res);
    }
}

[[noreturn]] inline void ThrowVkResult(VkResult res, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    Throw(Fmt("VkResult failure [%s]", vkResultString(res).c_str()), originator, sourceLocation);
}

inline VkResult CheckVkResult(VkResult res, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    if ((res) < VK_SUCCESS) {
        ThrowVkResult(res, originator, sourceLocation);
    }
    return res;
}

#define THROW_VK(res, cmd) ThrowVkResult(res, #cmd, FILE_AND_LINE)
#define CHECK_VKCMD(cmd) CheckVkResult(cmd, #cmd, FILE_AND_LINE)
#define CHECK_VKRESULT(res, cmdStr) CheckVkResult(res, cmdStr, FILE_AND_LINE)

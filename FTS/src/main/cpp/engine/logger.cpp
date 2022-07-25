// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include "include/pch.h"
#include "include/logger.h"

#include "../../../../../../../Android/Sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1/sstream"

#if defined(ANDROID)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "fts", __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "fts", __VA_ARGS__)
#endif

namespace {
Log::Level g_minSeverity{Log::Level::Info};
std::mutex g_logLock;
}  // namespace

namespace Log {
void SetLevel(Level minSeverity) { g_minSeverity = minSeverity; }

void Write(Level severity, const std::string& msg) {
    if (severity < g_minSeverity) {
        return;
    }

    const auto now = std::chrono::system_clock::now();
    const time_t now_time = std::chrono::system_clock::to_time_t(now);
    tm now_tm;
    localtime_r(&now_time, &now_tm);
    // time_t only has second precision. Use the rounding error to get sub-second precision.
    const auto secondRemainder = now - std::chrono::system_clock::from_time_t(now_time);
    const int64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(secondRemainder).count();

    static std::map<Level, const char*> severityName = {
        {Level::Verbose, "RVR_Verbose"}, {Level::Info, "RVR_Info___"}, {Level::Warning, "RVR_Warning"}, {Level::Error, "RVR_Error__"}};

    std::ostringstream out;
    out.fill('0');
    out << "[" << std::setw(2) << now_tm.tm_hour << ":" << std::setw(2) << now_tm.tm_min << ":" << std::setw(2) << now_tm.tm_sec
        << "." << std::setw(3) << milliseconds << "]"
        << "[" << severityName[severity] << "] " << msg << std::endl;

    std::lock_guard<std::mutex> lock(g_logLock);  // Ensure output is serialized
    ((severity == Level::Error) ? std::clog : std::cout) << out.str();
    if (severity == Level::Error)
        ALOGE("%s", out.str().c_str());
    else
        ALOGV("%s", out.str().c_str());
}
}  // namespace Log

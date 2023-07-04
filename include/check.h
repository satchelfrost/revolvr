/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "pch.h"
#include "common.h"

#define CHK_STRINGIFY(x) #x
#define TOSTRING(x) CHK_STRINGIFY(x)
#define FILE_AND_LINE __FILE__ ":" TOSTRING(__LINE__)

[[noreturn]] inline void Throw(std::string failureMessage, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    if (originator != nullptr) {
        failureMessage += Fmt("\n    Origin: %s", originator);
    }
    if (sourceLocation != nullptr) {
        failureMessage += Fmt("\n    Source: %s", sourceLocation);
    }

    Log::Write(Log::Level::Error, Fmt("%s", failureMessage.c_str()));
    std::terminate();
}

[[noreturn]] inline void ThrowEntity(std::string failureMessage,
                                     std::string entityName,
                                     const char* originator = nullptr,
                                     const char* sourceLocation = nullptr) {
    failureMessage = "[Entity: " + entityName + "] - " + failureMessage;
    if (originator != nullptr) {
        failureMessage += Fmt("\n    Origin: %s", originator);
    }
    if (sourceLocation != nullptr) {
        failureMessage += Fmt("\n    Source: %s", sourceLocation);
    }

    Log::Write(Log::Level::Error, Fmt("%s", failureMessage.c_str()));
    std::terminate();
}

#define ENTITY_ERR(msg, entityName) ThrowEntity(msg, entityName, nullptr, FILE_AND_LINE);
#define THROW(msg) Throw(msg, nullptr, FILE_AND_LINE);
#define CHECK(exp)                                      \
    {                                                   \
        if (!(exp)) {                                   \
            Throw("Check failed", #exp, FILE_AND_LINE); \
        }                                               \
    }
#define CHECK_MSG(exp, msg)                  \
    {                                        \
        if (!(exp)) {                        \
            Throw(msg, #exp, FILE_AND_LINE); \
        }                                    \
    }

[[noreturn]] inline void ThrowXrResult(XrResult res, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    Throw(Fmt("XrResult failure [%s]", to_string(res)), originator, sourceLocation);
}

inline XrResult CheckXrResult(XrResult res, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    if (XR_FAILED(res)) {
        ThrowXrResult(res, originator, sourceLocation);
    }

    return res;
}

#define THROW_XR(xr, cmd) ThrowXrResult(xr, #cmd, FILE_AND_LINE);
#define CHECK_XRCMD(cmd) CheckXrResult(cmd, #cmd, FILE_AND_LINE);
#define CHECK_XRRESULT(res, cmdStr) CheckXrResult(res, cmdStr, FILE_AND_LINE);

#ifdef XR_USE_PLATFORM_WIN32

[[noreturn]] inline void ThrowHResult(HRESULT hr, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    Throw(Fmt("HRESULT failure [%x]", hr), originator, sourceLocation);
}

inline HRESULT CheckHResult(HRESULT hr, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    if (FAILED(hr)) {
        ThrowHResult(hr, originator, sourceLocation);
    }

    return hr;
}

#define THROW_HR(hr, cmd) ThrowHResult(hr, #cmd, FILE_AND_LINE);
#define CHECK_HRCMD(cmd) CheckHResult(cmd, #cmd, FILE_AND_LINE);
#define CHECK_HRESULT(res, cmdStr) CheckHResult(res, cmdStr, FILE_AND_LINE);

#endif

/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Log {
enum class Level { Verbose, Info, Warning, Error };

void SetLevel(Level minSeverity);
void Write(Level severity, const std::string& msg);
}

namespace rvr {
void PrintInfo(const std::string &msg);
void PrintWarning(const std::string &msg);
void PrintError(const std::string &msg);
void PrintVerbose(const std::string &msg);
}

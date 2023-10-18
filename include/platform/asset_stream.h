/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

namespace rvr {
class AssetStream : public std::streambuf {
public:
    AssetStream(const std::string& fileName);
    ~AssetStream();
private:
    AAsset * asset_;
};
}

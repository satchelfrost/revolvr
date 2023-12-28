/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <platform/asset_stream.h>
#include <global_context.h>

namespace rvr {
AssetStream::AssetStream(const std::string &fileName) {
    asset_ = AAssetManager_open(GlobalContext::Inst()->GetAndroidContext()->GetAndroidAssetManager(),
                                fileName.c_str(),
                                AASSET_MODE_BUFFER);
    if (asset_) {
        char *begin = (char *) AAsset_getBuffer(asset_);
        char *end = begin + AAsset_getLength64(asset_);
        setg(begin, begin, end);
    }
}

AssetStream::~AssetStream() {
    if (asset_)
        AAsset_close(asset_);
}
};
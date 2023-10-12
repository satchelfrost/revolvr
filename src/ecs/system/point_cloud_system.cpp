/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "ecs/system/point_cloud_system.h"
#include "happly.h"
#include "common.h"
#include "global_context.h"

namespace rvr::system::point_cloud {
class asset_streambuf : public std::streambuf {
public:
    asset_streambuf(AAsset * the_asset)
            : the_asset_(the_asset) {
        char * begin = (char *)AAsset_getBuffer(the_asset);
        char * end = begin + AAsset_getLength64(the_asset);
        setg(begin, begin, end);
    }
    ~asset_streambuf() {
        AAsset_close(the_asset_);
    }
private:
    AAsset * the_asset_;
};
std::vector<Geometry::Vertex> getVertexDataFromPly(const std::string& fileName) {
//    AAsset *file = AAssetManager_open(GlobalContext::Inst()->GetAndroidContext()->GetAndroidAssetManager(),
//                                      fileName.c_str(),
//                                      AASSET_MODE_BUFFER);
//    off_t file_length = AAsset_getLength(file);
//    char *file_content = new char[file_length + 1];
//    AAsset_read(file, file_content, file_length);
//    AAsset_close(file);
//    file_content[file_length] = '\0';
//
    AAsset *asset = AAssetManager_open(GlobalContext::Inst()->GetAndroidContext()->GetAndroidAssetManager(),
                                      fileName.c_str(),
                                      AASSET_MODE_BUFFER);
    asset_streambuf sb(asset);
    std::istream is(&sb);
//    auto stream = std::istringstream(file_content);
    std::vector<Geometry::Vertex> vertices;
    try {
        happly::PLYData plyIn(is);
        std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
        std::vector<std::array<unsigned char, 3>> vColor = plyIn.getVertexColors();
        if (vPos.size() != vColor.size())
            throw std::runtime_error("vertex positions and colors do not match in size");

        vertices.resize(vPos.size());
        Geometry::Vertex vertex{};
        for (size_t i = 0; i < vPos.size(); i++) {
            vertex.Position.x = (float)vPos[i][0];
            vertex.Position.y = (float)vPos[i][1];
            vertex.Position.z = (float)vPos[i][2];
            vertex.Color.x    = (float)vColor[i][0] / 255.0f;
            vertex.Color.y    = (float)vColor[i][1] / 255.0f;
            vertex.Color.z    = (float)vColor[i][2] / 255.0f;
            vertices[i] = vertex;
        }
    } catch (const std::exception& e) {
        rvr::PrintError(e.what());
    }

    return vertices;
}
}

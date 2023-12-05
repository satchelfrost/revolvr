/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#define DECL_PFN(pfn) PFN_##pfn pfn = nullptr

namespace rvr {
class PassThrough {
public:
    PassThrough(XrInstance instance, XrSession session);

    // Create passthrough objects
    XrPassthroughFB passthrough = XR_NULL_HANDLE;
    XrPassthroughLayerFB passthroughLayer = XR_NULL_HANDLE;

private:

    // Passthrough function pointers
    DECL_PFN(xrCreatePassthroughFB);
    DECL_PFN(xrDestroyPassthroughFB);
    DECL_PFN(xrPassthroughStartFB);
    DECL_PFN(xrPassthroughPauseFB);
    DECL_PFN(xrCreatePassthroughLayerFB);
    DECL_PFN(xrDestroyPassthroughLayerFB);
    DECL_PFN(xrPassthroughLayerSetStyleFB);
    DECL_PFN(xrPassthroughLayerPauseFB);
    DECL_PFN(xrPassthroughLayerResumeFB);
    DECL_PFN(xrCreateTriangleMeshFB);
    DECL_PFN(xrDestroyTriangleMeshFB);
    DECL_PFN(xrTriangleMeshGetVertexBufferFB);
    DECL_PFN(xrTriangleMeshGetIndexBufferFB);
    DECL_PFN(xrTriangleMeshBeginUpdateFB);
    DECL_PFN(xrTriangleMeshEndUpdateFB);
    DECL_PFN(xrCreateGeometryInstanceFB);
    DECL_PFN(xrDestroyGeometryInstanceFB);
    DECL_PFN(xrGeometryInstanceSetTransformFB);
};
}

#undef DECL_PFN

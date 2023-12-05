/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/passthrough.h>
#define INIT_PFN(pfn) CHECK_XRCMD(xrGetInstanceProcAddr(instance, #pfn, (PFN_xrVoidFunction*)(&pfn)))

namespace rvr {
PassThrough::PassThrough(XrInstance instance, XrSession session) {
    CHECK_MSG(instance, "XR instance was null");
    CHECK_MSG(session, "XR session was null");

    INIT_PFN(xrCreatePassthroughFB);
    INIT_PFN(xrDestroyPassthroughFB);
    INIT_PFN(xrPassthroughStartFB);
    INIT_PFN(xrPassthroughPauseFB);
    INIT_PFN(xrCreatePassthroughLayerFB);
    INIT_PFN(xrDestroyPassthroughLayerFB);
    INIT_PFN(xrPassthroughLayerSetStyleFB);
    INIT_PFN(xrPassthroughLayerPauseFB);
    INIT_PFN(xrPassthroughLayerResumeFB);
    INIT_PFN(xrCreateTriangleMeshFB);
    INIT_PFN(xrDestroyTriangleMeshFB);
    INIT_PFN(xrTriangleMeshGetVertexBufferFB);
    INIT_PFN(xrTriangleMeshGetIndexBufferFB);
    INIT_PFN(xrTriangleMeshBeginUpdateFB);
    INIT_PFN(xrTriangleMeshEndUpdateFB);
    INIT_PFN(xrCreateGeometryInstanceFB);
    INIT_PFN(xrDestroyGeometryInstanceFB);
    INIT_PFN(xrGeometryInstanceSetTransformFB);

    // Create passthrough and passthrough layer
    XrPassthroughCreateInfoFB passthroughCI = {XR_TYPE_PASSTHROUGH_CREATE_INFO_FB};
    CHECK_XRCMD(xrCreatePassthroughFB(session, &passthroughCI, &passthrough));
    XrPassthroughLayerCreateInfoFB passthroughLayerCI = {XR_TYPE_PASSTHROUGH_LAYER_CREATE_INFO_FB};
    passthroughLayerCI.passthrough = passthrough;
    passthroughLayerCI.purpose = XR_PASSTHROUGH_LAYER_PURPOSE_RECONSTRUCTION_FB;
    CHECK_XRCMD(xrCreatePassthroughLayerFB(session, &passthroughLayerCI, &passthroughLayer));

    // Start passthrough and Create the passthrough style
    CHECK_XRCMD(xrPassthroughStartFB(passthrough));
    CHECK_XRCMD(xrPassthroughLayerResumeFB(passthroughLayer));
    XrPassthroughStyleFB style{XR_TYPE_PASSTHROUGH_STYLE_FB};
    style.textureOpacityFactor = 0.5f;
    style.edgeColor = {0.0f, 0.0f, 0.0f, 0.0f};
    CHECK_XRCMD(xrPassthroughLayerSetStyleFB(passthroughLayer, &style));
}
}

/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/vulkan_context.h>
#include <rendering/utilities/vulkan_results.h>
#include <platform/android_context.h>
#include <xr_context.h>
#include <global_context.h>
#include <ecs/system/spatial_system.h>
#include <ecs/system/lighting_system.h>
#include <math/linear_math.h>
#include <rendering/utilities/vulkan_utils.h>
#include <rendering/utilities/vertex_buffer_layout.h>
#include <rendering/utilities/vulkan_shader.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/point_light.h>
#include "ecs/component/types/mesh.h"

namespace rvr {
void VulkanContext::InitDevice(XrInstance xrInstance, XrSystemId systemId) {
    CheckVulkanGraphicsRequirements2KHR(xrInstance, systemId);
    CreateVulkanInstance(xrInstance, systemId);
    SetupReportCallback();
    PickPhysicalDevice(xrInstance, systemId);
    CreateLogicalDevice(xrInstance, systemId);
    CreateCommandPool();
    RetrieveQueues();
    StoreGraphicsBinding();
}

void VulkanContext::CreateVulkanInstance(XrInstance xrInstance, XrSystemId systemId) {
    // Optional application information
    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "app";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "RevolVR";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Vulkan instance information
    VkInstanceCreateInfo vkCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    vkCreateInfo.pApplicationInfo = &appInfo;
    std::vector<const char *> extensions = {"VK_EXT_debug_report"};
    bool validationLayerFound = CheckValidationLayerSupport();
    if (enableValidationLayers_ && validationLayerFound) {
        vkCreateInfo.enabledLayerCount = (uint32_t) requestedValidationLayers_.size();
        vkCreateInfo.ppEnabledLayerNames = requestedValidationLayers_.data();
        vkCreateInfo.enabledExtensionCount = (uint32_t) extensions.size();
        vkCreateInfo.ppEnabledExtensionNames = extensions.data();
    }
    else {
        vkCreateInfo.enabledLayerCount = 0;
        vkCreateInfo.ppEnabledLayerNames = nullptr;
        vkCreateInfo.enabledExtensionCount = (uint32_t) extensions.size();
        vkCreateInfo.ppEnabledExtensionNames = extensions.data();
    }

    // OpenXR-Specific Vulkan instance information
    XrVulkanInstanceCreateInfoKHR createInfo{XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR};
    createInfo.systemId = systemId;
    createInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
    createInfo.vulkanCreateInfo = &vkCreateInfo;
    createInfo.vulkanAllocator = nullptr;

    VkResult vkResult;
    XrResult xrResult;
    xrResult = CreateVulkanInstanceKHR(xrInstance, &createInfo, &instance_,
                                       &vkResult);
    CHECK_XRCMD(xrResult);
    CHECK_VKCMD(vkResult);
}

void VulkanContext::InitializeResources() {
    InitCubeResources();

    std::set<std::string> uniqueNames = system::render::GetUniqueModelNames();
    // Only try to load gltf models if necessary
    if (!uniqueNames.empty()) {
        SetupDescriptors();
        InitGltfResources();
        InitLightsResources();
        usingGltf_ = true;
    }

    uniqueNames = system::render::GetUniquePointCloudNames();
    if (!uniqueNames.empty())
        InitPointCloudResources();
}

XrSwapchainImageBaseHeader* VulkanContext::AllocateSwapchainImageStructs(
        uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) {
    //

    auto context = std::make_shared<SwapchainImageContext>(renderingContext_,
                                                                         capacity, swapchainCreateInfo);
    auto images = context->GetFirstImagePointer();
    imageToSwapchainContext_.insert(std::make_pair(images, context));
    return images;
}

void VulkanContext::RenderView(const XrCompositionLayerProjectionView &layerView,
                               const XrSwapchainImageBaseHeader *swapchainImage, uint32_t imageIndex) {
    // Texture arrays not supported.
    CHECK(layerView.subImage.imageArrayIndex == 0);
    // Compute the view-projection transform.
    // Note all matrices (including OpenXR) are column-major, right-handed.
    glm::mat4 projectionMatrix = math::matrix::CreateProjectionFromXrFOV(layerView.fov, 0.05f, 100.0f);
    glm::mat4 poseMatrix = math::Pose(layerView.pose).ToMat4();
    glm::mat4 viewMatrix = glm::affineInverse(poseMatrix);
    glm::mat4 viewProjection = projectionMatrix * viewMatrix;


    std::vector<glm::mat4> cubeMvps;
    renderBuffer_.clear();
    system::render::AppendCubeTransformBuffer(renderBuffer_);
    for (const math::Transform &model : renderBuffer_) {
        glm::mat4 modelMatrix = model.ToMat4();
        glm::mat4 mvp = viewProjection * modelMatrix;
        cubeMvps.push_back(mvp);
    }

    // Update uniform buffer for gltf models
    if (usingGltf_) {
        uboScene.projection = projectionMatrix;
        uboScene.view = viewMatrix;
        auto position = math::Pose(layerView.pose).GetPosition();
        uboScene.viewPos = glm::vec4(position, 0.0f);

        std::vector<PointLight*> pointLights;
        system::lighting::AppendLightSources(pointLights);
        uboScene.numLights = (int)pointLights.size();
        for (int i = 0; i < pointLights.size(); i++) {
            PointLight* pointLight = pointLights[i];
            auto* spatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(pointLight->id);
            if (spatial) {
                // Update ubo
                glm::vec3 lightPos = system::spatial::GetPlayerRelativeTransform(spatial).GetPosition();
                uboScene.pointLights[i].position = glm::vec4(lightPos, 1.0f);
                uboScene.pointLights[i].color = glm::vec4(pointLight->GetColor(), pointLight->GetIntensity());

                // Update light point light push constant info
                auto* mesh = GlobalContext::Inst()->GetECS()->GetComponent<Mesh>(pointLight->id);
                if (mesh) {
                    if (mesh->GetPrimitiveType() == Mesh::Quad) {
                        PointLightPushConst pushConst{};
                        pushConst.radius = 0.25f;
                        pushConst.position = uboScene.pointLights[i].position;
                        pushConst.color = uboScene.pointLights[i].color;
                        lightPushConstInfo_.push_back(pushConst);
                    } else {
                        PrintWarning("Cannot render light, quad mesh not available id = " +
                                     std::to_string(pointLight->id));
                    }
                }
            }
            else {
                PrintWarning("PointLight missing spatial. Eid = " + std::to_string(pointLight->id));
            }
        }

        // Hacky sorting
        std::map<float, PointLightPushConst> sorted;
        for (auto pushConst: lightPushConstInfo_) {
            auto offset = uboScene.viewPos - pushConst.position;
            float distSqrd = glm::dot(offset, offset);
            sorted[distSqrd] = pushConst;
        }
        lightPushConstInfo_.clear();
        for (auto& [key, pushConst] : sorted)
            lightPushConstInfo_.push_back(pushConst);

        uniformBuffer_->WriteToBuffer(&uboScene);
    }

    // Update the transforms for each gltf model
    system::render::AppendGltfModelPushConstants(models_);

    // Update the transforms for each point cloud
    system::render::AppendPointCloudPushConstants(pointClouds_, viewProjection);

    // Acquire swapchain context and begin render pass
    auto swapchainContext = imageToSwapchainContext_[swapchainImage];
    swapchainContext->BeginRenderPass(imageIndex);
    swapchainContext->Draw(cubePipeline_, cubeBuffer_, cubeMvps);
    for (auto& [name, model] : models_) {
        swapchainContext->DrawGltf(gltfPipeline_, model, uboSceneDescriptorSet_);
        swapchainContext->DrawGltf(outlinePipeline_, model, uboSceneDescriptorSet_);
        model->ClearPushConstants();
    }
    if (usingGltf_) {
        swapchainContext->DrawLights(lightsPipeline_, lightBuffer_, lightPushConstInfo_,
                                     uboSceneDescriptorSet_);
        lightPushConstInfo_.clear();
    }
    for (auto& [name, pointCloud] : pointClouds_) {
        swapchainContext->DrawPointCloud(pointCloudPipeline_, pointCloud);
        pointCloud->ClearPushConstants();
    }
    swapchainContext->EndRenderPass();
}

std::vector<std::string> VulkanContext::GetInstanceExtensions() {
    return {XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME};
}

uint32_t VulkanContext::GetSupportedSwapchainSampleCount(const XrViewConfigurationView &) {
    return VK_SAMPLE_COUNT_1_BIT;
}

const XrBaseInStructure* VulkanContext::GetGraphicsBinding() const {
    return reinterpret_cast<const XrBaseInStructure *>(&graphicsBinding_);
}

bool VulkanContext::CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : requestedValidationLayers_) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                PrintInfo("Requested validation layer found " + std::string(layerName));
                break;
            }
        }

        if (!layerFound) {
            PrintWarning("No validation layers found");
            return false;
        }
    }
    return true;
}

void VulkanContext::SetupReportCallback() {
    if (!enableValidationLayers_)
        return;

    VkDebugReportCallbackCreateInfoEXT createInfo{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                       VK_DEBUG_REPORT_WARNING_BIT_EXT |
                       VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                       VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                       VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    createInfo.pfnCallback = DebugReportCallback;
    createInfo.pUserData = this;
    CreateDebugReportCallbackEXT(instance_, &createInfo, nullptr, &debugReporter_);
}

void VulkanContext::Cleanup() {
    if (enableValidationLayers_)
        DestroyDebugReportCallbackEXT(instance_, debugReporter_, nullptr);
}

void VulkanContext::PickPhysicalDevice(XrInstance xrInstance, XrSystemId systemId) {
    XrVulkanGraphicsDeviceGetInfoKHR deviceGetInfo{XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR};
    deviceGetInfo.systemId = systemId;
    deviceGetInfo.vulkanInstance = instance_;
    CHECK_XRCMD(GetVulkanGraphicsDevice2KHR(xrInstance, &deviceGetInfo,
                                            &physicalDevice_));
}

void VulkanContext::CreateLogicalDevice(XrInstance xrInstance, XrSystemId systemId) {
    queueFamilyIndices_ = FindQueueFamilies(physicalDevice_);
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndices_.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    std::vector<VkFormat> formatList = {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
    };

    bool suitable_format = false;

    for (auto& format : formatList)
    {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(physicalDevice_, format, &formatProps);
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            PrintInfo("format " + std::to_string(format));
            suitable_format = true;
        }
    }

    if (!suitable_format)
        PrintError("No suitable format");

    // Create the device
    std::vector<const char *> deviceExtensions;
    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo deviceInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.enabledExtensionCount = (uint32_t) deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = nullptr;
    deviceInfo.pEnabledFeatures = &features;

    // OpenXr-Specific device create information
    XrVulkanDeviceCreateInfoKHR deviceCreateInfo{XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR};
    deviceCreateInfo.systemId = systemId;
    deviceCreateInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
    deviceCreateInfo.vulkanCreateInfo = &deviceInfo;
    deviceCreateInfo.vulkanPhysicalDevice = physicalDevice_;
    deviceCreateInfo.vulkanAllocator = nullptr;

    VkResult vkResult;
    XrResult xrResult;
    xrResult = CreateVulkanDeviceKHR(xrInstance, &deviceCreateInfo, &device_,
                                     &vkResult);
    CHECK_XRCMD(xrResult);
    CHECK_VKCMD(vkResult);
}

void VulkanContext::CreateCommandPool() {
    VkCommandPoolCreateInfo cmdPoolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndices_.graphicsFamily.value();
    CHECK_VKCMD(vkCreateCommandPool(device_, &cmdPoolInfo, nullptr,
                                    &graphicsPool_));
}

void VulkanContext::StoreGraphicsBinding() {
    graphicsBinding_.type = XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR;
    graphicsBinding_.instance = instance_;
    graphicsBinding_.physicalDevice = physicalDevice_;
    graphicsBinding_.device = device_;
    graphicsBinding_.queueFamilyIndex = queueFamilyIndices_.graphicsFamily.value();
    graphicsBinding_.queueIndex = 0;
}

void VulkanContext::RetrieveQueues() {
    vkGetDeviceQueue(device_, queueFamilyIndices_.graphicsFamily.value(), 0,
                     &graphicsQueue_);
}

void VulkanContext::SwapchainImagesReady(XrSwapchainImageBaseHeader *images) {
    auto context = imageToSwapchainContext_[images];
    context->InitRenderTargets();
}

void VulkanContext::InitRenderingContext(VkFormat colorFormat) {
    renderingContext_ = std::make_shared<RenderingContext>(physicalDevice_, device_,
                                                           graphicsQueue_, colorFormat,
                                                           graphicsPool_);
}

void VulkanContext::InitCubeResources() {
    auto vert = std::make_unique<VulkanShader>(device_,
                                                       "shaders/basic.vert.spv",
                                                       VulkanShader::Vertex);
    vert->PushConstant("Model View Projection Matrix", sizeof(glm::mat4));
    auto frag= std::make_unique<VulkanShader>(device_,
                                              "shaders/basic.frag.spv",
                                              VulkanShader::Fragment);
    cubeShaderStages_ = std::make_unique<ShaderStages>(device_, std::move(vert),
                                                     std::move(frag));
    VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.Push({0, DataType::F32, 3, "Position"});
    vertexBufferLayout.Push({1, DataType::F32, 3, "Color"});
    size_t sizeOfIndex = sizeof(Geometry::c_cubeIndices[0]);
    size_t sizeOfVertex = sizeof(Geometry::c_cubeVertices[0]);
    size_t indexCount = sizeof(Geometry::c_cubeIndices) / sizeOfIndex;
    size_t vertexCount = sizeof(Geometry::c_cubeVertices) / sizeOfVertex;
    auto indexBuffer = std::make_unique<VulkanBuffer>(renderingContext_,
                                                      sizeOfIndex, indexCount,
                                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                  MemoryType::HostVisible);
    auto vertexBuffer = std::make_unique<VulkanBuffer>(renderingContext_,
                                                       sizeOfVertex, vertexCount,
                                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                   MemoryType::HostVisible);
    cubeBuffer_ = std::make_unique<DrawBuffer>(std::move(indexBuffer),
                                               std::move(vertexBuffer));
    cubeBuffer_->UpdateIndices(Geometry::c_cubeIndices);
    cubeBuffer_->UpdateVertices(Geometry::c_cubeVertices);
    cubePipeline_ = std::make_unique<Pipeline>(renderingContext_, cubeShaderStages_, vertexBufferLayout,
                                               VK_FRONT_FACE_CLOCKWISE);
}

void VulkanContext::InitGltfResources() {

    // Create the shader stages, add any push constants and/or descriptor set layouts
    auto vert = std::make_unique<VulkanShader>(device_,
                                               "shaders/basic_gltf.vert.spv",
                                               VulkanShader::Vertex);
    vert->PushConstant("Model primitive", sizeof(glm::mat4) * 2); // yeah I know
    vert->AddSetLayout(descriptorSetLayouts_["ubo"]->GetDescriptorSetLayout());
    auto frag = std::make_unique<VulkanShader>(device_,
                                               "shaders/basic_gltf.frag.spv",
                                               VulkanShader::Fragment);
    for (auto& [name, model] : models_)
        frag->AddSetLayout(descriptorSetLayouts_[name]->GetDescriptorSetLayout());
    gltfShaderStages_ = std::make_unique<ShaderStages>(device_, std::move(vert),
                                                       std::move(frag));

    // Setup vertex buffer layout, and use that along with shader stages to create a pipeline
    VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.Push({0, DataType::F32, 3, "Position"});
    vertexBufferLayout.Push({1, DataType::F32, 3, "Normal"});
    vertexBufferLayout.Push({2, DataType::F32, 2, "UV"});
    vertexBufferLayout.Push({3, DataType::F32, 3, "Color"});
    gltfPipeline_ = std::make_unique<Pipeline>(renderingContext_, gltfShaderStages_, vertexBufferLayout,
                                               VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                               VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, false);

    // Create the shader stages, add any push constants and/or descriptor set layouts
    vert = std::make_unique<VulkanShader>(device_,
                                               "shaders/outline.vert.spv",
                                               VulkanShader::Vertex);
    vert->PushConstant("Model primitive", sizeof(glm::mat4) * 2);
    vert->AddSetLayout(descriptorSetLayouts_["ubo"]->GetDescriptorSetLayout());
    frag = std::make_unique<VulkanShader>(device_,"shaders/outline.frag.spv",
                                          VulkanShader::Fragment);
    for (auto& [name, model] : models_)
        frag->AddSetLayout(descriptorSetLayouts_[name]->GetDescriptorSetLayout());
    outlineShaderStages_ = std::make_unique<ShaderStages>(device_, std::move(vert),
                                                       std::move(frag));

    // Setup vertex buffer layout, and use that along with shader stages to create a pipeline
    outlinePipeline_ = std::make_unique<Pipeline>(renderingContext_, outlineShaderStages_, vertexBufferLayout,
                                               VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                               VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true);
}

void VulkanContext::SetupDescriptors() {
    // Setup model and uniform buffer before setting up descriptors
    std::set<std::string> uniqueNames = system::render::GetUniqueModelNames();
    for (auto& name : uniqueNames)
        models_[name] = std::make_unique<VulkanGLTFModel>(renderingContext_, name + ".gltf");

    // Setup the descriptor pool
    DescriptorPool::Builder builder(device_);
    uint32_t maxSets = 1; // start with ubo
    builder.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
    for (auto& [name, model] : models_) {
        uint32_t numImages = model->GetNumImages();
        maxSets += numImages;
        builder.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, numImages);
    }
    builder.SetMaxSets(maxSets);
    globalDescriptorPool_ = builder.Build();

    // Setup descriptor set layout and descriptor sets for ubo
    descriptorSetLayouts_["ubo"] = DescriptorSetLayout::Builder(device_)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        VK_SHADER_STAGE_ALL_GRAPHICS)
            .Build();
    uniformBuffer_ = std::make_unique<VulkanBuffer>(renderingContext_, sizeof(uboScene),
                                                    1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                    MemoryType::HostVisible);
    uniformBuffer_->Map();
    uniformBuffer_->WriteToBuffer(&uboScene);
    auto bufferInfo = uniformBuffer_->DescriptorInfo();
    DescriptorWriter(*descriptorSetLayouts_["ubo"], *globalDescriptorPool_)
        .WriteBuffer(0, &bufferInfo)
        .Build(uboSceneDescriptorSet_);

    // For each model setup descriptor sets for materials
    for (auto& [name, model] : models_) {
        descriptorSetLayouts_[name] = DescriptorSetLayout::Builder(device_)
                .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            VK_SHADER_STAGE_FRAGMENT_BIT)
                .Build();
        for (auto& image : model->GetImages()) {
            DescriptorWriter(*descriptorSetLayouts_[name], *globalDescriptorPool_)
                    .WriteImage(0, &image.texture.descriptor)
                    .Build(image.descriptorSet);
        }
    }
}

void VulkanContext::InitPointCloudResources() {
    // Get the point cloud files
    std::set<std::string> uniqueNames = system::render::GetUniquePointCloudNames();
    for (auto& name : uniqueNames)
        pointClouds_[name] = std::make_unique<PointCloudResource>(renderingContext_, name);

    // Setup shader stages & vertex buffer layout
    auto vert = std::make_unique<VulkanShader>(device_,
                                               "shaders/basic.vert.spv",
                                               VulkanShader::Vertex);
    vert->PushConstant("Model View Projection Matrix", sizeof(glm::mat4));
    auto frag= std::make_unique<VulkanShader>(device_,
                                              "shaders/basic.frag.spv",
                                              VulkanShader::Fragment);
    pointCloudShaderStages_ = std::make_unique<ShaderStages>(device_, std::move(vert),
                                                       std::move(frag));
    VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.Push({0, DataType::F32, 3, "Position"});
    vertexBufferLayout.Push({1, DataType::F32, 3, "Color"});

    pointCloudPipeline_ = std::make_unique<Pipeline>(renderingContext_, pointCloudShaderStages_,
                                                     vertexBufferLayout,
                                                     VK_FRONT_FACE_CLOCKWISE,
                                                     VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
}

UBOScene VulkanContext::GetUniform() const {
    return uboScene;
}

void VulkanContext::SetUniform(UBOScene scene) {
    uboScene = scene;
}

void VulkanContext::InitLightsResources() {
    auto vert = std::make_unique<VulkanShader>(device_,
                                               "shaders/lights.vert.spv",
                                               VulkanShader::Vertex);
    vert->PushConstant("Point light info", sizeof(PointLightPushConst), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    vert->AddSetLayout(descriptorSetLayouts_["ubo"]->GetDescriptorSetLayout());
    auto frag= std::make_unique<VulkanShader>(device_,
                                              "shaders/lights.frag.spv",
                                              VulkanShader::Fragment);
    lightShaderStages_ = std::make_unique<ShaderStages>(device_, std::move(vert),
                                                       std::move(frag));
    VertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.Push({0, DataType::F32, 3, "Position"});

    size_t sizeOfIndex = sizeof(Geometry::quad_indices[0]);
    size_t sizeOfVertex = sizeof(Geometry::quad_verts[0]);
    size_t indexCount = sizeof(Geometry::quad_indices) / sizeOfIndex;
    size_t vertexCount = sizeof(Geometry::quad_verts) / sizeOfVertex;
    auto indexBuffer = std::make_unique<VulkanBuffer>(renderingContext_,
                                                      sizeOfIndex, indexCount,
                                                      VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                      MemoryType::HostVisible);
    auto vertexBuffer = std::make_unique<VulkanBuffer>(renderingContext_,
                                                       sizeOfVertex, vertexCount,
                                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                       MemoryType::HostVisible);
    lightBuffer_ = std::make_unique<DrawBuffer>(std::move(indexBuffer),
                                               std::move(vertexBuffer));
    lightBuffer_->UpdateIndices(Geometry::quad_indices);
    lightBuffer_->UpdateVertices(Geometry::quad_verts);
    lightsPipeline_ = std::make_unique<Pipeline>(renderingContext_, lightShaderStages_, vertexBufferLayout,
                                                 VK_FRONT_FACE_CLOCKWISE,
                                                 VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, false);
}
}
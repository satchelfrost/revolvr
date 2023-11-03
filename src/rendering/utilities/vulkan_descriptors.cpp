/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/vulkan_descriptors.h>
#include "check.h"

namespace rvr {
DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
    assert(bindings_.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings_[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const {
    return std::make_unique<DescriptorSetLayout>(device_, bindings_);
}

DescriptorSetLayout::DescriptorSetLayout(
        VkDevice device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : device_(device), bindings_{bindings} {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv : bindings) {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(device_,&descriptorSetLayoutInfo,nullptr,
            &descriptorSetLayout_) != VK_SUCCESS) {
        THROW("failed to create descriptor set layout!");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device_, descriptorSetLayout_, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayout::GetDescriptorSetLayout() {
    return descriptorSetLayout_;
}

DescriptorPool::Builder &DescriptorPool::Builder::AddPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
    poolSizes_.push_back({descriptorType, count});
    return *this;
}

DescriptorPool::Builder &DescriptorPool::Builder::SetPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
    poolFlags_ = flags;
    return *this;
}
DescriptorPool::Builder &DescriptorPool::Builder::SetMaxSets(uint32_t count) {
    maxSets_ = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const {
    return std::make_unique<DescriptorPool>(device_, maxSets_, poolFlags_,
                                               poolSizes_);
}

DescriptorPool::DescriptorPool(
        VkDevice device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes)
        : device_{device} {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(device_, &descriptorPoolInfo, nullptr,
                               &descriptorPool_) != VK_SUCCESS) {
        THROW("failed to create descriptor pool!");
    }
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
}

bool DescriptorPool::AllocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool_;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(device_, &allocInfo, &descriptor) != VK_SUCCESS) {
        return false;
    }
    return true;
}

void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
    vkFreeDescriptorSets(
            device_,
            descriptorPool_,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
}

void DescriptorPool::ResetPool() {
    vkResetDescriptorPool(device_, descriptorPool_, 0);
}

DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool) : setLayout_(setLayout),
pool_(pool) {}

DescriptorWriter &DescriptorWriter::WriteBuffer(
        uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
    assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout_.bindings_[binding];

    assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes_.push_back(write);
    return *this;
}

DescriptorWriter &DescriptorWriter::WriteImage(
        uint32_t binding, VkDescriptorImageInfo *imageInfo) {
    assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout_.bindings_[binding];

    assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes_.push_back(write);
    return *this;
}

bool DescriptorWriter::Build(VkDescriptorSet &set) {
    bool success = pool_.AllocateDescriptor(setLayout_.GetDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }
    Overwrite(set);
    return true;
}

void DescriptorWriter::Overwrite(VkDescriptorSet &set) {
    for (auto &write : writes_) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool_.device_, writes_.size(), writes_.data(),
                           0, nullptr);
}
}
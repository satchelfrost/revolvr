#pragma once

#include <pch.h>
#include <unordered_map>

namespace rvr {
class DescriptorSetLayout {
private:
    VkDevice device_;
    VkDescriptorSetLayout descriptorSetLayout_;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

public:
    class Builder {
    public:
        Builder(VkDevice device) : device_{device} {}

        Builder &AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags,
                uint32_t count = 1);
        std::unique_ptr<DescriptorSetLayout> Build() const;

    private:
        VkDevice device_;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    DescriptorSetLayout(VkDevice device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;
    VkDescriptorSetLayout GetDescriptorSetLayout();

    friend class DescriptorWriter;
};

class DescriptorPool {
private:
    VkDevice device_;
    VkDescriptorPool descriptorPool_;
public:
    class Builder {
    private:
        VkDevice device_;
        std::vector<VkDescriptorPoolSize> poolSizes_{};
        uint32_t maxSets_ = 1000;
        VkDescriptorPoolCreateFlags poolFlags_ = 0;
    public:
        Builder(VkDevice device) : device_{device} {}

        Builder &AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder &SetPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder &SetMaxSets(uint32_t count);
        std::unique_ptr<DescriptorPool> Build() const;
    };

    DescriptorPool(VkDevice device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,
                   const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool &) = delete;
    DescriptorPool &operator=(const DescriptorPool &) = delete;

    bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;
    void FreeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
    void ResetPool();

    // TODO: This is temporary for testing only, get rid of
    VkDescriptorPool GetDescriptorPool() { return descriptorPool_; }

    friend class DescriptorWriter;
};

class DescriptorWriter {
private:
    DescriptorSetLayout& setLayout_;
    DescriptorPool& pool_;
    std::vector<VkWriteDescriptorSet> writes_;
public:
    DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool);

    DescriptorWriter &WriteBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    DescriptorWriter &WriteImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    bool Build(VkDescriptorSet &set);
    void Overwrite(VkDescriptorSet &set);
};
}
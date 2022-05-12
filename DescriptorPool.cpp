/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorPool
 */

#include <Kube/Core/Abort.hpp>
#include <Kube/Core/Assert.hpp>

#include "GPU.hpp"
#include "DescriptorPool.hpp"

using namespace kF;

GPU::DescriptorPool::~DescriptorPool(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyDescriptorPool(parent().logicalDevice(), handle(), nullptr);
}

GPU::DescriptorPool::DescriptorPool(const DescriptorPoolModel &model) noexcept
{
    if (const auto res = ::vkCreateDescriptorPool(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::DescriptorPool: Couldn't create descriptor pool '", Utils::ErrorMessage(res), '\'');
}

GPU::DescriptorSetHandle GPU::DescriptorPool::allocate(const DescriptorSetLayoutHandle &layout) noexcept
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = handle(),
        .descriptorSetCount = static_cast<std::uint32_t>(1),
        .pSetLayouts = &layout
    };

    DescriptorSetHandle descriptorSet;
    if (const auto res = ::vkAllocateDescriptorSets(parent().logicalDevice(), &allocInfo, &descriptorSet); res != VK_SUCCESS)
        kFAbort("GPU::DescriptorPool::allocate: Couldn't allocate descriptor set '", Utils::ErrorMessage(res), '\'');
    return descriptorSet;
}

void GPU::DescriptorPool::allocate(
        const DescriptorSetLayoutHandle * const layoutBegin, const DescriptorSetLayoutHandle * const layoutEnd,
        DescriptorSetHandle * const descriptorSetBegin, DescriptorSetHandle * const descriptorSetEnd) noexcept
{
    kFAssert(std::distance(layoutBegin, layoutEnd) == std::distance(descriptorSetBegin, descriptorSetEnd),
        "GPU::DescriptorPool::allocate: Distance between descriptorSetBegin & descriptorSetEnd doesn't correspond to the input layouts distance !");

    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = handle(),
        .descriptorSetCount = static_cast<std::uint32_t>(std::distance(layoutBegin, layoutEnd)),
        .pSetLayouts = layoutBegin
    };

    if (const auto res = ::vkAllocateDescriptorSets(parent().logicalDevice(), &allocInfo, descriptorSetBegin); res != VK_SUCCESS)
        kFAbort("GPU::DescriptorPool::allocate: Couldn't allocate descriptor sets '", Utils::ErrorMessage(res), '\'');
}

void GPU::DescriptorPool::deallocate(const DescriptorSetHandle * const begin, const DescriptorSetHandle * const end) noexcept
{
    ::vkFreeDescriptorSets(parent().logicalDevice(), handle(), static_cast<std::uint32_t>(std::distance(begin, end)), begin);
}

void GPU::DescriptorPool::reset(void) noexcept
{
    ::vkResetDescriptorPool(parent().logicalDevice(), handle(), VkDescriptorPoolResetFlags {});
}

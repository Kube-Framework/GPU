/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Semaphore
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Semaphore.hpp"

using namespace kF;

GPU::Semaphore::~Semaphore(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroySemaphore(parent().logicalDevice(), handle(), nullptr);
}

GPU::Semaphore::Semaphore(void) noexcept
{
    VkSemaphoreCreateInfo semaphoreInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VkSemaphoreCreateFlags()
    };

    if (const auto res = ::vkCreateSemaphore(parent().logicalDevice(), &semaphoreInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Semaphore: Couldn't create semaphore '", Utils::ErrorMessage(res), '\'');
}

GPU::TimelineSemaphore::~TimelineSemaphore(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroySemaphore(parent().logicalDevice(), handle(), nullptr);
}

GPU::TimelineSemaphore::TimelineSemaphore(const std::uint64_t initialValue) noexcept
{
    const VkSemaphoreTypeCreateInfo semaphoreType {
        /*VkStructureType    */.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        /*const void*        */.pNext = nullptr,
        /*VkSemaphoreType    */.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        /*uint64_t           */.initialValue = initialValue,
    };
    const VkSemaphoreCreateInfo semaphoreInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &semaphoreType,
        .flags = VkSemaphoreCreateFlags()
    };

    if (const auto res = ::vkCreateSemaphore(parent().logicalDevice(), &semaphoreInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::TimelineSemaphore: Couldn't create timeline semaphore '", Utils::ErrorMessage(res), '\'');
}
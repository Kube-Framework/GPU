/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: LogicalDevice
 */

#include <cstring>
#include <algorithm>

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "LogicalDevice.hpp"

using namespace kF;

GPU::LogicalDevice::~LogicalDevice(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyDevice(handle(), nullptr);
}

void GPU::LogicalDevice::waitIdle(void) noexcept
{
    ::vkDeviceWaitIdle(handle());
}

GPU::LogicalDevice::LogicalDevice(void) noexcept
{
    // Setup queues
    auto queueInfos = parent().queueManager().registerQueues();
    Core::TinySmallVector<float, 4> priorities;
    for (auto &info : queueInfos) {
        if (info.queueCount != priorities.size())
            priorities.resize(info.queueCount, 1);
    }
    for (auto &info : queueInfos) {
        info.pQueuePriorities = priorities.data();
    }
    // Device features
    const auto deviceFeatures = getDeviceFeatures();
    // Setup device
    const VkDeviceCreateInfo deviceInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &deviceFeatures->features,
        .flags = VkDeviceCreateFlags(),
        .queueCreateInfoCount = static_cast<std::uint32_t>(queueInfos.size()),
        .pQueueCreateInfos = queueInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<std::uint32_t>(deviceFeatures->extensions.size()),
        .ppEnabledExtensionNames = deviceFeatures->extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    // Create device
    if (const auto res = ::vkCreateDevice(parent().physicalDevice(), &deviceInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::LogicalDevice:: Couldn't create logical device '", ErrorMessage(res), '\'');
    // Update queue handles
    parent().queueManager().retreiveQueuesHandlers();
#if KUBE_DEBUG_BUILD
    kFInfo("Logical Extensions:");
    for (const auto &extension : deviceFeatures->extensions)
        kFInfo('\t', extension);
#endif
}

GPU::LogicalDevice::DeviceFeaturesPtr GPU::LogicalDevice::getDeviceFeatures(void) const noexcept
{
    auto deviceFeatures = DeviceFeaturesPtr::Make(DeviceFeatures {
        .extensions = getExtensions(),
        .indexingFeatures = PhysicalDeviceDescriptorIndexingFeatures {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            .pNext = nullptr
        },
        .features = PhysicalDeviceFeatures2 {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = nullptr
        }
    });
    deviceFeatures->features.pNext = &deviceFeatures->indexingFeatures;

    ::vkGetPhysicalDeviceFeatures2(parent().physicalDevice(), &deviceFeatures->features);

    kFEnsure(deviceFeatures->indexingFeatures.descriptorBindingPartiallyBound,
        "GPU::LogicalDevice: 'Partial descriptor binding bound' is not available for the selected device");

    kFEnsure(deviceFeatures->indexingFeatures.runtimeDescriptorArray,
        "GPU::LogicalDevice: 'Runtime descriptor array' is not available for the selected device");

    deviceFeatures->indexingFeatures = PhysicalDeviceDescriptorIndexingFeatures {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .pNext = nullptr,
        .descriptorBindingPartiallyBound = true,
        .runtimeDescriptorArray = true
    };

    deviceFeatures->features.features = PhysicalDeviceFeatures {
    };

    return deviceFeatures;
}

GPU::LogicalDevice::Extensions GPU::LogicalDevice::getExtensions(void) const noexcept
{
    const Extensions extensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME };
    Core::TinyVector<VkExtensionProperties> properties;

    if (const auto res = Internal::FillVkContainer(properties, &::vkEnumerateDeviceExtensionProperties, parent().physicalDevice(), nullptr); res != VK_SUCCESS)
        kFAbort("GPU::LogicalDevice::getExtensions: Couldn't enumerate logical device extensions '", ErrorMessage(res), '\'');
    for (const auto &extension : extensions) {
        auto found = false;
        for (const auto &property : properties) {
            if (found = !std::strcmp(property.extensionName, extension); found)
                break;
        }
        if (!found)
            kFAbort("GPU::Renderer::getLogicalExtensions: Extension '", extension, "' not found");
    }
    return extensions;
}
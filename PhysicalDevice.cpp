/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PhysicalDevice
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "PhysicalDevice.hpp"

using namespace kF;

GPU::PhysicalDevice::PhysicalDevice(void) noexcept
    : _properties(decltype(_properties)::Make())
{
    auto devices = getDevices();
    selectDevice(devices);

#if KUBE_DEBUG_BUILD
    kFInfo("[GPU] Devices:");
    for (auto &device : devices) {
        VkPhysicalDeviceProperties properties;
        ::vkGetPhysicalDeviceProperties(device, &properties);
        kFInfo((device == handle() ? "\t-> " : "\t"), properties.deviceName, ": invocations ", properties.limits.maxComputeWorkGroupInvocations,
            " group count (", properties.limits.maxComputeWorkGroupCount[0], ", ", properties.limits.maxComputeWorkGroupCount[1], ", ", properties.limits.maxComputeWorkGroupCount[2],
            ") group size (", properties.limits.maxComputeWorkGroupSize[0], ", ", properties.limits.maxComputeWorkGroupSize[1], ", ", properties.limits.maxComputeWorkGroupSize[2], ')');
    }
#endif
}

GPU::PhysicalDevice::Devices GPU::PhysicalDevice::getDevices(void) const noexcept
{
    Devices devices;

    if (const auto res = Internal::FillVkContainer(devices, &::vkEnumeratePhysicalDevices, parent().instance()); res != VK_SUCCESS)
        kFAbort("GPU::PhysicalDevice::PhysicalDevice: Couldn't enumerate physical devices '", ErrorMessage(res), '\'');
    return devices;
}

void GPU::PhysicalDevice::selectDevice(const Devices &devices) noexcept
{
    if (devices.empty())
        kFAbort("GPU::PhysicalDevice::selectDevice: No device detected !");
    handle() = devices[0];

    // Hard tape the AMD issue on macos
#if KUBE_PLATFORM_APPLE
    VkPhysicalDeviceProperties properties;
    ::vkGetPhysicalDeviceProperties(devices[0], &properties);
    if (std::string_view(properties.deviceName).find("AMD") != std::string_view::npos && devices.size() > 1)
        handle() = devices[1];
#endif
    ::vkGetPhysicalDeviceProperties(handle(), _properties.get());
}

GPU::Format GPU::PhysicalDevice::findSupportedFormat(const ImageTiling tiling, const FormatFeatureFlags features,
        const Format * const candidateBegin, const Format * const candidateEnd) const noexcept
{
    VkFormatProperties formatProperties {};
    const auto featureFlags = ToFlags(features);

    for (auto it = candidateBegin; it != candidateEnd; ++it) {
        ::vkGetPhysicalDeviceFormatProperties(handle(), static_cast<VkFormat>(*it), &formatProperties);
        if (tiling == ImageTiling::TilingLinear && (formatProperties.linearTilingFeatures & featureFlags) == featureFlags)
            return *it;
        else if (tiling == ImageTiling::TilingOptimal && (formatProperties.optimalTilingFeatures & featureFlags) == featureFlags)
            return *it;
    }
    kFErrorRaw("GPU::PhysicalDevice::findSupportedFormat: Couldn't find any supported format from candidate list { ");
    for (auto it = candidateBegin; it != candidateEnd; ++it)
        kFErrorRaw(FormatName(*it), ", ");
    kFAbort('}');
}

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
    kFInfo("Devices:");
    for (auto &device : devices) {
        VkPhysicalDeviceProperties properties;
        ::vkGetPhysicalDeviceProperties(device, &properties);
        if (device == handle()) [[unlikely]]
            _properties = decltype(_properties)::Make(properties);
        kFInfo((device == handle() ? "\t-> " : "\t"), properties.deviceName);
        kFInfo("\t\tmaxComputeWorkGroupInvocations: ", properties.limits.maxComputeWorkGroupInvocations);
        kFInfo("\t\tmaxComputeWorkGroupCount: ", properties.limits.maxComputeWorkGroupCount[0], ", ", properties.limits.maxComputeWorkGroupCount[1], ", ", properties.limits.maxComputeWorkGroupCount[2]);
        kFInfo("\t\tmaxComputeWorkGroupSize: ", properties.limits.maxComputeWorkGroupSize[0], ", ", properties.limits.maxComputeWorkGroupSize[1], ", ", properties.limits.maxComputeWorkGroupSize[2]);
    }
#endif
}

GPU::PhysicalDevice::Devices GPU::PhysicalDevice::getDevices(void) const noexcept
{
    Devices devices;

    if (const auto res = Utils::FillVkContainer(devices, &::vkEnumeratePhysicalDevices, parent().instance()); res != VK_SUCCESS)
        kFAbort("GPU::PhysicalDevice::PhysicalDevice: Couldn't enumerate physical devices '", Utils::ErrorMessage(res), '\'');
    return devices;
}

void GPU::PhysicalDevice::selectDevice(const Devices &devices) noexcept
{
    if (devices.empty())
        kFAbort("GPU::PhysicalDevice::selectDevice: No device detected !");
    handle() = devices[0];
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
        kFErrorRaw(Utils::FormatName(*it), ", ");
    kFAbort('}');
}

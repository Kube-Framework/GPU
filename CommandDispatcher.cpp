/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandDispatcher
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "CommandDispatcher.hpp"
#include "Fence.hpp"

using namespace kF;

GPU::CommandDispatcher::CommandDispatcher(void) noexcept
    : _cachedFrames(parent().frameCount())
{
}

void GPU::CommandDispatcher::dispatch(const QueueType queueType,
        const SubmitInfo * const submitBegin, const SubmitInfo * const submitEnd,
        const FenceHandle fence) noexcept
{
    const auto res = ::vkQueueSubmit(
        parent().queueManager().queue(queueType),
        Core::Distance<std::uint32_t>(submitBegin, submitEnd)),
        submitBegin,
        fence
    );
    if (res != VK_SUCCESS)
        kFAbort("GPU::CommandDispatcher::dispatch: Couldn't submit to queue '", ErrorMessage(res), '\'');
}

bool GPU::CommandDispatcher::tryAcquireNextFrame(void) noexcept
{
    constexpr auto UnrollClear = []<typename Type, std::size_t ...Indexes>(Type * const data, std::index_sequence<Indexes...>) {
        (... , data[Indexes].clear());
    };

    // Retreive a free semaphore for the next frame
    if (_availableSemaphores.empty()) [[unlikely]]
        _availableSemaphores.push();
    Semaphore semaphore { std::move(_availableSemaphores.back()) };
    _availableSemaphores.pop();

    // Try to retreive the next frame handle
    std::uint32_t retreivedFrame;
    const auto res = ::vkAcquireNextImageKHR(
        parent().logicalDevice(),
        parent().swapchain(),
        0, // Do not wait
        semaphore,
        NullHandle, // No fence
        &retreivedFrame
    );
    if (res != VK_SUCCESS) { // Check if the frame has been acquired or not
        _availableSemaphores.push(std::move(semaphore)); // If not, store the free semaphore for later use
        if ((res == VK_NOT_READY) | (res == VK_TIMEOUT) | (res == VK_ERROR_OUT_OF_DATE_KHR) | (res == VK_SUBOPTIMAL_KHR))
            return false;
        else
            kFAbort("GPU::CommandDispatcher::tryAcquireNextFrame: Couldn't acquire next image '", ErrorMessage(res), '\'');
    }

    // Update the new current frame
    _cachedFrames.setCurrentFrame(retreivedFrame);
    auto &cache = _cachedFrames.current();

    // Wait until the frame index finished all computes
    if (!cache.fenceCache.empty())
        Fence::Wait(cache.fenceCache.begin(), cache.fenceCache.end());

    // Set frame 'available' semaphore then clear old frame data
    auto &cachedSemaphore = cache.frameAvailable;
    if (cachedSemaphore.has_value()) [[likely]]
        _availableSemaphores.push(std::move(cachedSemaphore.value()));
    cachedSemaphore.emplace(std::move(semaphore));
    UnrollClear(cache.perQueueSemaphoreCache.data(), PerQueueIndexSequence);
    UnrollClear(cache.perQueueFenceCache.data(), PerQueueIndexSequence);
    cache.semaphoreCache.clear();
    cache.fenceCache.clear();
    parent().dispatchFrameAcquired(retreivedFrame);
    return true;
}

void GPU::CommandDispatcher::presentFrame(void) noexcept
{
    constexpr auto UnrollAccumulate = []<std::size_t ...Indexes>(const auto * const data, std::index_sequence<Indexes...>) -> std::size_t {
        return (... + data[Indexes].size());
    };

    constexpr auto UnrollMerge = []<std::size_t ...Indexes>(const auto * const data, auto &to, std::index_sequence<Indexes...>) -> void {
        (... , to.insert(to.end(), data[Indexes].begin(), data[Indexes].end()));
    };

    // Collect every semaphore in use for the current frame
    auto &cache = _cachedFrames.current();
    const auto semaphoreCount = static_cast<std::uint32_t>(UnrollAccumulate(cache.perQueueSemaphoreCache.data(), PerQueueIndexSequence));
    if (semaphoreCount) [[likely]] {
        cache.semaphoreCache.reserve(semaphoreCount);
        UnrollMerge(cache.perQueueSemaphoreCache.data(), cache.semaphoreCache, PerQueueIndexSequence);
    }

    // Send presentation command with the collected semaphore count
    const std::uint32_t currentFrame = _cachedFrames.currentFrame();
    const VkPresentInfoKHR presentInfo {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = cache.semaphoreCache.size(),
        .pWaitSemaphores = cache.semaphoreCache.begin(),
        .swapchainCount = 1,
        .pSwapchains = &parent().swapchain().handle(),
        .pImageIndices = &currentFrame,
        .pResults = nullptr,
    };
    const auto res = ::vkQueuePresentKHR(parent().queueManager().queue(QueueType::Present), &presentInfo);
    if (res != VK_SUCCESS && res != VK_ERROR_OUT_OF_DATE_KHR && res != VK_SUBOPTIMAL_KHR)
        kFAbort("GPU::CommandDispatcher::presentFrame: Couldn't setup present queue '", ErrorMessage(res), '\'');

    // Collect every fence in use for the current frame
    const auto fenceCount = static_cast<std::uint32_t>(UnrollAccumulate(cache.perQueueFenceCache.data(), PerQueueIndexSequence));
    if (fenceCount) [[likely]] {
        cache.fenceCache.reserve(fenceCount);
        UnrollMerge(cache.perQueueFenceCache.data(), cache.fenceCache, PerQueueIndexSequence);
    }
}
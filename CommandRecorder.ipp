/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandRecording
 */

#include "CommandRecorder.hpp"

inline void kF::GPU::CommandRecorder::beginRenderPass(const RenderPassHandle renderPass, const FramebufferHandle framebuffer,
        const Rect2D renderArea, const std::initializer_list<ClearValue> &clears, const SubpassContents contents) const noexcept
{
    const RenderPassBeginInfo renderPassBegin(renderPass, framebuffer, renderArea, clears.begin(), clears.end());
    ::vkCmdBeginRenderPass(_command, &renderPassBegin, static_cast<VkSubpassContents>(contents));
}

inline void kF::GPU::CommandRecorder::bindDescriptorSets(const PipelineBindPoint pipelineBindPoint, const PipelineLayoutHandle pipelineLayout,
        const std::uint32_t firstSet, const DescriptorSetHandle * const descriptorSetBegin, const DescriptorSetHandle * const descriptorSetEnd,
        const std::uint32_t * const dynamicOffsetBegin, const std::uint32_t * const dynamicOffsetEnd) const noexcept
{
    ::vkCmdBindDescriptorSets(_command, static_cast<VkPipelineBindPoint>(pipelineBindPoint), pipelineLayout,
        firstSet, static_cast<std::uint32_t>(std::distance(descriptorSetBegin, descriptorSetEnd)), descriptorSetBegin,
        static_cast<std::uint32_t>(std::distance(dynamicOffsetBegin, dynamicOffsetEnd)), dynamicOffsetBegin
    );
}

inline void kF::GPU::CommandRecorder::pipelineBarrier(const PipelineStageFlags sourceStage, const PipelineStageFlags destinationStage, const DependencyFlags dependencyFlags,
        const MemoryBarrier * const memoryBarrierBegin, const MemoryBarrier * const memoryBarrierEnd,
        const BufferMemoryBarrier * const bufferBarrierBegin, const BufferMemoryBarrier * const bufferBarrierEnd,
        const ImageMemoryBarrier * const imageBarrierBegin, const ImageMemoryBarrier * const imageBarrierEnd) const noexcept
{
    ::vkCmdPipelineBarrier(
        _command, static_cast<VkPipelineStageFlags>(sourceStage), static_cast<VkPipelineStageFlags>(destinationStage), static_cast<VkDependencyFlags>(dependencyFlags),
        static_cast<std::uint32_t>(std::distance(memoryBarrierBegin, memoryBarrierEnd)), memoryBarrierBegin,
        static_cast<std::uint32_t>(std::distance(bufferBarrierBegin, bufferBarrierEnd)), bufferBarrierBegin,
        static_cast<std::uint32_t>(std::distance(imageBarrierBegin, imageBarrierEnd)), imageBarrierBegin
    );
}
/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: RenderPassManager
 */

#include "RenderPassManager.hpp"

using namespace kF;

GPU::RenderPassManager::RenderPassManager(const std::initializer_list<RenderPassFactory> &renderPassFactories) noexcept
    : _factories(renderPassFactories.begin(), renderPassFactories.end())
{
    onViewSizeChanged();
}

void GPU::RenderPassManager::onViewSizeChanged(void) noexcept
{
    _renderPasses.resize(_factories.size(), [this](const auto index) {
        return _factories[index]();
    });
}
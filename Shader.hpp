/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Shader
 */

#pragma once

#include <Kube/Core/Vector.hpp>

#include "Handle.hpp"

namespace kF::GPU
{
    class Shader;
}

/** @brief Abstraction of a GPU shader */
class kF::GPU::Shader : public Handle<VkShaderModule>
{
public:
    /** @brief A vector containing binary data */
    using BinaryCode = Core::Utils::IteratorRange<const std::uint8_t *>;


    /** @brief Construct a shader from binary code and an optional path */
    Shader(const BinaryCode &binary, const std::string_view &path = std::string_view {}) noexcept;

    /** @brief Move constructor */
    Shader(Shader &&other) noexcept = default;

    /** @brief Destruct the shader */
    ~Shader(void) noexcept;

    /** @brief Move assignment */
    Shader &operator=(Shader &&other) noexcept = default;
};
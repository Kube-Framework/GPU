/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Matrixes
 */

#pragma once

#include <glm/glm.hpp>

namespace kF::GPU
{
    // Float vectors
    using V1f = glm::fvec1;
    using V2f = glm::fvec2;
    using V3f = glm::fvec3;
    using V4f = glm::fvec4;

    // Unsigned vectors
    using V1u = glm::uvec1;
    using V2u = glm::uvec2;
    using V3u = glm::uvec3;
    using V4u = glm::uvec4;

    // Int vectors
    using V1i = glm::ivec1;
    using V2i = glm::ivec2;
    using V3i = glm::ivec3;
    using V4i = glm::ivec4;

    // Float matrixes
    using Mat2f = glm::mat2;
    using Mat3f = glm::mat3;
    using Mat4f = glm::mat4;
    using Mat2x2f = glm::mat2x2;
    using Mat2x3f = glm::mat2x3;
    using Mat2x4f = glm::mat2x4;
    using Mat3x2f = glm::mat3x2;
    using Mat3x3f = glm::mat3x3;
    using Mat3x4f = glm::mat3x4;
    using Mat4x2f = glm::mat4x2;
    using Mat4x3f = glm::mat4x3;
    using Mat4x4f = glm::mat4x4;

    // Unsigned matrixes
    using Mat2u = glm::umat2x2;
    using Mat3u = glm::umat3x3;
    using Mat4u = glm::umat4x4;
    using Mat2x2u = glm::umat2x2;
    using Mat2x3u = glm::umat2x3;
    using Mat2x4u = glm::umat2x4;
    using Mat3x2u = glm::umat3x2;
    using Mat3x3u = glm::umat3x3;
    using Mat3x4u = glm::umat3x4;
    using Mat4x2u = glm::umat4x2;
    using Mat4x3u = glm::umat4x3;
    using Mat4x4u = glm::umat4x4;

    // Integer matrixes
    using Mat2i = glm::imat2x2;
    using Mat3i = glm::imat3x3;
    using Mat4i = glm::imat4x4;
    using Mat2x2i = glm::imat2x2;
    using Mat2x3i = glm::imat2x3;
    using Mat2x4i = glm::imat2x4;
    using Mat3x2i = glm::imat3x2;
    using Mat3x3i = glm::imat3x3;
    using Mat3x4i = glm::imat3x4;
    using Mat4x2i = glm::imat4x2;
    using Mat4x3i = glm::imat4x3;
    using Mat4x4i = glm::imat4x4;
}
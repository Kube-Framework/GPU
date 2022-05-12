#pragma once

#include <Kube/Core/Platform.hpp>

#define VMA_VULKAN_VERSION 1002000

#if KUBE_COMPILER_GCC
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wunused-parameter"
# pragma GCC diagnostic ignored "-Wconversion"
# pragma GCC diagnostic ignored "-Wtype-limits"
# pragma GCC diagnostic ignored "-Wunused-variable"
# pragma GCC diagnostic ignored "-Wmissing-field-initializers"
# include <vk_mem_alloc.h>
# pragma GCC diagnostic pop
#else
# include <vk_mem_alloc.h>
#endif
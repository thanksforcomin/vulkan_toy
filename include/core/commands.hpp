#pragma once

#include "include/vulkan/vulkan_utils.hpp"

namespace core {
    using VkCommandPools = std::array<VkCommandPool, FRAMES_IN_FLIGHT>;
    using VkCommandBuffers = std::array<VkCommandBuffer, FRAMES_IN_FLIGHT>;
}
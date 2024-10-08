#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

#include <optional>
#include <vector>
#include <cstdio>

#ifndef FRAMES_IN_FLIGHT
    #define FRAMES_IN_FLIGHT 2
#endif

namespace vulkan {
    struct queue_family_indicies {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;
        std::optional<uint32_t> compute_family;
        std::optional<uint32_t> transfer_family;
        
        bool is_complete() { return (graphics_family.has_value() && present_family.has_value()) && compute_family.has_value() && transfer_family.has_value(); }
    };

    struct device {
        VkPhysicalDevice physical;
        VkDevice logical;
    };  

    struct swap_chain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct allocated_buffer {
        VkBuffer buffer;
        VmaAllocation allocation;
        size_t size;
        VmaAllocator *allocator;
    };

    struct allocated_image {
        VkImage image;
        VkImageView view;
        VkExtent3D extent;
        VkFormat format;
        VmaAllocation allocation;
        VmaAllocator *allocator;
    };

    using allocated_buffers = std::array<allocated_buffer, FRAMES_IN_FLIGHT>;
    using allocated_images = std::array<allocated_image, FRAMES_IN_FLIGHT>;
}
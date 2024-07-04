#pragma once

#include "include/vulkan/structs.hpp"

namespace core {
  namespace graphics {
    class vulkan_context {
      private:
        GLFWwindow* m_Window = nullptr;
        VkInstance instance;
        VkSurfaceKHR surface;
        vulkan::vulkan_device device;
        vulkan::queue_family_indicies queue_indicies;
        VmaAllocator allocator;
      public:
        vulkan_context();
        ~vulkan_context();
    };

    class swapchain {
      public:
        VkExtent2D extent;
        VkFormat format;
      
      private:
        VkSwapchainKHR swapchain_object;
        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;

      public:
        swapchain() = default;
        swapchain(VkSwapchainKHR swapchain, VkExtent2D extent, VkFormat format);
        ~swapchain();
    };
  }
}
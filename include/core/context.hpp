#pragma once

#include "include/vulkan/structs.hpp"
#include <vulkan/vulkan_core.h>

/* this file is for everything related to the vulkan enviroment in which things work */

namespace core {
  class vulkan_context; //forward decl
  class swap_chain {
    public:
      VkExtent2D extent;
      VkFormat format;
    
    private:
      vulkan_context *context;

      VkSwapchainKHR swapchain_object;
      std::vector<VkImage> images;
      std::vector<VkImageView> image_views;

    public:
      swap_chain(vulkan_context *context);
      swap_chain(VkSwapchainKHR swapchain, VkExtent2D extent, VkFormat format);
      ~swap_chain();
  };
  class vulkan_context {
    private:
      const GLFWwindow* window = nullptr;
      const VkInstance instance;
      const VkSurfaceKHR surface;
      const vulkan::vulkan_device device;
      const vulkan::queue_family_indicies queue_indicies;
      swap_chain swapchain;
      const VmaAllocator allocator;
    public:
      vulkan_context(std::vector<const char*> extensions, std::vector<const char*> device_extensions);
      ~vulkan_context();
  };

  class queue {
    private:
      vulkan_context* context;

      VkQueue queue;
    public:
      queue(vulkan_context* context, uint32_t queue_family_index);
      ~queue();

  };
}
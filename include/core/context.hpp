#pragma once

#include "include/vulkan/structs.hpp"
#include <mutex>
#include <optional>
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

      VkImageView get_image_view(uint32_t index);

    private:
      VkSwapchainKHR get_swapchain(vulkan::swap_chain_support_details& support, VkSurfaceFormatKHR& surface_format);
      void create_image_views();
      void recreate(); //for resizing
  };

  class vulkan_context {
    public:
      GLFWwindow* window = nullptr;
      const VkInstance instance;
      const VkSurfaceKHR surface;
      const vulkan::device device;
      const vulkan::queue_family_indicies queue_indicies;
      const swap_chain swapchain;
      const VmaAllocator allocator;

    private:
      static std::vector<const char*> validation_layers;

      static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
                VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                VkDebugUtilsMessageSeverityFlagsEXT msg_type,
                const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                void* user_data
            );

      VkDebugUtilsMessengerEXT debug_messenger;
    public:
      vulkan_context(std::vector<const char*> extensions, std::vector<const char*> device_extensions);
      ~vulkan_context();

    private:
      VkInstance get_instance(std::vector<const char*> extensions);
      VkSurfaceKHR get_surface();
      VkDevice get_device(std::vector<const char*> device_extensions);
      VmaAllocator get_allocator();

      VkResult create_debug_messenger(const VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                      const VkAllocationCallbacks *allocator,
                                      VkDebugUtilsMessengerEXT *messenger);
      void setup_debug_messenger();
  };

  class vulkan_context_singleton {
    public:
      friend struct vulkan_context_builder;

    private:
      //all the singleton hoopla
      //we would only need the move constructors for stuff
      vulkan_context_singleton() noexcept;
      vulkan_context_singleton(vulkan_context_singleton&&) = delete;
      vulkan_context_singleton& operator=(vulkan_context_singleton&&) = delete;
      ~vulkan_context_singleton();

      //actual stuff
      const GLFWwindow* window;
      const VkInstance instance;
      const VkSurfaceKHR surface;
      const vulkan::device device;
      const vulkan::queue_family_indicies queue_indicies;
      const swap_chain swapchain;
      const VmaAllocator allocator;
      const VkDebugUtilsMessengerEXT debug_messenger; //???

      //statics
      //TODO: change to constexpr somehow
      static std::vector<const char*> validation_layers;
      static std::vector<const char*> application_extensions;
      static std::vector<const char*> device_extensions;
      static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
                VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                VkDebugUtilsMessageSeverityFlagsEXT msg_type,
                const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                void* user_data
            );

    public:
      static vulkan_context_singleton& get() {
        static vulkan_context_singleton instance_;
        return instance_;
      };
  };

  struct vulkan_context_builder {

  };
}

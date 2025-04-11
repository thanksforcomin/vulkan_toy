#include "include/core/context.hpp"
#include "include/vulkan/structs.hpp"
#include "include/vulkan/vulkan_initializers.hpp"
#include "include/vulkan/vulkan_utils.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_structs.hpp>

#include <iostream>

namespace core {
  vulkan_context::vulkan_context(std::vector<const char*> extensions, std::vector<const char*> device_extensions) :
    window(glfw::create_window(720, 720)), //i'll leave it by default so far
    instance(get_instance(extensions)),
    surface(get_surface()),
    device(
      {
        .physical = vulkan::pick_physical_device(instance, surface, device_extensions),
        .logical = get_device(device_extensions)
      }
    ),
    queue_indicies(vulkan::find_queue_family(device.physical, surface)),
    swapchain(this),
    allocator(get_allocator())
  { }

  vulkan_context::~vulkan_context() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device.logical, nullptr);
    vkDestroyInstance(instance, nullptr);
  }

  VkInstance vulkan_context::get_instance(std::vector<const char*> extensions) {
    auto inst = vulkan::create_instance("Vulkan", extensions);
    if (inst.has_value()) return inst.value();
    //error handling
    std::cout << "failed to initialize instance\n";
    std::exit(EXIT_FAILURE);
  }

  VkSurfaceKHR vulkan_context::get_surface() {
    auto surf = vulkan::create_surface(instance, window);
    if (surf.has_value()) return surf.value();
    //error handling
    std::cout << "failed to initialize surface\n";
    std::exit(EXIT_FAILURE);
  }

  VkDevice vulkan_context::get_device(std::vector<const char*> device_extensions) {
    auto dev = vulkan::create_logical_device(device.physical, surface, device_extensions);
    if (dev.has_value()) return dev.value();
    //error handling
    std::cout << "failed to initialize device\n";
    std::exit(EXIT_FAILURE);
  }

  VmaAllocator vulkan_context::get_allocator() {
    auto alloca = vulkan::create_allocator(device, instance);
    if (alloca.has_value()) return alloca.value();
    //error handling
    std::cout << "failed to initialize allocator\n";
    std::exit(EXIT_FAILURE);
  }

  VkResult vulkan_context::create_debug_messenger(const VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                                   const VkAllocationCallbacks *allocator,
                                                   VkDebugUtilsMessengerEXT *messenger)
  {
      auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
      if (func != nullptr)
          return func(instance, create_info, allocator, messenger);
      else
          return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  void vulkan_context::setup_debug_messenger() {
    VkDebugUtilsMessengerCreateInfoEXT create_info {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debug_callback,
      .pUserData = nullptr
    };

    if (create_debug_messenger(&create_info, nullptr, &debug_messenger) != VK_SUCCESS)
      throw std::runtime_error("failed to set up debug messenger\n");
  }

  VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_context::debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
        VkDebugUtilsMessageSeverityFlagsEXT msg_type,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void *user_data)
  {
      std::cerr << "validation layers: " << callback_data->pMessage << std::endl;

      return VK_FALSE;
  }
}

namespace core {
  vulkan_context_singleton::vulkan_context_singleton() :
    window(glfw::create_window(720, 720)),
    instance(*vulkan::create_instance("Vulkan", application_extensions)),
    surface(*vulkan::create_surface(instance, window)),
}

namespace core {
  swap_chain::swap_chain(vulkan_context *context) :
    context(context)
  {
    vulkan::swap_chain_support_details support = vulkan::get_swap_chain_support(context->device.physical, context->surface);
    VkSurfaceFormatKHR surface_format = vulkan::choose_format(support.formats);
    format = surface_format.format;
    extent = vulkan::choose_extent(support.capabilities, context->window);
    swapchain_object = get_swapchain(support, surface_format);
  }

  swap_chain::~swap_chain() {
    vkDestroySwapchainKHR(context->device.logical, swapchain_object, nullptr);
  }

  VkSwapchainKHR swap_chain::get_swapchain(vulkan::swap_chain_support_details& support, VkSurfaceFormatKHR& surface_format) {
    auto swp_chain = vulkan::create_swap_chain(context->device,
                                         context->surface,
                                         context->queue_indicies,
                                         support,
                                         context->window,
                                         surface_format,
                                         extent,
                                         vulkan::choose_present_mode(support.present_modes));

    if (swp_chain.has_value()) return swp_chain.value();
    //error handling
    std::cout << "failed to initialize swap chain\n";
    std::exit(EXIT_FAILURE);
  }

  VkImageView swap_chain::get_image_view(uint32_t index) {
    return image_views[index];
  }

  void swap_chain::create_image_views() {
    for(VkImage& image : images)
      image_views.push_back(vulkan::create_image_view(context->device.logical, image, format).value());
  }
}

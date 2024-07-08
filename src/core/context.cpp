#include "include/core/context.hpp"
#include "include/vulkan/structs.hpp"
#include "include/vulkan/vulkan_initializers.hpp"
#include "include/vulkan/vulkan_utils.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_structs.hpp>

namespace core {
  vulkan_context::vulkan_context(std::vector<const char*> extensions, std::vector<const char*> device_extensions) :
    window(glfw::create_window(720, 720)), //i'll leave it by default so far
    instance(vulkan::create_instance("Vulkan", extensions)),
    surface(vulkan::create_surface(instance, window)),
    device(
      {
        .physical = vulkan::create_physical_device(instance, surface, device_extensions),
        .logical = vulkan::create_logical_device(device.physical, surface, device_extensions)
      }
    ),
    queue_indicies(vulkan::find_queue_family(device.physical, surface)),

  {

  }

  vulkan_context::~vulkan_context() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device.logical, nullptr);
    vkDestroyInstance(instance, nullptr);
  }


  swap_chain::swap_chain(vulkan_context *context) :
    context(context)
  { 
    vulkan::swap_chain_support_details support = vulkan::get_swap_chain_support(context->device.physical, context->surface);
    VkSurfaceFormatKHR surface_format = vulkan::choose_format(support.formats);
    format = surface_format.format;
    extent = vulkan::choose_extent(support.capabilities, context->window);
    swapchain_object = vulkan::create_swap_chain(context->device, 
                                         context->surface, 
                                         context->queue_indicies, 
                                         support, 
                                         context->window, 
                                         surface_format, 
                                         extent, 
                                         vulkan::choose_present_mode(support.present_modes));
  }
}
#include "include/core/context.hpp"
#include "include/vulkan/vulkan_initializers.hpp"

#include <iostream>
#include <expected>

int main() {
  std::vector<const char*> instance_extensions{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
  std::vector<const char*> device_extensions{VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, 
                                              VK_KHR_MAINTENANCE2_EXTENSION_NAME,
                                              VK_KHR_MULTIVIEW_EXTENSION_NAME,
                                              VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                                              VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
                                              VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                              VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME};

  core::vulkan_context context(instance_extensions, device_extensions);

  return 0;
}
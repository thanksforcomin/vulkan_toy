#include "include/core/context.hpp"
#include "include/vulkan/vulkan_initializers.hpp"

namespace core {
  vulkan_context::vulkan_context(std::vector<const char*> extensions, std::vector<const char*> device_extensions) :
    window(glfw::create_window(720, 720)), //i'll leave it by default so far
    instance(vulkan::create_instance("Vulkan", extensions)),
}
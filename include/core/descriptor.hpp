#pragma once

#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "include/core/context.hpp"

namespace core {
  class descriptor_allocator {
    private:
      vulkan_context *context;

      static const std::unordered_map<VkDescriptorType, float> pool_sizes;

      VkDescriptorPool curr_pool{VK_NULL_HANDLE};

    public:
      descriptor_allocator(vulkan_context *context);
      ~descriptor_allocator() = default;

      VkDescriptorPool create_descriptor_pool(uint32_t count);
      VkDescriptorSet allocate_descriptor_set(VkDescriptorSetLayout &layout);   

      VkDescriptorPool get_pool();
  };

  struct descriptor_builder {

  };
}
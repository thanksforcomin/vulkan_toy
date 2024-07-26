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
    private:
      std::vector<VkWriteDescriptorSet> writes;
      std::vector<VkDescriptorSetLayoutBinding> bindings;

    public:
      descriptor_builder() = default;
      ~descriptor_builder() = default;

      descriptor_builder& add_buffer(uint32_t binding, 
                                     VkDescriptorBufferInfo *buffer_info, 
                                     VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
                                     VkShaderStageFlags flags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
      descriptor_builder& add_image(uint32_t binding, 
                                    VkDescriptorImageInfo *image_info, 
                                    VkDescriptorType type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 
                                    VkShaderStageFlags flags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

      VkDescriptorSet build();
  };
}
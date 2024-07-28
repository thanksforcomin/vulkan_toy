#include "include/core/descriptor.hpp"
#include "include/core/context.hpp"
#include "include/vulkan/vulkan_initializers.hpp"
#include "include/vulkan/vulkan_utils.hpp"
#include <chrono>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace core {
  descriptor_builder::descriptor_builder(descriptor_allocator *desc_allocator) : allocator(desc_allocator) { }
  
  descriptor_builder& descriptor_builder::add_buffer(uint32_t binding, VkDescriptorBufferInfo *buffer_info, VkDescriptorType type, VkShaderStageFlags flags) {
    bindings.push_back(vulkan::get_descriptor_set_layout_binding(type, flags, binding));
    writes.push_back(vulkan::get_descriptor_write_info(type, VK_NULL_HANDLE, binding, buffer_info, nullptr));
    return *this;
  }

  descriptor_builder& descriptor_builder::add_image(uint32_t binding, VkDescriptorImageInfo *image_info, VkDescriptorType type, VkShaderStageFlags flags) {
    bindings.push_back(vulkan::get_descriptor_set_layout_binding(type, flags, binding));
    writes.push_back(vulkan::get_descriptor_write_info(type, VK_NULL_HANDLE, binding, nullptr, image_info));
    return *this;
  }

  VkDescriptorSet descriptor_builder::build() {
    auto layout = vulkan::create_descriptor_set_layout(allocator->context->device.logical, bindings);
    if (!layout.has_value()) return VK_NULL_HANDLE;

    auto dst_set = vulkan::allocate_descriptor_set(allocator->context->device.logical, alloca, layout.value());
    if (!dst_set.has_value()) return VK_NULL_HANDLE;
    
    for (auto& write : writes) write.dstSet = dst_set.value();

    vkUpdateDescriptorSets(allocator->context->device.logical, writes.size(), writes.data(), 0, nullptr);

    return dst_set.value();
  }
}

namespace core {
  const std::unordered_map<VkDescriptorType, float> core::descriptor_allocator::pool_sizes {
          { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
          { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
          { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
          { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
          { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
          { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
          { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
          { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
          { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
          { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
          { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
  };

  descriptor_allocator::descriptor_allocator(vulkan_context* vulkan_context) : context(vulkan_context) { }

  VkDescriptorPool descriptor_allocator::create_descriptor_pool(uint32_t count) {
    std::vector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.reserve(pool_sizes.size());

    for (const auto& [type, size] : pool_sizes) {
      pool_sizes.push_back({type, count * size});
    }

    auto pool = vulkan::create_descriptor_pool(context->device.logical, pool_sizes, count);
    if (pool.has_value())
      return pool.value();
    else {
      if (pool.error() == VK_ERROR_FRAGMENTATION_EXT)
        return create_descriptor_pool(count);
      return VK_NULL_HANDLE;
    }
  }

  VkDescriptorSet descriptor_allocator::allocate_descriptor_set(VkDescriptorSetLayout &layout) {
    if (curr_pool == VK_NULL_HANDLE)
      curr_pool = create_descriptor_pool(1000); //default pool size

    auto descriptor_set = vulkan::allocate_descriptor_set(context->device.logical, curr_pool, layout);
    if (descriptor_set.has_value())
      return descriptor_set.value();
    else {
      if (descriptor_set.error() == VK_ERROR_FRAGMENTATION_EXT)
        curr_pool = create_descriptor_pool(1000);
      return allocate_descriptor_set(layout);
    }
  }
}
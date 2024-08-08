#include "include/core/commands.hpp"
#include "include/vulkan/vulkan_initializers.hpp"
#include "include/vulkan/vulkan_utils.hpp"

#include <iostream>

namespace core {
  command_dispatcher::command_dispatcher(vulkan_context *context) : context(context) {
    auto cmd_pool = vulkan::create_command_pool(context->device.logical, 0, context->queue_indicies.graphics_family.value());

    if (cmd_pool.has_value()) {
      command_pool = cmd_pool.value();
    } else { 
      std::cout << "failed to create command pool\n";
      std::exit(EXIT_FAILURE);
    }
  }

  command_dispatcher::~command_dispatcher() {
    vkFreeCommandBuffers(context->device.logical, command_pool, command_buffers.size(), command_buffers.data());
    vkDestroyCommandPool(context->device.logical, command_pool, nullptr);
  }


  //TODO: deleting data from vectors is kinda inefficient, especially every frame. make it more efficient
  VkCommandBuffer command_dispatcher::get_command_buffer() {
    if (!command_buffers.empty()) {
      VkCommandBuffer cmd_buf = command_buffers.back();
      command_buffers.pop_back();
      return cmd_buf;
    } else {
      return create_command_buffer();
    }
  }

  void command_dispatcher::execute_commands(const VkQueue &queue, VkSubmitInfo *submit_info, VkFence &fence, bool reuse) {
    VkResult res = vkQueueSubmit(queue, 1, submit_info, fence);
    if (res != VK_SUCCESS) {
      std::cout << "failed to submit draw command buffer\n";
      std::exit(EXIT_FAILURE);
    }
    if (reuse) command_buffers.insert(command_buffers.begin(), submit_info->pCommandBuffers, submit_info->pCommandBuffers + submit_info->commandBufferCount * sizeof(VkCommandBuffer));
  }

  VkCommandBuffer command_dispatcher::create_command_buffer() {
    auto cmd_buf = vulkan::allocate_command_buffer(context->device.logical, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    if (cmd_buf.has_value()) 
      return cmd_buf.value();
    else {
      std::cout << "failed to create command buffer\n";
      std::exit(EXIT_FAILURE);
    }
  }
}
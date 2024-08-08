#include "include/core/commands.hpp"
#include "include/vulkan/vulkan_initializers.hpp"
#include "include/vulkan/vulkan_utils.hpp"

#include <iostream>
#include <span>

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
    for (auto& it : command_buffers) {
      vkFreeCommandBuffers(context->device.logical, command_pool, 1, &it.first);
    }
    vkDestroyCommandPool(context->device.logical, command_pool, nullptr);
  }


  //TODO: deleting data from vectors is kinda inefficient, especially every frame. make it more efficient
  VkCommandBuffer command_dispatcher::get_command_buffer() {
    //if no available command buffers, create a new one
    if (command_buffers.empty()) {
      return command_buffers.insert({create_command_buffer(), false}).first->first;
    } else {
      for (auto& it : command_buffers) {
        if (it.second) {
          it.second = false;
          return it.first;
        }
      }
      //if we haven't found an unused command buffer, create a new one
      return command_buffers.insert({create_command_buffer(), false}).first->first;
    }
  }

  void command_dispatcher::execute_commands(const VkQueue &queue, VkSubmitInfo *submit_info, VkFence &fence, bool reuse) {
    VkResult res = vkQueueSubmit(queue, 1, submit_info, fence);
    if (res != VK_SUCCESS) {
      std::cout << "failed to submit draw command buffer\n";
      std::exit(EXIT_FAILURE);
    }
    if (reuse) {
      for (const VkCommandBuffer& it : std::span<const VkCommandBuffer>(submit_info->pCommandBuffers, submit_info->commandBufferCount)) {
        //i am not sure about this
        VkCommandBuffer bfr = it;
        command_buffers[bfr] = true;
      }
    }
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
#pragma once

#include "include/core/context.hpp"

namespace core {
  using VkCommandPools = std::array<VkCommandPool, FRAMES_IN_FLIGHT>;
  using VkCommandBuffers = std::array<VkCommandBuffer, FRAMES_IN_FLIGHT>;

  class command_dispatcher {
  private:
    vulkan_context *context;
    std::vector<VkCommandBuffer> command_buffers;
    VkCommandPool command_pool;

  public:
    command_dispatcher(vulkan_context *context);
    ~command_dispatcher();

    VkCommandBuffer get_command_buffer();
    void execute_commands(const VkQueue &queue, VkSubmitInfo *submit_info, VkFence &fence, bool reuse = false);
  private:
    VkCommandBuffer create_command_buffer();
  };
}

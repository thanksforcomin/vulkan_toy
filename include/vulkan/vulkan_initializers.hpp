#pragma once

#include "include/utils/expected.hpp"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "include/vulkan/structs.hpp"

#include <expected>
#include <vector>
#include <string>

namespace glfw {
    GLFWwindow* create_window(uint32_t height, uint32_t width);
}

namespace vulkan {
    using VkRes = VkResult;

    std::expected<VkSurfaceKHR, VkRes> create_surface(const VkInstance &inst, GLFWwindow *window);

    std::expected<VkInstance, VkRes> create_instance(std::string application_name, std::vector<const char*> extensions = {});

    //todo: transfer to vulkan utils
    VkPhysicalDevice pick_physical_device(const VkInstance &inst, const VkSurfaceKHR &surface, std::vector<const char*>& device_extensions);

    std::expected<VkDevice, VkRes> create_logical_device(const VkPhysicalDevice &dev, const VkSurfaceKHR &surface, std::vector<const char*>& device_extensions);
    
    //todo: transfer to vulkan utils
    VkQueue get_queue(VkDevice &dev, uint32_t queue_family_index);
    
    std::expected<VmaAllocator, VkRes> create_allocator(const device& device, const VkInstance& instance);
    
    std::expected<VkSwapchainKHR, VkRes> create_swap_chain(const device &vulkan_dev, const VkSurfaceKHR &surface, queue_family_indicies indicies, swap_chain_support_details support_details, 
                                     const GLFWwindow *window, VkSurfaceFormatKHR format, VkExtent2D extent, VkPresentModeKHR present_mode);
                                    
    std::expected<VkImage, VkRes> create_image(VkDevice &dev, VkFormat format, VkImageUsageFlags usage, VkExtent3D extent);

    std::expected<VkImageView, VkRes> create_image_view(const VkDevice &dev, VkImage &image, VkFormat &format);

    std::expected<VkCommandPool, VkRes> create_command_pool(const VkDevice &dev, VkCommandPoolCreateFlags flags, uint32_t queue_family_index);

    std::expected<VkCommandBuffer, VkRes> allocate_command_buffer(const VkDevice &dev, VkCommandPool &command_pool, VkCommandBufferLevel level, uint32_t count = 1);

    std::expected<VkDescriptorSetLayout, VkRes> create_descriptor_set_layout(const VkDevice &dev, std::vector<VkDescriptorSetLayoutBinding> &bindings);

    std::expected<VkDescriptorSet, VkRes> allocate_descriptor_set(const VkDevice &dev, const VkDescriptorPool &pool, VkDescriptorSetLayout &layout);

    std::expected<VkDescriptorPool, VkRes> create_descriptor_pool(const VkDevice &dev, std::vector<VkDescriptorPoolSize> pool_sizes, uint32_t max_sets);

    std::expected<VkFence, VkRes> create_fence(VkDevice &dev, VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);

    std::expected<VkSemaphore, VkRes> create_semaphore(VkDevice &dev, VkSemaphoreCreateFlags flags = 0);

    //TODO: delete this
    std::expected<VkRenderPass, VkRes> create_render_pass(VkDevice &dev, std::vector<VkAttachmentDescription> attachments, 
                                    std::vector<VkSubpassDescription> subpasses, std::vector<VkSubpassDependency> dependencies = {});

    std::expected<VkPipelineLayout, VkRes> create_pipeline_layout(VkDevice &dev, std::vector<VkDescriptorSetLayout> set_layouts, std::vector<VkPushConstantRange> push_constants = {});

    std::expected<VkPipeline, VkRes> create_compute_pipeline(VkDevice &dev, VkPipelineLayout &pipeline_layout, VkPipelineShaderStageCreateInfo &compute_shader);

    std::expected<VkShaderModule, VkRes> create_chader_module(VkDevice &dev, std::vector<char>&& data);
}

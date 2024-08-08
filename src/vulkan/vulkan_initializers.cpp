#include "include/vulkan/vulkan_initializers.hpp"
#include "include/vulkan/structs.hpp"
#include "include/vulkan/vulkan_utils.hpp"
#include "include/vulkan/vulkan_create_infos.hpp"
#include "include/utils/expected.hpp"

#include <iostream>
#include <initializer_list>
#include <set>
#include <vulkan/vulkan_core.h>

namespace glfw {
    GLFWwindow* create_window(uint32_t height, uint32_t width) {
        if(!glfwInit()) throw std::runtime_error("funky!\n GLFW is not inited");

        if(!glfwVulkanSupported()) throw std::runtime_error("funky!\n vulkan is not supported");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        return glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
    }
}

namespace vulkan {
    /*
     * Creates a Vulkan surface for a GLFW window.
     */
   std::expected<VkSurfaceKHR, VkRes> create_surface(const VkInstance &inst, GLFWwindow *window) {
        VkSurfaceKHR surface;
        VkResult res = glfwCreateWindowSurface(inst, window, nullptr, &surface);
        if (res != VK_SUCCESS) return std::unexpected(res);
        return surface;
    }

    /*
     * Creates a Vulkan instance with the specified application name.
     */
    std::expected<VkInstance, VkRes> create_instance(std::string applicaion_name, std::vector<const char*> extensions) {
        std::vector<const char *> required_extensions = require_extensions();
        required_extensions.insert(required_extensions.end(), extensions.begin(), extensions.end());
        std::vector<const char *> validation_layers{"VK_LAYER_KHRONOS_validation"};

        if(!validation_layers_support(validation_layers))
            throw std::runtime_error("validation layers are not supported");

        VkApplicationInfo appInfo{}; // general info
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicaion_name.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "minivan";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        std::cout << "minivan" << " "
                << " 1, 0, 0 \n";

        VkInstanceCreateInfo createInfo = vulkan::instance_create_info(&appInfo, validation_layers, required_extensions);
        VkInstance inst;
        
        VkResult res = vkCreateInstance(&createInfo, nullptr, &inst);
        if(res != VK_SUCCESS) 
            return std::unexpected(res);

        return inst;
    };

    /*
     * Creates a physical device for Vulkan rendering.
     */
    VkPhysicalDevice pick_physical_device(const VkInstance &inst, const VkSurfaceKHR& surface, std::vector<const char*>& device_extensions) {
        unsigned int device_count = 0;
        vkEnumeratePhysicalDevices(inst, &device_count, nullptr);

        if (device_count == 0)
            throw std::runtime_error("no available devices, closing");

        std::vector<VkPhysicalDevice> available_devices(device_count);
        vkEnumeratePhysicalDevices(inst, &device_count, available_devices.data());

        for (auto &dev : available_devices)
        {
            if (is_device_valid(dev, surface, device_extensions))
            {
                VkPhysicalDeviceProperties device_properties;
                vkGetPhysicalDeviceProperties(dev, &device_properties);
                std::cout << device_properties.deviceName << " was chosen as the working device\n";
                return dev;
            }
        }

        throw std::runtime_error("no device is good enough.");
        return VK_NULL_HANDLE;
    };
        
    /**
    * Creates a logical device for Vulkan graphics rendering.
    */
    std::expected<VkDevice, VkRes> create_logical_device(const VkPhysicalDevice &dev, const VkSurfaceKHR &surface, std::vector<const char*>& device_extensions) {
        queue_family_indicies queue_family = find_queue_family(dev, surface);
        std::set<uint32_t> unique_family_queues = {queue_family.graphics_family.value(), queue_family.present_family.value()};

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        VkPhysicalDeviceFeatures device_fetures = {};

        float queue_priority = 1.0f;

        for (uint32_t queue_family : unique_family_queues)
        {
            VkDeviceQueueCreateInfo create_info(queue_create_info(queue_family, queue_priority));
            queue_create_infos.push_back(create_info);
        }

        VkDeviceCreateInfo create_info = vulkan::logical_device_create_info(&device_fetures, queue_create_infos, device_extensions); // logical device create info
        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = vulkan::get_dynamic_rendering_features(); //is's two integers so not that bad
        create_info.pNext = &dynamic_rendering_features;
        
        VkDevice log_dev;
        VkResult res = vkCreateDevice(dev, &create_info, nullptr, &log_dev);
        if (res != VK_SUCCESS)
            return std::unexpected(res);

        return log_dev;
    }

    VkQueue get_queue(VkDevice &dev, uint32_t queue_family_index) {
        VkQueue queue;
        vkGetDeviceQueue(dev, queue_family_index, 0, &queue);
        return queue;
    }

    std::expected<VmaAllocator, VkRes> create_allocator(const device& device, const VkInstance& instance) {
        VmaAllocator allocator;
        VmaAllocatorCreateInfo allocator_info = allocator_create_info(instance, device);
        VkResult res = vmaCreateAllocator(&allocator_info, &allocator);
        if(res != VK_SUCCESS)
            return std::unexpected(res);
        return allocator;
    }

    std::expected<VkSwapchainKHR, VkRes> create_swap_chain(const device &vulkan_dev, const VkSurfaceKHR &surface, queue_family_indicies indicies, swap_chain_support_details support_details, 
                                     const GLFWwindow *window, VkSurfaceFormatKHR format, VkExtent2D extent, VkPresentModeKHR present_mode) {

        VkSwapchainCreateInfoKHR create_info{ 
            swap_chain_create_info(window,
                                   2, 
                                   support_details, 
                                   format,
                                   extent,
                                   present_mode,
                                   surface,
                                   {indicies.graphics_family.value(), indicies.present_family.value()}, 
                                   VK_NULL_HANDLE) 
        };

        VkSwapchainKHR swap_chain;
        VkResult res = vkCreateSwapchainKHR(vulkan_dev.logical, &create_info, nullptr, &swap_chain);
        if (res != VK_SUCCESS)
            return std::unexpected(res);
        return swap_chain;
    }

    std::expected<VkImage, VkRes> create_image(VkDevice &dev, VkFormat format, VkImageUsageFlags usage, VkExtent3D extent) {
        VkImage img;
        VkImageCreateInfo create_info{image_create_info(format, usage, extent)};
        VkResult res = vkCreateImage(dev, &create_info, nullptr, &img);
        if (res != VK_SUCCESS)
            return std::unexpected(res);
        return img;
    }

    std::expected<VkImageView, VkRes> create_image_view(const VkDevice &dev, VkImage &image, VkFormat &format)
    {
        VkImageView image_view;
        VkImageViewCreateInfo create_info(image_view_create_info(image, format));
        VkRes res = vkCreateImageView(dev, &create_info, nullptr, &image_view);
        if (res != VK_SUCCESS) return std::unexpected(res);
        return image_view;
    }

    std::expected<VkCommandPool, VkRes> create_command_pool(const VkDevice &dev, VkCommandPoolCreateFlags flags, uint32_t queue_family_index) {
        VkCommandPoolCreateInfo create_info{command_pool_create_info(queue_family_index, flags)};
        VkCommandPool command_pool;
        VkRes res = vkCreateCommandPool(dev, &create_info, nullptr, &command_pool);
        if (res != VK_SUCCESS) return std::unexpected(res);
        return command_pool;
    }

    std::expected<VkCommandBuffer, VkRes> allocate_command_buffer(const VkDevice &dev, VkCommandPool &command_pool, VkCommandBufferLevel level, uint32_t count) {
        VkCommandBufferAllocateInfo create_info{command_buffer_allocate_info(command_pool, level, count)};
        VkCommandBuffer command_buffer;
        if (VkRes res = vkAllocateCommandBuffers(dev, &create_info, &command_buffer); res != VK_SUCCESS)
            return std::unexpected(res);
        return command_buffer;
    }

     std::expected<VkDescriptorSetLayout, VkRes> create_descriptor_set_layout(const VkDevice &dev, std::vector<VkDescriptorSetLayoutBinding> &bindings) {
        VkDescriptorSetLayoutCreateInfo create_info{descriptor_set_layout_create_info(bindings.data(), bindings.size())};
        VkDescriptorSetLayout layout;
        if (VkRes res = vkCreateDescriptorSetLayout(dev, &create_info, nullptr, &layout); res != VK_SUCCESS)
            return std::unexpected(res);
        return layout;
    }

    std::expected<VkDescriptorSet, VkResult> allocate_descriptor_set(const VkDevice &dev, const VkDescriptorPool &pool, VkDescriptorSetLayout &layout) {
        VkDescriptorSetAllocateInfo create_info{descriptor_set_allocate_info(pool, &layout)};
        VkDescriptorSet descriptor_set;
        VkResult res = vkAllocateDescriptorSets(dev, &create_info, &descriptor_set);
        if (res != VK_SUCCESS)
            return std::unexpected<VkResult>(res);
        return descriptor_set;
    }

    std::expected<VkDescriptorPool, VkRes> create_descriptor_pool(const VkDevice &dev, std::vector<VkDescriptorPoolSize> pool_sizes, uint32_t max_sets) {
        VkDescriptorPoolCreateInfo create_info{descriptor_pool_create_info(&pool_sizes[0], pool_sizes.size(), max_sets)};
        VkDescriptorPool descriptor_pool;
        if (VkRes res = vkCreateDescriptorPool(dev, &create_info, nullptr, &descriptor_pool); res != VK_SUCCESS)
            return std::unexpected(res);
        return descriptor_pool;
    }

    std::expected<VkFence, VkRes> create_fence(VkDevice &dev, VkFenceCreateFlags flags) {
        VkFenceCreateInfo create_info{fence_create_info(flags)};
        VkFence fence;
        if (VkRes res = vkCreateFence(dev, &create_info, nullptr, &fence); res != VK_SUCCESS)
            return std::unexpected(res);
        return fence;
    }

    std::expected<VkSemaphore, VkRes> create_semaphore(VkDevice &dev, VkSemaphoreCreateFlags flags) {
        VkSemaphoreCreateInfo create_info{semaphore_create_info(flags)};
        VkSemaphore semaphore;
        if (VkRes res = vkCreateSemaphore(dev, &create_info, nullptr, &semaphore); res != VK_SUCCESS)
            return std::unexpected(res);
        return semaphore;
    };

    std::expected<VkRenderPass, VkRes> create_render_pass(VkDevice &dev, std::vector<VkAttachmentDescription> attachments, 
                                    std::vector<VkSubpassDescription> subpasses, std::vector<VkSubpassDependency> dependencies) 
    {
        VkRenderPassCreateInfo create_info{render_pass_create_info(attachments.data(), attachments.size(), 
                                                                   subpasses.data(), subpasses.size(), 
                                                                   dependencies.data(), dependencies.size())};
        VkRenderPass render_pass;
        if (VkRes res = vkCreateRenderPass(dev, &create_info, nullptr, &render_pass); res != VK_SUCCESS)
            return std::unexpected(res);
        return render_pass;
    }

   std::expected<VkPipelineLayout, VkRes> create_pipeline_layout(VkDevice &dev, std::vector<VkDescriptorSetLayout> set_layouts, std::vector<VkPushConstantRange> push_constants) {
        VkPipelineLayoutCreateInfo create_info{pipeline_layout_create_info(set_layouts.data(), set_layouts.size(), push_constants.data(), push_constants.size())};
        VkPipelineLayout pipeline_layout;
        if(VkRes res = vkCreatePipelineLayout(dev, &create_info, nullptr, &pipeline_layout); res != VK_SUCCESS) 
            return std::unexpected(res);
        return pipeline_layout;
    }

    std::expected<VkPipeline, VkRes> create_compute_pipeline(VkDevice &dev, VkPipelineLayout &pipeline_layout, VkPipelineShaderStageCreateInfo &compute_shader) {
        VkComputePipelineCreateInfo create_info{compute_pipeline_create_info(pipeline_layout, compute_shader)};
        VkPipeline pipeline;
        if (VkRes res = vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline); res != VK_SUCCESS)
            return std::unexpected(res);
        return pipeline;
    }

    std::expected<VkShaderModule, VkRes> create_chader_module(VkDevice &dev, std::vector<char>&& data) {
        VkShaderModuleCreateInfo create_info{shader_module_create_info(data.size(), &data[0])};
        VkShaderModule shader_module;
        if (VkRes res = vkCreateShaderModule(dev, &create_info, nullptr, &shader_module); res != VK_SUCCESS)
            return std::unexpected(res);
        return shader_module;
    }
}
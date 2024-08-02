#pragma once

#include <vector>
#include <string>
#include <functional>

#include "include/core/context.hpp"

namespace rendergraph
{
  struct node
  {
    std::string name;
    std::vector<node*> children;

    std::function<void (VkCommandBuffer& cmd_buffer)> process;

    void add_input_texture(const std::string& name);
    void add_output_texture(const std::string& name);
  };

  class framegraph {
    //TODO: make it make more sense
    std::vector<node> nodes;

    public:
      framegraph(core::vulkan_context* context);
      ~framegraph() = default;

      node* add_node(std::vector<node*>&& parents);

      void bake();
      void execute();
  };
}
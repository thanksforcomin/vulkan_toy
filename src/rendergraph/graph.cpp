#include "include/rendergraph/graph.hpp"

#include <queue>

namespace rendergraph {
  node* framegraph::add_node(std::vector<node*>&& parents) {
    nodes.push_back(node{});
    node* new_node = &nodes.back();

    for (auto& parent : parents) {
      parent->children.push_back(new_node);
    }

    return new_node;
  }

  void framegraph::bake() { 
      //simple bfs
    std::queue<node*> queue;
    queue.push(&nodes[0]);

    while(!queue.empty()) {
      node* node = queue.front();
      queue.pop();
      for (auto& child : node->children) {
        queue.push(child);
      }
    }
  }
}

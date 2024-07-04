#pragma once

#include <vector>
#include <string>
#include <functional>

namespace rendergraph
{
  struct node
  {
    std::string name;
    std::vector<node*> inputs;
    std::vector<node*> outputs;

    std::function<void ()> process;
  };
}
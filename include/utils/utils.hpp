#pragma once

#include <functional>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace utils {
  std::vector<char> read_from_file(std::string filepath);

  template <class T>
  inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  };


  template<typename... Ty>
  struct type_list {};

  template<std::size_t N, typename... Ty>
  struct get_element {
    using type = void;
  };

  template<std::size_t N, typename T, typename... Ty>
  struct get_element<N, T, Ty...> : get_element<N - 1, Ty...> { };

  template<typename T, typename... Ty>
  struct get_element<0, T, Ty...> {
    using type = T;
  };

  template<std::size_t N, typename List>
  struct pos;

  template<std::size_t N, typename... Ty>
  struct pos<N, type_list<Ty...>> : get_element<N, Ty...> { };

  template<typename T>
  struct function_traits { };

  template<typename Ret, typename... Args>
  struct function_traits<Ret(Args...)> {
    using args_types = type_list<Args...>;
  };

  template <typename Ret, typename... Args>
  struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

  template<auto T>
  using args_of = typename function_traits<decltype(T)>::args_types;

  template<auto T>
  consteval auto reflect() -> args_of<T> {
    return {};
  }
}

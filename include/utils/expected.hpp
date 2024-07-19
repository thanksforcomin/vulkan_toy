#pragma once

#include <exception>
#include <expected>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <utility>

namespace utils {
  template<typename T, typename E>
  class expection : public std::expected<T, E> {
  public:
    using std::expected<T, E>::expected;

    expection& on_error(std::function<void(E)> fn) {
      if (this->has_value())
        return *this;
      fn(this->error());
      return *this;
    }

    expection& on_error_emplace(std::function<T(E)> fn) {
      if (this->has_value())
        return *this;
      this->emplace(fn(this->error()));
      return *this;
    }
  };
}
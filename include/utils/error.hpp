#pragma once

template<typename T>
struct FUNCTION_RESULT {
  T result;
  unsigned status;
};

#define call(typename x) FUNCTION_RESULT<decltype(x)>
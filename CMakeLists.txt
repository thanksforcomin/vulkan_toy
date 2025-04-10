cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(renderer
  LANGUAGES CXX
  VERSION 0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  src/*.cpp
  src/*.cxx
  src/*.cc
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
  include/*.hpp
  include/*.h
)

add_executable(test ${SRC_FILES} ${HEADER_FILES})

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
  message(STATUS "Found Vulkan: ${Vulkan_VERSION}")
  target_link_libraries(test PRIVATE Vulkan::Vulkan)
else()
  message(FATAL_ERROR "Vulkan not found - required for this project")
endif()

option(GLFW_BUILD_DOCS "Build GLFW documentation" OFF)
option(GLFW_BUILD_TESTS "Build GLFW tests" OFF)
option(GLFW_BUILD_EXAMPLES "Build GLFW examples" OFF)

find_package(glfw3 3.3 QUIET)  # Specify minimum version
if(glfw3_FOUND)
  message(STATUS "Found system GLFW: ${glfw3_VERSION}")
  target_link_libraries(test PRIVATE glfw)
else()
  message(STATUS "Using bundled GLFW")
  add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glfw")
  target_link_libraries(test PRIVATE glfw)
endif()

find_package(glm REQUIRED)
target_link_libraries(test PRIVATE glm::glm)

find_package(VulkanMemoryAllocator CONFIG QUIET)
if(VulkanMemoryAllocator_FOUND)
  target_link_libraries(test PRIVATE GPUOpen::VulkanMemoryAllocator)
else()
  message(STATUS "Using bundled VulkanMemoryAllocator")
  add_subdirectory("${CMAKE_SOURCE_DIR}/libs/VulkanMemoryAllocator")
  target_link_libraries(test PRIVATE GPUOpen::VulkanMemoryAllocator)
endif()

install(TARGETS test DESTINATION bin)

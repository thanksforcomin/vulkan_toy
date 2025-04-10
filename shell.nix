let
  nixpkgs = fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-24.05";
  pkgs = import nixpkgs { config = {}; overlays = []; };
in

pkgs.mkShellNoCC {
  packages = with pkgs; [
    vulkan-tools
    vulkan-headers
    vulkan-validation-layers
    vulkan-loader
    vulkan-utility-libraries

    glfw3
    glm
    libGL
    libGLU
    glfw
    glew
    freeglut
    mesa
    xorg.libX11
    xorg.libXext
    xorg.libXrandr
    xorg.libXcursor
    xorg.libXi

    cmake
    pkg-config
  ];

  # Help CMake find Vulkan
  Vulkan_LIBRARY = "${pkgs.vulkan-loader}/lib/libvulkan.so";
  Vulkan_INCLUDE_DIR = "${pkgs.vulkan-headers}/include";

}

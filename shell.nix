let
  nixpkgs = fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-24.05";
  pkgs = import nixpkgs { config = {}; overlays = []; };
in

pkgs.mkShell {
  buildInputs = with pkgs; [
    glfw
    libGL
    libglvnd
    mesa
  ];

  packages = with pkgs; [
    vulkan-tools
    vulkan-headers
    vulkan-validation-layers
    vulkan-loader
    vulkan-utility-libraries

    #glfw stuff
    glfw3
    glfw

    #glm stuff for math
    glm

    #opengl stuff
    libGL
    libGLU
    libglvnd
    glew
    freeglut
    mesa
    xorg.libX11
    xorg.libXext
    xorg.libXrandr
    xorg.libXcursor
    xorg.libXi

    #cmake stuff
    cmake
    pkg-config

    #meson??? I am considering moving to
    #meson
  ];

  # Help CMake find Vulkan
  Vulkan_LIBRARY = "${pkgs.vulkan-loader}/lib/libvulkan.so";
  Vulkan_INCLUDE_DIR = "${pkgs.vulkan-headers}/include";

}

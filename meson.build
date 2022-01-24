project('fractals', 'c', 'cpp')

executable(
  'main.out', 
  sources: [
    'src/main.cpp',
    'src/glad/glad.c',
    'src/stb_image.cpp',
    'src/irg/common.cpp',
    'src/irg/keyboard.cpp',
    'src/irg/window.cpp',
    'src/irg/camera.cpp',
  ],
  include_directories: [
    'include'
  ],
  dependencies: [
    dependency('OpenGL'),
    dependency('glfw3'),
    dependency('glm'),
    meson.get_compiler('c').find_library('dl')
  ],
  override_options: [
    'cpp_std=c++17', 
    'warning_level=3',
  ]
)

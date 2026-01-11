project('chip8', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20', 'b_lundef=false', 'debug=true'])

sfml_dep = dependency('sfml-all', required : true)

executable('chip8',
  'src/main.cpp',
  'src/cpu.cpp',
  'src/memory.cpp',
  'src/chip8.cpp',
  dependencies : [sfml_dep])
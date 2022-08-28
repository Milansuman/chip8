project('chip8', 'cpp')

sources = ['src/main.cpp', 'src/hardware.hpp', 'src/hardware.cpp']
deps = [dependency('sfml-window'), dependency('sfml-graphics'), dependency('sfml-audio')]

executable('chip8', sources: sources, dependencies: deps)
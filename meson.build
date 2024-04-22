project('HelloTriangle', 'c')

warning('this meson file isn\'t finished use with caution')

c = meson.get_compiler('c')

deps = dependency('vulkan','glfw','cglm')

libdir = include_directories('include')

src= static_library('vulkan', 'main.c', dependencies: deps, include_directories : libdir)

executable('HelloTriangle',
	sources: 'main.c',
	link_with: src,
	dependencies: deps,
	include_directories: libdir)

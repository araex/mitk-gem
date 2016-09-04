set(module_dirs
  GemCore
)

foreach(module_dir ${module_dirs})
  add_subdirectory(${module_dir})
endforeach()

set(module_dirs
  GemCore
  GemMesher
)

foreach(module_dir ${module_dirs})
  add_subdirectory(${module_dir})
endforeach()

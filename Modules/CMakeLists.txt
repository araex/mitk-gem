set(module_dirs
  MyAwesomeLib
)

foreach(module_dir ${module_dirs})
  add_subdirectory(${module_dir})
endforeach()

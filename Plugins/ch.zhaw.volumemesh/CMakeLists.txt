project(ch_zhaw_volumemesh)

mitk_create_plugin(
  EXPORT_DIRECTIVE VOLUMEMESH_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt GemMesher
  WARNINGS_NO_ERRORS
)

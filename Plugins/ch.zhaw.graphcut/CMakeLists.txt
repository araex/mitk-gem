project(ch_zhaw_graphcut)

mitk_create_plugin(
  EXPORT_DIRECTIVE GRAPHCUT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)

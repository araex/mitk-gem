project(ch_zhaw_resample)

mitk_create_plugin(
  EXPORT_DIRECTIVE RESAMPLE_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)

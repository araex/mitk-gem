project(ch_zhaw_padding)

mitk_create_plugin(
  EXPORT_DIRECTIVE PADDING_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
  WARNINGS_NO_ERRORS
)

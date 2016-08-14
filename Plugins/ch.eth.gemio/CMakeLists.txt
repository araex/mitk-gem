project(ch_eth_gemio)

mitk_create_plugin(
        EXPORT_DIRECTIVE GEMIO_EXPORT
        EXPORTED_INCLUDE_SUFFIXES src
        MODULE_DEPENDS MitkQtWidgetsExt
        PACKAGE_DEPENDS Qt5|WebEngine
)

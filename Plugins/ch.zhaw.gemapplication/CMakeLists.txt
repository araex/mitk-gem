project(ch_zhaw_gemapplication)

mitk_create_plugin(
        EXPORT_DIRECTIVE GEM_APPLICATION_EXPORT
        EXPORTED_INCLUDE_SUFFIXES src
        MODULE_DEPENDS MitkQtWidgetsExt
        PACKAGE_DEPENDS Qt4|QtWebKit Qt5|WebKitWidgets
)

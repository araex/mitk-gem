project(ch_zhaw_gemapplication)

mitk_create_plugin(
        EXPORT_DIRECTIVE GEM_APPLICATION_EXPORT
        EXPORTED_INCLUDE_SUFFIXES src
        MODULE_DEPENDS MitkCore MitkQtWidgetsExt # We need the MitkCore dependency in order to get the GemIO autoloading module to be noticed by the CPack generetor script. See http://mitk-users.1123740.n5.nabble.com/IO-module-not-loaded-in-package-tp7751p7765.html
        WARNINGS_NO_ERRORS
)

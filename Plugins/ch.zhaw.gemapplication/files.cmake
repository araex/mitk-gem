set(SRC_CPP_FILES)

set(INTERNAL_CPP_FILES
        QmitkExtApplication.cpp
        QmitkExtApplicationPlugin.cpp
        QmitkExtAppWorkbenchAdvisor.cpp
        perspectives/GemPerspective.cpp
        )

set(MOC_H_FILES
        src/internal/QmitkExtApplication.h
        src/internal/QmitkExtApplicationPlugin.h
        src/internal/perspectives/GemPerspective.h
        )

set(UI_FILES
        src/internal/perspectives/QmitkWelcomeScreenViewControls.ui
        )

set(CACHED_RESOURCE_FILES
        plugin.xml
        resources/icon_research.xpm
        resources/perspectives/eye.png
        )

set(QRC_FILES
        resources/QmitkExtApplication.qrc
        resources/welcome/QmitkWelcomeScreenView.qrc
        )

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
    set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
    set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})


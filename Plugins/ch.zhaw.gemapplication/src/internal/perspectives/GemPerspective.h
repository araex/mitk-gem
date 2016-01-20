#pragma once

#include <berryIPerspectiveFactory.h>
#include <QObject.h>

class GemPerspective : public QObject, public berry::IPerspectiveFactory
{
    Q_OBJECT
            Q_INTERFACES(berry::IPerspectiveFactory)

public:

    GemPerspective();

    void CreateInitialLayout(berry::IPageLayout::Pointer layout) override;

};
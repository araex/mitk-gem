#pragma once

#include <QWidget>
#include <QLayout>

#include <tinyxml.h>

#include "PowerLawWidget.h"
#include "PowerLawFunctor.h"

/**
 * Responsible to maintain a consistent state for a group of power laws.
 * - adding / removing of new power laws
 * - makes sure that shared definition boundaries between 2 power law widgets are initialized
 * - disables editing of min / max definition boundaries
 * - power law functor creation
 * - serializing / loading power laws to / from XML
 */
class PowerLawWidgetManager : public QObject {
    Q_OBJECT

public:
    /**
     * Needs a parent QWidget to which it attaches all the power law widgets to.
     */
    PowerLawWidgetManager(QWidget *_parent);

    /**
     * Get a power law widget by index. nullptr if index does not exist.
     */
    PowerLawWidget *getWidget(size_t);
    size_t getNumberOfWidgets();

    /**
     * Creates a power law functor from the current GUI state
     */
    PowerLawFunctor createFunctor();

    // XML I/O
    TiXmlElement *serializeToXml();
    void loadFromXml(TiXmlElement *);

public slots:
    /**
     * Adds a new power law widget and sets up the shared boundary connections
     */
    PowerLawWidget* addPowerLaw();
    /**
     * Removes the last power law widget and updates the shared boundary connections
     */
    bool removePowerLaw();

private:
    /**
     * Removes all current widgets and inserts the given ones
     */
    void setPowerLawWidgets(std::vector<PowerLawWidget *>);
    /**
     * Makes sure all shared boundaries are connected properly
     */
    void updateConnections();

    QWidget *m_Parent;
    std::vector<PowerLawWidget *> m_Widgets;
};
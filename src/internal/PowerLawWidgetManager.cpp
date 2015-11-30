#include "PowerLawWidgetManager.h"

PowerLawWidgetManager::PowerLawWidgetManager(QWidget *_parent) : m_Parent(_parent) {

}

void PowerLawWidgetManager::addPowerLaw() {
    auto widget = new PowerLawWidget;
    m_Parent->layout()->addWidget(widget);
    m_Widgets.push_back(widget);
    updateConnections();
}

void PowerLawWidgetManager::updateConnections() {
    if(m_Widgets.size() == 0){
        return;
    }

    auto first = *(m_Widgets.cbegin());
    auto last = *(m_Widgets.cend() - 1);

    if(m_Widgets.size() == 1){
        first->lockMin(true);
        first->lockMax(true);
    } else if (m_Widgets.size() == 2) {
        first->lockMin(true);
        last->lockMax(true);
        auto mid = PowerLawWidget::Range() / 2 + PowerLawWidget::MinValue;
        first->setMax(mid);
        last->setMin(mid);
        first->connect(last);
    } else {
        auto previousLast = *(m_Widgets.cend() - 2);
        auto min = previousLast->getMin();
        previousLast->setMax(min+1);
        last->setMin(min+1);
        previousLast->connect(last);
    }
}
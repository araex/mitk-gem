#include "PowerLawWidgetManager.h"

PowerLawWidgetManager::PowerLawWidgetManager(QWidget *_parent) : m_Parent(_parent) {
    addPowerLaw(); // always need at least 1
}

PowerLawWidget* PowerLawWidgetManager::addPowerLaw() {
    auto widget = new PowerLawWidget;
    m_Parent->layout()->addWidget(widget);
    m_Widgets.push_back(widget);
    updateConnections();
    return widget;
}

bool PowerLawWidgetManager::removePowerLaw() {
    if(m_Widgets.size() > 1){
        auto widget = m_Widgets.back();
        m_Parent->layout()->removeWidget(widget);
        m_Widgets.pop_back();
        delete widget;
        updateConnections();
        return true;
    }
    return false;
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
        first->setMax(0);
        last->setMin(0);
        first->connect(last);
    } else {
        auto previousLast = *(m_Widgets.cend() - 2);
        auto min = previousLast->getMin();
        previousLast->setMax(min+1);
        last->setMin(min+1);
        last->lockMax(true);
        previousLast->connect(last);
    }
}

PowerLawFunctor PowerLawWidgetManager::createFunctor() {
    PowerLawFunctor ret;
    for(const auto& widget : m_Widgets){
        auto upperBound = widget->getMax();

        // "max" is implemented as SpinBox::setSpecialValueTex, so we need to convert it back
        auto fixedUpperBound = upperBound == PowerLawWidget::MinValue ? PowerLawWidget::MaxValue : upperBound;
        ret.AddPowerLaw(widget->getPowerLawParameters(), fixedUpperBound);
    }

    return ret;
}

PowerLawWidget* PowerLawWidgetManager::getWidget(size_t _idx) {
    if(m_Widgets.size() <= _idx){
        return nullptr;
    }
    return m_Widgets[_idx];
}

size_t PowerLawWidgetManager::getNumberOfWidgets() {
    return m_Widgets.size();
}
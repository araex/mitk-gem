#include "PowerLawWidgetManager.h"

PowerLawWidgetManager::PowerLawWidgetManager(QWidget *_parent) : m_Parent(_parent) {

}

PowerLawWidget* PowerLawWidgetManager::addPowerLaw() {
    auto widget = new PowerLawWidget;
    m_Parent->layout()->addWidget(widget);
    m_Widgets.push_back(widget);
    updateConnections();
    return widget;
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
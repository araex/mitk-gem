#include "PowerLawWidgetManager.h"

#include <QMessageBox>

PowerLawWidgetManager::PowerLawWidgetManager(QWidget *_parent) : m_Parent(_parent) {
    // defaults from paper
    addPowerLaw();
    auto w1 = addPowerLaw();
    w1->setMin(0);
    w1->m_Factor->setValue(6850);
    w1->m_Exponent->setValue(1.49);
}

PowerLawWidget *PowerLawWidgetManager::addPowerLaw() {
    auto widget = new PowerLawWidget;
    m_Parent->layout()->addWidget(widget);
    m_Widgets.push_back(widget);
    updateConnections();
    return widget;
}

bool PowerLawWidgetManager::removePowerLaw() {
    if (m_Widgets.size() > 1) {
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
    if (m_Widgets.size() == 0) {
        return;
    }

    auto first = *(m_Widgets.cbegin());
    auto last = *(m_Widgets.cend() - 1);

    if (m_Widgets.size() == 1) {
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
        previousLast->setMax(min + 1);
        last->setMin(min + 1);
        last->lockMax(true);
        previousLast->connect(last);
    }
}

PowerLawFunctor PowerLawWidgetManager::createFunctor() {
    PowerLawFunctor ret;
    for (const auto &widget : m_Widgets) {
        auto upperBound = widget->getMax();

        // "max" is implemented as SpinBox::setSpecialValueTex, so we need to convert it back
        auto fixedUpperBound = upperBound == PowerLawWidget::MinValue ? PowerLawWidget::MaxValue : upperBound;
        ret.AddPowerLaw(widget->getPowerLawParameters(), fixedUpperBound);
    }

    return ret;
}

PowerLawWidget *PowerLawWidgetManager::getWidget(size_t _idx) {
    if (m_Widgets.size() <= _idx) {
        return nullptr;
    }
    return m_Widgets[_idx];
}

size_t PowerLawWidgetManager::getNumberOfWidgets() {
    return m_Widgets.size();
}

TiXmlElement *PowerLawWidgetManager::serializeToXml() {
    auto root = new TiXmlElement("PowerLaws");

    for (auto &widget : m_Widgets) {
        auto params = widget->getPowerLawParameters();
        auto law = new TiXmlElement("PowerLawParameters");
        law->SetDoubleAttribute("factor", params.factor);
        law->SetDoubleAttribute("exponent", params.exponent);
        law->SetDoubleAttribute("offset", params.offset);
        law->SetDoubleAttribute("rangeMin", widget->getMin());
        law->SetDoubleAttribute("rangeMax", widget->getMax());
        root->LinkEndChild(law);
    }

    return root;
}

void PowerLawWidgetManager::loadFromXml(TiXmlElement *_root) {
    std::vector < PowerLawWidget * > widgets;

    double valFactor, valExponent, valOffset, valMin, valMax;
    for (auto child = _root->FirstChildElement("PowerLawParameters"); child; child = child->NextSiblingElement()) {
        auto r0 = child->QueryDoubleAttribute("factor", &valFactor);
        auto r1 = child->QueryDoubleAttribute("exponent", &valExponent);
        auto r2 = child->QueryDoubleAttribute("offset", &valOffset);
        auto r3 = child->QueryDoubleAttribute("rangeMin", &valMin);
        auto r4 = child->QueryDoubleAttribute("rangeMax", &valMax);

        if (r0 == TIXML_SUCCESS && r1 == TIXML_SUCCESS && r2 == TIXML_SUCCESS && r3 == TIXML_SUCCESS &&
            r4 == TIXML_SUCCESS) {
            auto w = new PowerLawWidget();
            w->m_Factor->setValue(valFactor);
            w->m_Exponent->setValue(valExponent);
            w->m_Offset->setValue(valOffset);
            w->setMin(valMin);
            w->setMax(valMax);
            widgets.push_back(w);
        } else {
            QMessageBox::warning(0, "failed to load power laws", "could not load power laws: invalid file structure.");
            return;
        }
    }

    setPowerLawWidgets(widgets);
}

void PowerLawWidgetManager::setPowerLawWidgets(std::vector < PowerLawWidget * > _widgets) {
    while (removePowerLaw()); // clear
    for (auto &widget : _widgets) {
        m_Parent->layout()->addWidget(widget);
        m_Widgets.push_back(widget);
        updateConnections();
    }
}
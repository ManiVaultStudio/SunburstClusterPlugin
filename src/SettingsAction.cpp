#include "SettingsAction.h"

#include "SunburstClusterPlugin.h"

#include <QFrame>

using namespace mv;
using namespace mv::gui;

SunburstSettings::SunburstSettings(SunburstClusterPlugin& sunburstPlugin) :
    WidgetAction(&sunburstPlugin, "Settings"),
    _sunburstPlugin(sunburstPlugin),
    _dataNameAction(this, "Dataset"),
    _sunburstPlotZoomOption(this, "Zoom on click", false),
    _crossLevelSelectionOption(this, "Select in base data", false)
{
    setText("Settings");
    setSerializationName("Settings");

    _dataNameAction.setDefaultWidgetFlags(StringAction::WidgetFlag::Label);

    _crossLevelSelectionOption.setToolTip("Select in base point data instead of cluster data.\nThis will NOT publish cluster selections but individual points.");
}

void SunburstSettings::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _crossLevelSelectionOption.fromParentVariantMap(variantMap);
    _sunburstPlotZoomOption.fromParentVariantMap(variantMap);
    _dataNameAction.fromParentVariantMap(variantMap);
}

QVariantMap SunburstSettings::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _crossLevelSelectionOption.insertIntoVariantMap(variantMap);
    _sunburstPlotZoomOption.insertIntoVariantMap(variantMap);
    _dataNameAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

SunburstSettings::Widget::Widget(QWidget* parent, SunburstSettings* settingsAction) :
    WidgetActionWidget(parent, settingsAction)
{
    setAutoFillBackground(true);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(3);

    layout->addWidget(settingsAction->getDataNameAction().createLabelWidget(this));
    layout->addWidget(settingsAction->getDataNameAction().createWidget(this));

    QFrame* vLine = new QFrame();
    vLine->setFrameShape(QFrame::VLine);
    vLine->setFrameShadow(QFrame::Sunken);

    layout->addSpacing(4);
    layout->addWidget(vLine);
    layout->addSpacing(4);

    layout->addWidget(settingsAction->getPlotZoomOption().createWidget(this));
    layout->addSpacing(4);
    layout->addWidget(settingsAction->getCrossLevelSelectionOption().createWidget(this));
    layout->addStretch(1);

    setLayout(layout);
}
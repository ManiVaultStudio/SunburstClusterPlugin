#include "SettingsAction.h"

#include "SunburstClusterPlugin.h"

#include <QFrame>

using namespace mv;
using namespace mv::gui;

SunburstSettings::SunburstSettings(QObject* parent) :
    WidgetAction(parent, "Sunburst settings"),
    _dataNameAction(this, "Dataset"),
    _sunburstPlotZoomOption(this, "Focus view", false),
    _crossLevelSelectionOption(this, "Select in base data", false),
    _pointDataSetGUID(this, "Points data set GUI"),
    _clusterDataSetsGUID(this, "Cluster data sets GUI")
{
    setText("Sunburst settings");
    setSerializationName("Sunburst settings");

    _dataNameAction.setDefaultWidgetFlags(StringAction::WidgetFlag::Label);

    _crossLevelSelectionOption.setToolTip("Select in base point data instead of cluster data.\nThis will NOT publish cluster selections but individual points.");
    _sunburstPlotZoomOption.setToolTip("Click to zoom and focus and a partition.");
}

void SunburstSettings::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _crossLevelSelectionOption.fromParentVariantMap(variantMap);
    _sunburstPlotZoomOption.fromParentVariantMap(variantMap);
    _dataNameAction.fromParentVariantMap(variantMap);
    _pointDataSetGUID.fromParentVariantMap(variantMap);
    _clusterDataSetsGUID.fromParentVariantMap(variantMap);
}

QVariantMap SunburstSettings::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _crossLevelSelectionOption.insertIntoVariantMap(variantMap);
    _sunburstPlotZoomOption.insertIntoVariantMap(variantMap);
    _dataNameAction.insertIntoVariantMap(variantMap);
    _pointDataSetGUID.insertIntoVariantMap(variantMap);
    _clusterDataSetsGUID.insertIntoVariantMap(variantMap);

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

#pragma once

#include <actions/StringAction.h>
#include <actions/StringsAction.h>
#include <actions/ToggleAction.h>
#include <actions/WidgetAction.h>

class SunburstSettings : public mv::gui::WidgetAction
{
protected:

    class Widget : public mv::gui::WidgetActionWidget {
    public:
        Widget(QWidget* parent, SunburstSettings* settingsAction);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new SunburstSettings::Widget(parent, this);
    };

public:
    SunburstSettings(QObject* parent);

private slots:

    void onLoadDims();

public slots:
    void onApplyClamping();
    void onApplyDimensionFiltering();

public: // Action getters

    mv::gui::StringAction& getDataNameAction() { return _dataNameAction; }
    mv::gui::ToggleAction& getPlotZoomOption() { return _sunburstPlotZoomOption; }
    mv::gui::ToggleAction& getCrossLevelSelectionOption() { return _crossLevelSelectionOption; }
    mv::gui::StringAction& getPointDataSetGUIDAction() { return _pointDataSetGUID; }
    mv::gui::StringsAction& getClusterDataSetsGUIDAction() { return _clusterDataSetsGUID; }

public: // Serialization

    void fromVariantMap(const QVariantMap& variantMap) override;
    QVariantMap toVariantMap() const override;

private:
    mv::gui::StringAction       _dataNameAction;
    mv::gui::ToggleAction       _sunburstPlotZoomOption;
    mv::gui::ToggleAction       _crossLevelSelectionOption;
    mv::gui::StringAction       _pointDataSetGUID;              // Used for serialization
    mv::gui::StringsAction      _clusterDataSetsGUID;           // Used for serialization
};

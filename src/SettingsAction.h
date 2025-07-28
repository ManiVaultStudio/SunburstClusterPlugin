
#pragma once

#include <actions/StringAction.h>
#include <actions/ToggleAction.h>
#include <actions/WidgetAction.h>

class SunburstClusterPlugin;

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
    SunburstSettings(SunburstClusterPlugin& sunburstPlugin);

private slots:

    void onLoadDims();

public slots:
    void onApplyClamping();
    void onApplyDimensionFiltering();

public: // Action getters

    mv::gui::StringAction& getDataNameAction() { return _dataNameAction; }
    mv::gui::ToggleAction& getPlotZoomOption() { return _sunburstPlotZoomOption; }
    mv::gui::ToggleAction& getCrossLevelSelectionOption() { return _crossLevelSelectionOption; }

public: // Serialization

    /**
     * Load plugin from variant map
     * @param Variant map representation of the plugin
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save plugin to variant map
     * @return Variant map representation of the plugin
     */
    QVariantMap toVariantMap() const override;

private:
    SunburstClusterPlugin&      _sunburstPlugin;
    mv::gui::StringAction       _dataNameAction;
    mv::gui::ToggleAction       _sunburstPlotZoomOption;
    mv::gui::ToggleAction       _crossLevelSelectionOption;
};

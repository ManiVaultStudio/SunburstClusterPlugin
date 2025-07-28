#pragma once

#include "LoadDialog.h"

#include <Dataset.h>
#include <ViewPlugin.h>

#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>

#include <memory>
#include <vector>

#include <QString>
#include <QStringList>

namespace mv {
    namespace gui {
        class DropWidget;
    }
}

// =============================================================================
// View
// =============================================================================


class SunburstSettings;
class SunburstWidget;

class SunburstClusterPlugin : public mv::plugin::ViewPlugin
{
    Q_OBJECT
    
public:
    SunburstClusterPlugin(const mv::plugin::PluginFactory* factory);
    ~SunburstClusterPlugin();
    
    void init() override;
    
    /**
     * Load one (or more datasets in the view)
     * @param datasets Dataset(s) to load
     */
    void loadData(const mv::Datasets& datasets) override;

    void loadDataImpl();

    /**
     * Callback which is invoked when &Dataset<Points>::dataSelectionChanged is emitted
     */
    //void onDataSelectionChanged();

    mv::CoreInterface* getCore() { return _core;  }
    QString getCurrentDataSetName() const;
    QString getCurrentDataSetID() const;

private:
    // Parses data to JSON and passes it to the web widget
    void plotSunburn();

    /** Updates the window title (includes the name of the loaded dataset) */
    void updateWindowTitle();

protected:
    // informs the core about a selection 
    void publishSelection(const QString& clusterNames);

signals:
    void dataSetChanged();

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

    using ClusterDatasets = QVector<mv::Dataset<Clusters>>;

    mv::Dataset<Points>         _currentPointDataSet = {};
    ClusterDatasets             _currentClusterDataSets = {};

    SunburstWidget*             _sunburstWidget = nullptr;
    SunburstSettings*           _settingsWidget = nullptr;
    std::unique_ptr<LoadDialog> _loadDialog = {};
    mv::gui::DropWidget*        _dropWidget = nullptr;
};


// =============================================================================
// Factory
// =============================================================================

class SunburstClusterPluginFactory : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.SunburstClusterPlugin"
                      FILE  "PluginInfo.json")
    
public:
    SunburstClusterPluginFactory();

    ~SunburstClusterPluginFactory() override {}

    mv::plugin::ViewPlugin* produce() override;

    /** Returns the data types that are supported by the example view plugin */
    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    mv::gui::PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};

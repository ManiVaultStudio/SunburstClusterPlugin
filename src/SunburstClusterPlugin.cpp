#include "SunburstClusterPlugin.h"

#include "SunburstClusterWidget.h"
#include "Utils.h"
#include "SettingsAction.h"

#include "DataHierarchyItem.h"
#include "event/Event.h"
#include "Dataset.h"
#include "PointData/PointData.h"
#include "ClusterData/ClusterData.h"

#include <actions/PluginTriggerAction.h>
#include <widgets/DropWidget.h>
#include <DatasetsMimeData.h>

#include <QtCore>
#include <QtConcurrent> 
#include <QtDebug>
#include <QVariantMap> 

#include <algorithm>
#include <cassert>
#include <utility>

Q_PLUGIN_METADATA(IID "studio.manivault.SunburstClusterPlugin")

using namespace mv;

// =============================================================================
// View
// =============================================================================

SunburstClusterPlugin::SunburstClusterPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _loadDialog(std::make_unique<LoadDialog>(nullptr))
{ 
    connect(_loadDialog.get(), &LoadDialog::accepted, this, &SunburstClusterPlugin::loadDataImpl);
}

SunburstClusterPlugin::~SunburstClusterPlugin()
{
}

void SunburstClusterPlugin::init()
{
    // Load webpage
    _sunburstWidget = new SunburstWidget();
    _sunburstWidget->setPage(":sunburst_plot/sunburst/sunburst.html", "qrc:/sunburst_plot/sunburst/");     // set html contents of webpage

    // Create layout
    QVBoxLayout* layout = new QVBoxLayout();
    
    _settingsWidget    = new SunburstSettings(*this);
    _dropWidget        = new gui::DropWidget(_sunburstWidget);
    auto& pluginWidget = getWidget();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(_settingsWidget->createWidget(&pluginWidget));
    layout->addWidget(_sunburstWidget, 1);

    pluginWidget.setLayout(layout);
    pluginWidget.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    _dropWidget->setDropIndicatorWidget(new gui::DropWidget::DropIndicatorWidget(&pluginWidget, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this](const QMimeData* mimeData) -> gui::DropWidget::DropRegions {
        gui::DropWidget::DropRegions dropRegions;

        const auto datasetsMimeData = dynamic_cast<const DatasetsMimeData*>(mimeData);

        if (datasetsMimeData == nullptr)
            return dropRegions;

        if (datasetsMimeData->getDatasets().count() > 1)
            return dropRegions;

        const auto dataset        = datasetsMimeData->getDatasets().first();
        const auto datasetGuiName = dataset->text();
        const auto datasetId      = dataset->getId();
        const auto dataType       = dataset->getDataType();

        if (dataType == PointType) {
            const auto candidateDataset  = mv::data().getDataset(datasetId);
            const auto candidateChildren = candidateDataset->getChildren();

            if (datasetId == getCurrentDataSetID()) {
                dropRegions << new gui::DropWidget::DropRegion(this, "Warning", "Data already loaded", "exclamation-circle", false);
            }
            else if(candidateChildren.size() < 1) {
                dropRegions << new gui::DropWidget::DropRegion(this, "Warning", "Data must have at least 1 cluster child", "exclamation-circle", false);
            }
            else if (std::none_of(candidateChildren.begin(), candidateChildren.end(), [](const auto& childDataset) { return childDataset->getDataType() == ClusterType; })) {
                dropRegions << new gui::DropWidget::DropRegion(this, "Warning", "Data must have at least 1 cluster child", "exclamation-circle", false);
            }
            else {
                dropRegions << new gui::DropWidget::DropRegion(this, "Points", QString("Visualize %1 as parallel coordinates").arg(datasetGuiName), "map-marker-alt", true, [this, candidateDataset]() {
                    _dropWidget->setShowDropIndicator(false);
                    loadData({ candidateDataset });
                    });

            }
        }
        else {
            dropRegions << new gui::DropWidget::DropRegion(this, "Incompatible data", "Only point data types are supported", "exclamation-circle", false);
        }

        return dropRegions;
        });

    // Update the window title when the GUI name of the position dataset changes
    connect(&_currentPointDataSet, &Dataset<Points>::guiNameChanged, this, &SunburstClusterPlugin::updateWindowTitle);

    // Update the selection (coming from core) in sunburst
    //connect(&_currentPointDataSet, &Dataset<Points>::dataSelectionChanged, this, &SunburstClusterPlugin::onDataSelectionChanged);

    // Update the selection (coming from sunburst) in core
    connect(&_sunburstWidget->getCommunicationObject(), &SunburstCommunicationObject::newSelectionInSunburst, this, &SunburstClusterPlugin::publishSelection);

    // Update plot when zoom option is toggled
    connect(&_settingsWidget->getPlotZoomOption(), &gui::ToggleAction::toggled, this, [this](bool) {
        const bool optZoom = _settingsWidget->getPlotZoomOption().isChecked();
        _sunburstWidget->passOptToJS(optZoom);
        });

    updateWindowTitle();
}

void SunburstClusterPlugin::loadDataImpl()
{
    if (!_sunburstWidget->isWebPageLoaded())
        return;

    QList<std::int32_t> selectionClusterData = _loadDialog->getClusterOptionIndices();

    if (selectionClusterData.size() != _currentClusterDataSets.size()) {
        ClusterDatasets filtered;
        for (std::int32_t id : selectionClusterData) {
            if (id >= 0 && id < _currentClusterDataSets.size()) {
                filtered.append(_currentClusterDataSets[id]);
            }
        }

        std::swap(_currentClusterDataSets, filtered);
    }

    // save data guid for serialization
    //_settingsWidget->getDataGUIDAction().setString(_currentPointDataSet->getId());

    // display data name 
    //_settingsWidget->getDataNameLabel().setText(_currentPointDataSet->text());

    // parse data to JS in a different thread as to not block the UI
    QFuture<void> fvoid = QtConcurrent::run(&SunburstClusterPlugin::plotSunburn, this);

    updateWindowTitle();
}

void SunburstClusterPlugin::loadData(const mv::Datasets& datasets)
{
    // Exit if there is nothing to load
    if (datasets.isEmpty())
        return;

    _dropWidget->setShowDropIndicator(false);

    _currentPointDataSet = datasets.first();
    _currentClusterDataSets.clear();

    QStringList clusterDataNames;

    for (const auto& childDataset : _currentPointDataSet->getChildren()) {
        if (childDataset->getDataType() == ClusterType) {
            _currentClusterDataSets.append(childDataset);
            clusterDataNames.append(childDataset->getGuiName());
        }
    }

    _loadDialog->setClusterSetNames(clusterDataNames);
    _loadDialog->show();
}

QString SunburstClusterPlugin::getCurrentDataSetName() const 
{ 
    if (_currentPointDataSet.isValid())
        return _currentPointDataSet->text();
    else
        return QString{};
}

QString SunburstClusterPlugin::getCurrentDataSetID() const 
{ 
    if (_currentPointDataSet.isValid())
        return _currentPointDataSet->getId();
    else
        return QString{};
}

//void SunburstClusterPlugin::onDataSelectionChanged()
//{
//    // Get the selection set and respective IDs that changed
//    const auto& selectionSet = _currentPointDataSet->getSelection<Points>();
//    const auto& selectionIndices = selectionSet->indices;
//
//    // send them to js side
//    _sunburstWidget->passSelectionToJS(selectionIndices);
//}

void SunburstClusterPlugin::plotSunburn()
{
    const size_t numClusterData = _currentClusterDataSets.size();

    JsonNode root;
    root.name = _currentPointDataSet->getGuiName();

    // create helper vector
    std::vector<std::vector<bool>> visited;
    visited.resize(numClusterData);
    for (size_t currentClusterID = 0; currentClusterID < numClusterData; currentClusterID++) {
        visited[currentClusterID].resize(_currentClusterDataSets[currentClusterID]->getClusters().size(), currentClusterID == 0);
        
        for (auto& cluster : _currentClusterDataSets[currentClusterID]->getClusters()) {
            std::sort(SUN_PARALLEL_EXECUTION
                cluster.getIndices().begin(), 
                cluster.getIndices().end()
            );
        }
    }

    // map the clusters to a hierachy
    // assumption:
    // if an ID of a finer-level cluster is in a coarser level cluster, there is a parent-child relation
    auto populateNode = [this, &visited, numClusterData](const Cluster& cluster, size_t coarserClusterLevel) -> JsonNode {
        auto populateNode_impl = [this, &visited, numClusterData](const Cluster& cluster, size_t coarserClusterLevel, auto& self) -> JsonNode {
            JsonNode node;
            node.name = cluster.getName();
            node.color = cluster.getColor().name();

            if (coarserClusterLevel == numClusterData) {
                node.value = cluster.getNumberOfIndices();
            }
            else {
                // Find the finer-level that overlap
                const auto& coarserClusters     = _currentClusterDataSets[coarserClusterLevel]->getClusters();
                const size_t numCoarserClusters = visited[coarserClusterLevel].size();

                for (size_t clusterID = 0; clusterID < numCoarserClusters; clusterID++) {
                    if (visited[coarserClusterLevel][clusterID])
                        continue;

                    if (hasCommonElement(coarserClusters[clusterID].getIndices(), cluster.getIndices())) {
                        visited[coarserClusterLevel][clusterID] = true;
                        node.children.append(self(coarserClusters[clusterID], coarserClusterLevel + 1, self));
                    }
                
                }
            }

            return node;
            };
        return populateNode_impl(cluster, coarserClusterLevel, populateNode_impl);
    };

    const auto& topLevelClusterData = _currentClusterDataSets.first();

    for (const auto& topLevelCluster : topLevelClusterData->getClusters()) {
        root.children.append(populateNode(topLevelCluster, 1));
    }

    const QString dataJsonStr = root.toJsonStr();
    const bool optZoom = _settingsWidget->getPlotZoomOption().isChecked();

    _sunburstWidget->passDataToJS(dataJsonStr, optZoom);
}

void SunburstClusterPlugin::publishSelection(const QString& clusterNamesStr)
{
    // For each cluster dataset we parse the selected cluster IDs
    const size_t numClusterData = _currentClusterDataSets.size();
    std::vector<std::vector<std::uint32_t>> selectedClusters;
    selectedClusters.resize(numClusterData);

    const QList<QStringList> clusterNames = convertJsSelectionClusterNames(clusterNamesStr);

    for (const auto& clusterPath : clusterNames) {
        assert(clusterPath.size() > 2); // each path is suffixed with the base point data name

        const QString& clusterName = clusterPath.back();
        const size_t clusterDataID = clusterPath.size() - 2;

        assert(clusterDataID < numClusterData);

        const auto allClusterNames = _currentClusterDataSets[clusterDataID]->getClusterNames();
        const auto clusterID       = findIndex(allClusterNames, clusterName);

        if (clusterID.has_value()) {
            selectedClusters[clusterDataID].push_back(static_cast<std::uint32_t>(clusterID.value()));
        }
    }

    if (_settingsWidget->getCrossLevelSelectionOption().isChecked()) {
        std::vector<std::uint32_t> selectedPointIDs;

        for (size_t clusterDataID = 0; clusterDataID < numClusterData; clusterDataID++) {
            const auto& clusters = _currentClusterDataSets[clusterDataID]->getClusters();

            for(const auto& clustersID: selectedClusters[clusterDataID]) {
                const auto& pointIDs = clusters[clustersID].getIndices();
                selectedPointIDs.insert(selectedPointIDs.end(), pointIDs.begin(), pointIDs.end());
            }

        }

        _currentPointDataSet->getSelection<Points>()->indices = std::move(selectedPointIDs);
        events().notifyDatasetDataSelectionChanged(_currentPointDataSet);
    }
    else {
        // Limitations of this approach: 
        // if there are selections across multiple hierarchi levels
        // only the selection in the highest-selected level will be honored
        // this is due to ManiVaults selection handling, which will
        // overwrite previous selection when selecting in another cluster 
        // that belongs to the same point data.
        size_t noSelections = 0;
        for (size_t clusterDataID = 0; clusterDataID < numClusterData; clusterDataID++) {
            const auto& selectedClustersForID = selectedClusters[clusterDataID];
            auto& clusterData = _currentClusterDataSets[clusterDataID];

            if (selectedClustersForID.empty()) {
                noSelections++;
                continue;
            }

            clusterData->setSelectionIndices(selectedClustersForID);
            events().notifyDatasetDataSelectionChanged(clusterData);
        }

        // Reset selection of all are empty
        if (noSelections == numClusterData) {
            for (size_t clusterDataID = 0; clusterDataID < numClusterData; clusterDataID++) {
                auto& clusterData = _currentClusterDataSets[clusterDataID];

                clusterData->setSelectionIndices({});
                events().notifyDatasetDataSelectionChanged(clusterData);
            }
        }
    }
}

void SunburstClusterPlugin::updateWindowTitle()
{
    QString currentDatasetName = _currentPointDataSet.isValid() ? _currentPointDataSet->text() : "[None]";

    getWidget().setWindowTitle(QString("%1: %2").arg(getGuiName(), currentDatasetName));
    _settingsWidget->getDataNameAction().setString(currentDatasetName);
}

void SunburstClusterPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);

    _settingsWidget->fromParentVariantMap(variantMap);
}

QVariantMap SunburstClusterPlugin::toVariantMap() const
{
    QVariantMap variantMap = ViewPlugin::toVariantMap();

    _settingsWidget->insertIntoVariantMap(variantMap);

    return variantMap;
}

// =============================================================================
// Factory
// =============================================================================

SunburstClusterPluginFactory::SunburstClusterPluginFactory()
{
    setIconByName("sun");
}

plugin::ViewPlugin* SunburstClusterPluginFactory::produce()
{
    return new SunburstClusterPlugin(this);
}

mv::DataTypes SunburstClusterPluginFactory::supportedDataTypes() const
{
    return { PointType };
}

mv::gui::PluginTriggerActions SunburstClusterPluginFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    gui::PluginTriggerActions pluginTriggerActions;

    const auto getInstance = [this]() -> SunburstClusterPlugin* {
        return dynamic_cast<SunburstClusterPlugin*>(plugins().requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (PluginFactory::areAllDatasetsOfTheSameType(datasets, PointType)) {
        if (numberOfDatasets >= 1) {
            if (datasets.first()->getDataType() == PointType) {
                auto pluginTriggerAction = new gui::PluginTriggerAction(const_cast<SunburstClusterPluginFactory*>(this), this, "Sunburst plot", "Load dataset in sunburst plot", icon(), [this, getInstance, datasets](gui::PluginTriggerAction& pluginTriggerAction) -> void {
                    for (const auto& dataset : datasets)
                        getInstance()->loadData(Datasets({ dataset }));
                });

                pluginTriggerActions << pluginTriggerAction;
            }
        }
    }

    return pluginTriggerActions;
}
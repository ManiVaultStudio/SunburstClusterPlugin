#include "SunburstClusterWidget.h"

#include <PointData/PointData.h>

#include <QDebug>

#include <algorithm>

using namespace mv;
using namespace mv::gui;

// =============================================================================
// SunburstCommunicationObject
// =============================================================================

void SunburstCommunicationObject::js_passSelectedClustersToQt(const QString& clusterNames) {
    // pass selction to core
    emit newSelectionInSunburst(clusterNames);
}

//void SunburstCommunicationObject::newSelectionToJS(const std::vector<unsigned int>& selectionIDs) {
//    QVariantList selection;
//
//    // if nothing is selected, tell the parcoords to show all data
//    if (selectionIDs.size() == 0)
//    {
//        selection.append("-");
//    }
//    // otherwise send all IDs
//    else
//    {
//        for (const auto& ID : selectionIDs)
//            selection.append(ID);
//    }
//
//    // send data to JS
//    //emit qt_setSelectionInJS(selection);
//}

// =============================================================================
// SunburstWidget
// =============================================================================

SunburstWidget::SunburstWidget():
    mv::gui::WebWidget()
{
    setAcceptDrops(true);   // drag & drop handled in SunburstClusterPlugin.cpp

    Q_INIT_RESOURCE(sunburst_resources);
    init(&_communicationObject);

    layout()->setContentsMargins(0, 0, 0, 0);
}

void SunburstWidget::passDataToJS(const QString& data, const bool optZoom)
{
    emit _communicationObject.qt_setDataInJS(data, optZoom);
}

void SunburstWidget::passOptToJS(const bool optZoom)
{
    emit _communicationObject.qt_setOptInJS(optZoom);
}

//void SunburstWidget::passSelectionToJS(const std::vector<unsigned int>& selectionIDs)
//{
//    _communicationObject.newSelectionToJS(selectionIDs);
//}

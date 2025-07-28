#pragma once 

#include "widgets/WebWidget.h"

#include <QVariantList>

// =============================================================================
// SunburstCommunicationObject
// =============================================================================

class SunburstCommunicationObject : public mv::gui::WebCommunicationObject
{
    Q_OBJECT

signals:
    // Signals to JS side
    void qt_setDataInJS(const QString& data, const bool optZoom = false);
    void qt_setOptInJS(const bool optZoom = false);

    //void qt_setSelectionInJS(const QVariantList& selection);

    // Signals Qt internal
    void newSelectionInSunburst(const QString& clusterNames);

public slots:
    // Invoked from JS side 
    void js_passSelectedClustersToQt(const QString& clusterNames);

    // converts vector to array string and emits qt_setSelectionInJS
    //void newSelectionToJS(const std::vector<unsigned int>& selectionIDs);

private:
    //std::vector<unsigned int> _selectedIDsFromJS = {};

};


// =============================================================================
// SunburstWidget
// =============================================================================

class SunburstWidget : public mv::gui::WebWidget
{
    Q_OBJECT

public:
    SunburstWidget();

    void passDataToJS(const QString& data, const bool optZoom = false);
    void passOptToJS(const bool optZoom);
    //void passSelectionToJS(const std::vector<unsigned int>& selectionIDs);

    SunburstCommunicationObject& getCommunicationObject() { return _communicationObject; };

private:
    SunburstCommunicationObject     _communicationObject = {};
};

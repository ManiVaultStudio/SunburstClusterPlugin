#pragma once

#include <actions/GroupAction.h>
#include <actions/OptionsAction.h>
#include <actions/TriggerAction.h>

#include <QDialog>
#include <QSize>
#include <QStringList>
#include <QWidget>

// =============================================================================
// Loading input box
// =============================================================================

class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    LoadDialog(QWidget* parent);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(400, 50);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    void setClusterSetNames(const QStringList& clusterSetNames);
    QStringList getClusterSetNames();
    QList<std::int32_t> getClusterOptionIndices();

protected:
    mv::gui::OptionsAction      _clusterSetSelectionAction;     /** Data type action */
    mv::gui::GroupAction        _groupAction;                   /** Data type action */
    mv::gui::TriggerAction      _okButton;
};

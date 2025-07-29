#pragma once

#include <actions/GroupAction.h>
#include <actions/OptionsAction.h>
#include <actions/TriggerAction.h>
#include <util/Serializable.h>

#include <QDialog>
#include <QList>
#include <QSize>
#include <QStringList>
#include <QWidget>

#include <cstdint>

// =============================================================================
// Loading input box
// =============================================================================

class LoadDialog : public QDialog, public mv::util::Serializable
{
    Q_OBJECT

public:
    LoadDialog(QWidget* parent);

public: // Setter

    void setClusterSetNames(const QStringList& clusterSetNames);

public: // Getter

    QStringList getClusterSetNames() const;
    QList<std::int32_t> getClusterOptionIndices() const;

    mv::gui::OptionsAction& getOptionsAction() { 
        return _clusterSetSelectionAction; 
    }

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(400, 50);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

public: // Serialization

    void fromVariantMap(const QVariantMap& variantMap) override;
    QVariantMap toVariantMap() const override;

protected:
    mv::gui::OptionsAction      _clusterSetSelectionAction;
    mv::gui::GroupAction        _groupAction;
    mv::gui::TriggerAction      _okButton;
};

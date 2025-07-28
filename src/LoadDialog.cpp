#include "LoadDialog.h"

#include <QGridLayout>

using namespace mv;
using namespace mv::gui;

LoadDialog::LoadDialog(QWidget* parent) :
    QDialog(parent),
    _clusterSetSelectionAction(this, "Cluster sets", {}, {}),
    _okButton(this, "Ok"),
    _groupAction(this, "Load Settings")
{
    setWindowTitle(tr("Sunburst input data"));
    setModal(true);

    _groupAction.addAction(&_clusterSetSelectionAction);

    auto layout = new QGridLayout();
    layout->setContentsMargins(10, 10, 10, 10);
    int row = 0;

    QLabel* infoText = new QLabel(this);
    infoText->setText(
        "Select all cluster data sets that should be displayed.\n"
        "Datasets have to be ordered hierarchically already.\n"
        "Cluster indices will be sorted."
    );

    layout->addWidget(infoText, row, 0, 1, 4);
    layout->addWidget(_groupAction.createWidget(this), ++row, 0, 1, 4);
    layout->addWidget(_okButton.createWidget(this), ++row, 3, 1, 1, Qt::AlignRight);

    setLayout(layout);

    connect(&_okButton, &mv::gui::TriggerAction::triggered, this, &QDialog::accept);
}

void LoadDialog::setClusterSetNames(const QStringList& clusterSetNames) {
    _clusterSetSelectionAction.setOptions(clusterSetNames, true);
    _clusterSetSelectionAction.selectAll();
}

QStringList LoadDialog::getClusterSetNames() {
    return _clusterSetSelectionAction.getSelectedOptions();
}

QList<std::int32_t> LoadDialog::getClusterOptionIndices() {
    return _clusterSetSelectionAction.getSelectedOptionIndices();
}

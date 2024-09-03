#pragma once
#include <qdialog.h>
#include <qspinbox.h>
#include "ui_NNSettingsDialog.h"

class NNSettingsDialog :
    public QDialog
{
    Q_OBJECT

public:
    NNSettingsDialog(QVector<int> layerInfo, double learnrate, QWidget *parent = nullptr);
    ~NNSettingsDialog();

    QVector<int> getLayerInfo();
    double getLearnFactor();

    void setupDialog();

public slots:
    void layerCountSPChanged(int i);

private:
    Ui::NNSettingsDialog m_ui;
    QVector<int> m_layerInfo;
    double m_learnFactor;

    QList<QSpinBox*> m_spinboxes;
};


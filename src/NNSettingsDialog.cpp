#include "NNSettingsDialog.h"
#include <qlabel.h>



NNSettingsDialog::NNSettingsDialog(QVector<int> layerInfo, double learnrate, QWidget *parent) :
    QDialog(parent), m_layerInfo(layerInfo), m_learnFactor(learnrate)
{
    m_ui.setupUi(this);
    this->setWindowTitle("NN Settings");

    setMinimumWidth(1000);



    setupDialog();

    QObject::connect(m_ui.spinBox_layerCount, SIGNAL(valueChanged(int)), this, SLOT(layerCountSPChanged(int)));
}


NNSettingsDialog::~NNSettingsDialog()
{
}

void NNSettingsDialog::setupDialog()
{
    QGridLayout * pGridLayout = m_ui.gridLayout_layerInfo;

    // Layout leeren >>>
    /*for (int y = 0; y < pGridLayout->rowCount(); ++y)
    {
        for (int x = 0; x < pGridLayout->columnCount(); ++x)
        {
            QLayoutItem * pItem = pGridLayout->itemAtPosition(y, x);

            if (pItem)
            {
                pGridLayout->removeItem(pItem);
                QWidget * pWidget = pItem->widget();
                delete pWidget;
                delete pItem;
            }
        }
    }*/
    // <<< Layout leeren


    m_ui.doubleSpinBox_learnFactor->setValue(m_learnFactor);
  
    m_ui.spinBox_layerCount->setValue(m_layerInfo.size());


    pGridLayout->addWidget(new QLabel("Neurons on each Layer:"), 0, 0);
    for (int i = 0; i < m_layerInfo.size(); i++)
    {
        QSpinBox *pSBNeuronCount = new QSpinBox(this);
        pSBNeuronCount->setMinimum(1);
        pSBNeuronCount->setMaximum(1024);
        pSBNeuronCount->setValue(m_layerInfo[i]);
        pGridLayout->addWidget(pSBNeuronCount, 1, i);
        m_spinboxes.push_back(pSBNeuronCount);
    }
}


QVector<int> NNSettingsDialog::getLayerInfo()
{
    QVector<int> layerinfo;

    for (int i = 0; i < m_spinboxes.size(); i++)
    {
        layerinfo.push_back(m_spinboxes[i]->value());
    }
    return layerinfo;
}
double NNSettingsDialog::getLearnFactor()
{
    return m_ui.doubleSpinBox_learnFactor->value();
}

void NNSettingsDialog::layerCountSPChanged(int i)
{
    QGridLayout * pGridLayout = m_ui.gridLayout_layerInfo;
    int amountOfNewLayers = i - m_spinboxes.size();

    if (amountOfNewLayers > 0)
    {
        for (int j = 0; j < amountOfNewLayers; j++)
        {
            QSpinBox *pSBNeuronCount = new QSpinBox(this);
            pSBNeuronCount->setMinimum(1);
            pSBNeuronCount->setMaximum(1024);
            pGridLayout->addWidget(pSBNeuronCount, 1, m_spinboxes.size());
            m_spinboxes.push_back(pSBNeuronCount);
        }
    }
    else if (amountOfNewLayers < 0)
    {
        amountOfNewLayers *= (-1);
        for (int j = 0; j < amountOfNewLayers; j++)
        {
            QLayoutItem * pItem = pGridLayout->itemAtPosition(1, m_spinboxes.size() - 1);

            if (pItem)
            {
                m_spinboxes.removeLast();
                pGridLayout->removeItem(pItem);
                QWidget * pWidget = pItem->widget();
                delete pWidget;
                delete pItem;
            }
        }
    }
}
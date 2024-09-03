#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDebug>
#include <qsettings.h>
#include <qfiledialog.h>
#include <QtConcurrent>


#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>
#include "filter.h"
#include "resizer.h"
#include "utility.h"
#include "LogListItem.h"
#include "NNSettingsDialog.h"

#include <chrono>


using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mp_ui(new Ui::MainWindow),
    mp_scene(new QGraphicsScene(this))
{
    mp_ui->setupUi(this);

    connect(mp_ui->actionOpen, SIGNAL(triggered()), this, SLOT(openActionTriggered()));
    connect(mp_ui->actionStart, SIGNAL(triggered()), this, SLOT(startActionTriggered()));
    //connect(mp_ui->actionSave_Contours, SIGNAL(triggered()), this, SLOT(saveContoursActionTriggered()));
    connect(mp_ui->actionNeural_Network_Settings, SIGNAL(triggered()), this, SLOT(neurakNetworknSettingsActionTriggered()));
    connect(mp_ui->actionStart_Neural_Network_Training, SIGNAL(triggered()), this, SLOT(startNNTrainingActionTriggered()));
    connect(mp_ui->actionStop_Neural_Network_Training, SIGNAL(triggered()), this, SLOT(stopNNTrainingActionTriggered()));
    //connect(mp_ui->actionMake_traindata, SIGNAL(triggered()), this, SLOT(maketraindataActionTriggered()));

    connect(&m_processingManager, SIGNAL(sendLogMessage(QString, QColor, QPixmap)), this, SLOT(displayLogMessage(QString, QColor, QPixmap)));
    connect(mp_ui->log_listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(logMessageClicked(QListWidgetItem*)));


    
    mp_ui->graphicsView->setScene(mp_scene);
    mp_ui->graphicsView->setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

}

MainWindow::~MainWindow()
{
    delete mp_ui;
}


void MainWindow::startActionTriggered()
{
    // start the processing in another thread
    QtConcurrent::run(&m_processingManager, &ProcessingManager::startProcessing);
}

void MainWindow::startNNTrainingActionTriggered()
{
    // start the training in another thread
    QtConcurrent::run(&m_processingManager, &ProcessingManager::startNNTraining);
}
void MainWindow::stopNNTrainingActionTriggered()
{
    // start the training in another thread
    QtConcurrent::run(&m_processingManager, &ProcessingManager::stopNNTraining);
}

void MainWindow::openActionTriggered()
{
    // opens the file explorer dialog and lets the user select an image (jpg, png, tiff or tif)
    QSettings settings;
    const QString lastInputDir = settings.value("lastInputDirectory").toString();
    const QString fileName = QFileDialog::getOpenFileName(this, "Open File(s)", lastInputDir, "Bilder (*.jpg *.png *.tiff *.tif *.ppm *.bmp)");

    if (false == fileName.isEmpty())
    {
        settings.setValue("lastInputDirectory", QFileInfo(fileName).absoluteDir().absolutePath());

        // delete the logs from the previous image
        mp_ui->log_listWidget->clear();


        const QPixmap image = QPixmap(fileName);

        mp_scene->clear();
        mp_image = mp_scene->addPixmap(image);

        // scales the scene to fit all the items inside it
        mp_ui->graphicsView->fitInView(mp_scene->itemsBoundingRect(), Qt::KeepAspectRatio);

        // load the image into the ProcessingManager
        m_processingManager.loadImage(fileName);
    }
}

void MainWindow::saveContoursActionTriggered()
{
    const QList<Contour> *contours = m_processingManager.candidateList();

    for(int i = 0; i < contours->size(); i++)
    {
        // an attempt to not have the files overwrite each other. the names of the files arent relevant
        String saveBasePath = "C://Users//fohss//Documents//git//bildverarbeitung//signData//";
        String imageName = saveBasePath + QString::number(rand()).toStdString() + QString::number(rand()).toStdString() + QString::number(rand()).toStdString() + ".jpg";

        std::vector<int> imSaveParams;
        imSaveParams.push_back(IMWRITE_JPEG_QUALITY);
        imSaveParams.push_back(100);
        imwrite(imageName, contours->at(i).image(), imSaveParams);
    }
}
void MainWindow::maketraindataActionTriggered()
{
    // opens the file explorer dialog and lets the user select an image (jpg, png, tiff or tif)
    QSettings settings;
    const QString lastInputDir = settings.value("lastInputDirectory").toString();
    const QString dirName = QFileInfo(QFileDialog::getOpenFileName(this, "Open File(s)", lastInputDir, "Bilder (*.jpg *.png *.tiff *.tif *.ppm *.bmp)")).absoluteDir().absolutePath();

    if (false == dirName.isEmpty())
    {
        settings.setValue("lastInputDirectory", dirName);
        qDebug() << "abs path: " << dirName;

        vector<string> fileNames = Utility::getFileNames(dirName.toStdString());

        for(size_t i = 0; i < fileNames.size(); i++)
        {
            // delete the logs from the previous image
            mp_ui->log_listWidget->clear();

            qDebug() << "image Nr " << (i+1) << "/" << fileNames.size();

            // load the image into the ProcessingManager
            m_processingManager.loadImage(QString::fromStdString(fileNames[i]));

            // start the processing
            //QtConcurrent::run(&m_processingManager, &ProcessingManager::startProcessing);
            m_processingManager.startProcessing();

        }
    }
}

void MainWindow::neurakNetworknSettingsActionTriggered()
{
    QVector<int> layerInfo = m_processingManager.neuralNetwork()->layerInfo();
    double learnFactor = m_processingManager.neuralNetwork()->learnFactor();

    NNSettingsDialog nnSettingsDialog(layerInfo, learnFactor, this);
    const int retDialog = nnSettingsDialog.exec();
    if (QDialog::Accepted == retDialog)
    {
        layerInfo = nnSettingsDialog.getLayerInfo();
        learnFactor = nnSettingsDialog.getLearnFactor();

        // reset the neuralnetwork that the m_processingManager is holding
        m_processingManager.resettNeuralNetwork(layerInfo, learnFactor);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    mp_ui->graphicsView->fitInView(mp_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::displayLogMessage(QString message, QColor color, QPixmap image)
{
    LogListItem *newLogItem = new LogListItem(message, color, image);

    mp_ui->log_listWidget->insertItem(0, newLogItem);
}

void MainWindow::logMessageClicked(QListWidgetItem *item)
{
    LogListItem *logItem = dynamic_cast<LogListItem*>(item);
    if (logItem != 0)
    {
        mp_scene->clear();
        // load the pixmap that is attatched to the logItem into the scene of the graphicsview
        mp_image = mp_scene->addPixmap(logItem->m_image);

        // scales the scene to fit all the items inside it
        mp_ui->graphicsView->fitInView(mp_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}
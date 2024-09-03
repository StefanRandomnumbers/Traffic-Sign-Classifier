#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <qscatterseries.h>
#include <qthread.h>
#include <qlistwidget.h>

#include "ProcessingManager.h"

#include <qgraphicsscene.h>
#include <qpixmap.h>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow :
    public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

public slots:
    void startActionTriggered();
    void openActionTriggered();
    void saveContoursActionTriggered();
    void maketraindataActionTriggered();
    void neurakNetworknSettingsActionTriggered();
    void startNNTrainingActionTriggered();
    void stopNNTrainingActionTriggered();

    void displayLogMessage(QString message, QColor color, QPixmap image);
    void logMessageClicked(QListWidgetItem *item);
    

private:
    Ui::MainWindow *mp_ui;
    QGraphicsScene *mp_scene;
    QGraphicsPixmapItem *mp_image;

    ProcessingManager m_processingManager;
};

#endif // MAINWINDOW_H

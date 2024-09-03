#pragma once
#include <qpixmap.h>

#include <qlistwidget.h>

class LogListItem :
    public QListWidgetItem
{
public:
    LogListItem(QString logMessage, QColor color, QPixmap im) :
        QListWidgetItem(logMessage),
        m_image(im)
    {
        setForeground(color);
    }

    LogListItem(QString logMessage, QColor color) :
        QListWidgetItem(logMessage)
    {
        setForeground(color);
    }

    ~LogListItem()
    {}

    QPixmap m_image;
};
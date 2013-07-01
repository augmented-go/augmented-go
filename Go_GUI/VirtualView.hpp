#pragma once
#include <QApplication>
#include "qwidget.h"
#include <qgraphicsview.h>


class QGLSceneNode;

class VirtualView : public QGraphicsView
{
    Q_OBJECT
public:
    VirtualView(QWidget *parent = 0);
    ~VirtualView();
	void createAndSetScene(QSize size);

};

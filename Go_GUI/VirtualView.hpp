#pragma once
#include <QApplication>
#include "qwidget.h"
#include <qgraphicsview.h>


class QGLSceneNode;

class VirtualView : public QGraphicsView
{
    Q_OBJECT
public:
    VirtualView();
    ~VirtualView();
	void createAndSetScene(QSize size);

};

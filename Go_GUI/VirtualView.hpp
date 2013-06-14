#pragma once
#include <QApplication>
#include "qwidget.h"
#include "qglview.h"
#include "QGLAbstractScene.h"

class QGLSceneNode;

class VirtualView : public QGLView
{
    Q_OBJECT
public:
    VirtualView(QWidget *parent = 0) : m_scene(0) {}
    ~VirtualView();

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

private:
    QGLAbstractScene *m_scene;
	QGLSceneNode* m_main;
	QQuaternion m_pose;
};
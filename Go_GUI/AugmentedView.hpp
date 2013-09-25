#pragma once

#include <QGLView>
#include <QGLAbstractScene>

class AugmentedView : public QGLView
{
 
public:
    AugmentedView(QWidget *parent = 0);
    ~AugmentedView();

	void setImage(QImage image);
	void rescaleImage(QSize size){};
	void resize(int, int);
	void resize(QSize size);
 
protected:
    void initializeGL(QGLPainter * painter);
    void paintGL(QGLPainter * painter);
	
 
private:
    QGLAbstractScene *m_scene;
    QGLSceneNode *m_rootNode;
};
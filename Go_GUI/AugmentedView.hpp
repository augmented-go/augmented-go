#pragma once

#include <QGLView>
#include <QGLAbstractScene>
#include <QGLBuilder>
#include <QGraphicsBillboardTransform>

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
	float ratio;
    QGLAbstractScene *m_scene;
    QGLSceneNode *m_stone;
	QGLSceneNode *m_background;
	QGraphicsBillboardTransform* faceToCamera;
	QGLTexture2D white_stone_tex, background_tex;
	QGLMaterial white_stone_mat, background_mat;
};
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
	void resize(QSize size);
	void updateDifferences(QList<QPoint>* errors = 0);
 
protected:
    void initializeGL(QGLPainter * painter);
    void paintGL(QGLPainter * painter);
	
 
private:
	float cont_width, cont_height, img_width, img_height;
	int stone_size;
	QList<QPoint> white_stones;
	QList<QPoint> black_stones;
	QList<QPoint> redglowing_stones;
    QGLSceneNode *m_white_stone, *m_black_stone, *m_redglowing_stone;
	QGLSceneNode *m_background;
	QGLTexture2D background_tex;
	QGLMaterial white_stone_mat, black_stone_mat, redglowing_stone_mat, background_mat;
};
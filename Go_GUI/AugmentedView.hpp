#pragma once
#include <QApplication>
#include "qwidget.h"
#include "qglview.h"
#include "qglbuilder.h"

class QGLSceneNode;

class AugmentedView : public QGLView
{
    Q_OBJECT
public:
    AugmentedView(QWidget *parent = 0) {
		m_scene = new QGLSceneNode();
		board_material = new QGLMaterial;
		augmented_picture.setPath(QLatin1String("../Go_GUI/textures/example_pic.jpg"));
		augmented_picture.setScheme(QLatin1String("file"));
	
		board_material->setDiffuseColor(Qt::white);
		board_material->setTextureUrl(augmented_picture);
		this->setOption(QGLView::CameraNavigation, false);	// disables mouse interaction
		//setKeyboardGrabEnabled(true);
		//this->setMouseGrabEnabled(true);
	}
	~AugmentedView(){};
protected:
	void initializeGL(QGLPainter *painter){
		QGLBuilder builder;
		builder.addPane(3);
		QGLSceneNode* augmented = builder.finalizedSceneNode();
		
		this->camera()->setEye(QVector3D(0.0f, 0.0f, 10.0f));
		this->camera()->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
		this->camera()->setCenter(QVector3D(0.0f,0.0f,0.0f));

		augmented->setMaterial(board_material);

		m_scene->addNode(augmented);

		painter->setStandardEffect(QGL::LitDecalTexture2D);
	};

	void paintGL(QGLPainter *painter){
		if (m_scene)
			m_scene->draw(painter);
	};
private:
    QGLSceneNode *m_scene;
	QGLMaterial *board_material;
	QUrl augmented_picture;
};
#include "virtualview.hpp"
#include "qglbuilder.h"
#include "qglscenenode.h"
#include "qglteapot.h"
#include "QGLAbstractScene.h"
#include <QMessageBox>

void VirtualView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
	 // Background color
    painter->setClearColor(QColor(0, 0, 0));

    this->m_scene = QGLAbstractScene::loadScene(QLatin1String("../Go_GUI/models/example_board.3ds"));
	if (this->m_scene == NULL)
		QMessageBox::critical(this->parent_Widget, QString("file not found!"), QString("The 3ds file of go-board was not found!"));

	QGLSceneNode* scene_camera = this->m_scene->mainNode()->findChild<QGLSceneNode *>("Camera001");
	scene_camera->setPosition(QVector3D(50.0f, 0.0f, -80.0f));


	if (scene_camera){
		float y = scene_camera->position().y();
		QString s = QString::number(y);
	}
	//this->setCamera(new QGLCamera(scene_camera));
	this->camera()->setEye(scene_camera->position());
	this->camera()->setUpVector(QVector3D(0.0f, 0.0f, -1.0f));
	this->camera()->setCenter(QVector3D(0.0f,0.0f,0.0f));

	QList<QGLSceneNode* > l = this->m_scene->mainNode()->allChildren();
	std::vector<QString> vstrings;
	for (QGLSceneNode* a : l)
		vstrings.push_back(a->objectName());
	QGLSceneNode* stone1 = this->m_scene->mainNode()->findChild<QGLSceneNode *>("Sphere001");

	QGLMaterial* m = new QGLMaterial();
	m->setDiffuseColor(Qt::white);
	m->setAmbientColor(Qt::white);
	if (stone1){
		QString s = stone1->objectName();
		stone1->setMaterial(m);
	}
	if (m_scene)
         m_main = m_scene->mainNode();

	QQuaternion xt = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f);
    QQuaternion yt = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 30.0f);
	m_pose = yt * xt;

	painter->setStandardEffect(QGL::LitDecalTexture2D);
}

VirtualView::~VirtualView()
{
    delete m_scene;
}

void VirtualView::paintGL(QGLPainter *painter)
{
	painter->modelViewMatrix().rotate(m_pose);

	if (m_main)
		m_main->draw(painter);
	
}

/*
protected:
	void initializeGL(QGLPainter *painter){
		/*QGLBuilder builder;
		builder.addPane(3);
		QGLSceneNode* augmented = builder.finalizedSceneNode();
		
		//this->camera()->setEye(QVector3D(0.0f, 0.0f, 10.0f));
		//this->camera()->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
		//this->camera()->setCenter(QVector3D(0.0f,0.0f,0.0f));

		augmented->setMaterial(board_material);

		m_scene->addNode(augmented);

		painter->setStandardEffect(QGL::LitDecalTexture2D);

	};

	void paintGL(QGLPainter *painter){
		//if (m_scene)
		//	m_scene->draw(painter);
	};
	*/

	/*m_scene = new QGLSceneNode();
		board_material = new QGLMaterial;
		augmented_picture.setPath(QLatin1String("../Go_GUI/textures/example_pic.jpg"));
		augmented_picture.setScheme(QLatin1String("file"));
	
		board_material->setDiffuseColor(Qt::white);
		board_material->setTextureUrl(augmented_picture);
		//this->setOption(QGLView::CameraNavigation, false);	// disables mouse interaction
		//setKeyboardGrabEnabled(true);
		//this->setMouseGrabEnabled(true);*/
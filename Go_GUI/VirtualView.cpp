#include "virtualview.hpp"
#include "qglbuilder.h"
#include "qglscenenode.h"
#include "qglteapot.h"
#include "QGLAbstractScene.h"
#include <QMessageBox>

#include <string>

void VirtualView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
    this->m_scene = QGLAbstractScene::loadScene(QLatin1String("../o_GUI/models/example_board.3ds"));
	if (this->m_scene == NULL) 
		QMessageBox::critical(this->parent_Widget, QString("file not found!"), QString("The 3ds file of go-board was not found!"));

	QGLSceneNode* scene_camera = this->m_scene->mainNode()->findChild<QGLSceneNode *>("Camera001");
	QString s = QString::number(scene_camera->position().y());

	this->camera()->setEye(QVector3D(50.0f, 0.0f, -80.0f));
	this->camera()->setUpVector(QVector3D(0.0f, 0.0f, -1.0f));
	this->camera()->setCenter(QVector3D(0.0f,0.0f,0.0f));

	QList<QGLSceneNode* > l = this->m_scene->mainNode()->allChildren();
	std::vector<QString> vstrings;
	for (QGLSceneNode* a : l)
		vstrings.push_back(a->objectName());
	QGLSceneNode* stone1 = this->m_scene->mainNode()->findChild<QGLSceneNode *>("Sphere001");

	QGLMaterial* m = new QGLMaterial();
	m->setColor(QColor(255,255,255));
	if (stone1){
		QString s = stone1->objectName();
		stone1->setMaterial(m);
	}
	if (m_scene)
         m_main = m_scene->mainNode();

	

	QQuaternion xt = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f);
    QQuaternion yt = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 30.0f);
	m_pose = yt * xt;
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
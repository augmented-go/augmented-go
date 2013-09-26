#include "AugmentedView.hpp"

#include <math.h>


AugmentedView::AugmentedView(QWidget *parent)
	: m_scene(0)
    , m_stone(0)
{
}
 
AugmentedView::~AugmentedView()
{
    //delete m_scene;
}
 
void AugmentedView::initializeGL(QGLPainter * painter)
{
 
 
    //this->setCsetClearBeforeRendering(true);
 
    //viewer.setFlags(Qt::FramelessWindowHint);

	// Viewing Volume
    camera()->setFieldOfView(25);
    camera()->setNearPlane(1);
    camera()->setFarPlane(1000);
 
    // Position of the camera
    camera()->setEye(QVector3D(0, 0, 50));
 
    // Direction that the camera is pointing
    camera()->setCenter(QVector3D(0, 0, 0));
	camera()->setUpVector(QVector3D(0,1,0));
 
    // Load the 3d model from the file
	m_scene = QGLAbstractScene::loadScene("res/models/stone.3ds");
    m_stone = m_scene->mainNode();

	float distance = camera()->eye().length();
	float pane_scale = distance / tan(camera()->fieldOfView() / 180.0f * 3.14) /**/ / 5.5f; // testvalue
	qDebug() << "width: " << pane_scale; //70
	QGLBuilder builder;
	
	qDebug() << "ratio: " << ratio;
	builder.addPane(pane_scale);
	m_background = builder.finalizedSceneNode();

	background_tex.setSize(QSize(1000,1000));
	background_tex.setImage(QImage("res/textures/go_board_13.png"));
	background_mat.setTexture(&background_tex);

	white_stone_tex.setSize(QSize(100,100));
	white_stone_tex.setImage(QImage("res/textures/white_stone.png"));
	white_stone_mat.setTexture(&white_stone_tex);
	white_stone_mat.setDiffuseColor(QColor(255,255,255));

	for (QGLSceneNode* s : m_stone->allChildren()){
		s->setMaterial(&white_stone_mat);
		s->setEffect(QGL::StandardEffect::LitMaterial);
		s->updated();
	}

	m_background->setEffect(QGL::StandardEffect::LitModulateTexture2D);
	m_background->setMaterial(&background_mat);	
}
 
void AugmentedView::paintGL(QGLPainter * painter)
{
	painter->modelViewMatrix().scale(ratio, 1,1);
	m_background->draw(painter);
	
	painter->modelViewMatrix().scale(1/ratio, 1, 1);
    
	qDebug() << camera()->eye().normalized();
	/*QVector3D rotVec = QVector3D::crossProduct(QVector3D(0,0,1), camera()->eye().normalized());
	float rot = acos(QVector3D::dotProduct(QVector3D(0,0,1), camera()->eye().normalized())) * 57.2957795;
	float rot2 = acos(QVector3D::dotProduct(QVector3D(0,0,1), camera()->eye().normalized())) * 57.2957795;
	//rot2 = rot2 / 3.64904f; // correction !?!?!?!
	if (camera()->eye().z() < 0)
		rot2 += 180;
	qDebug() << rot;
	qDebug() << rot2;
	qDebug() << rotVec.normalized();
	
	
	//painter->modelViewMatrix().rotate(90, camera()->eye().normalized());
	
	QMatrix4x4 mat;
	mat.rotate(rot2, camera()->eye().normalized());
	mat.rotate(rot, rotVec.normalized());
	mat.scale(1.0f, 0.9f, 1.0f);
	//painter->modelViewMatrix() *= mat;
	//painter->modelViewMatrix().rotate(rot2-90, QVector3D(0,0,1));*/
	//QMatrix4x4 mat;
	//mat.rotate(-90, QVector3D(1,0,0));
	//painter->modelViewMatrix() *= mat;
	
	painter->modelViewMatrix().scale(0.2);	// testvalue
	painter->modelViewMatrix().rotate(90, QVector3D(1,0,0));
	m_stone->draw(painter);
}

void AugmentedView::setImage(QImage image){
	background_tex.setImage(image);
	background_mat.setTexture(&background_tex);
	ratio = image.width() * 1.0f/image.height();
	qDebug() << ratio;
}

void AugmentedView::resize(int w, int h){
	float distance = camera()->eye().length();
	float width = distance / tan(camera()->fieldOfView() / 180.0f * 3.14) / 4.5f; // 4.5?!
	qDebug() << "width: " << width;
	ratio = w * 1.0f/h;
	qDebug() << ratio;
}

void AugmentedView::resize(QSize size){
	qDebug() << "resized!";
	ratio = size.width() * 1.0f/size.height();
	qDebug() << ratio;
}
#include "AugmentedView.hpp"

#include <math.h>

float radToDeg = 57.2957795;
float degToRad = 0.0174527;

AugmentedView::AugmentedView(QWidget *parent)
{}
 
AugmentedView::~AugmentedView()
{
    //delete m_scene;
}
 
void AugmentedView::initializeGL(QGLPainter * painter)
{ 
    //this->setCsetClearBeforeRendering(true);
    //viewer.setFlags(Qt::FramelessWindowHint);

	// Viewing Volume
    camera()->setFieldOfView(60);
    camera()->setNearPlane(1);
    camera()->setFarPlane(1000);
 
    // Position of the camera
    camera()->setEye(QVector3D(0, 300, 0));
	camera()->setProjectionType(QGLCamera::ProjectionType::Perspective);

    // Direction that the camera is pointing
    camera()->setCenter(QVector3D(0, 0, 0));

	//up vector
	camera()->setUpVector(QVector3D(0, 0, -1));
 
    // Load the 3d model from the file
	m_white_stone = QGLAbstractScene::loadScene("res/models/stone.3ds")->mainNode();
	m_black_stone = QGLAbstractScene::loadScene("res/models/stone.3ds")->mainNode();
	m_redglowing_stone = QGLAbstractScene::loadScene("res/models/stone.3ds")->mainNode();

	// Creating background pane (camera picture)
	float distance = camera()->eye().length();
	float pane_size = distance / tan(camera()->fieldOfView() * degToRad);
	//qDebug() << "distance: " << distance;
	//qDebug() << "width: " << pane_size; //19,4963
	QGLBuilder builder;
	builder.addPane(QSize(pane_size*2, pane_size*2));
	m_background = builder.finalizedSceneNode();

	// Setting textures and materials
	background_mat.setTexture(&background_tex);
	m_background->setEffect(QGL::StandardEffect::LitModulateTexture2D); // texture
	m_background->setMaterial(&background_mat);	

	white_stone_mat.setDiffuseColor(QColor(255,255,255));
	black_stone_mat.setDiffuseColor(QColor(255,255,255));
	redglowing_stone_mat.setAmbientColor(QColor(255,0,0));
	redglowing_stone_mat.setDiffuseColor(QColor(200,0,0));

	// not yet found out how to find child (functions don't work)
	for(QGLSceneNode* s : m_white_stone->allChildren()){
		s->setMaterial(&white_stone_mat);
		s->setEffect(QGL::StandardEffect::LitMaterial);	// no texture, just diffuse
		s->updated();
	}

	for(QGLSceneNode* s : m_black_stone->allChildren()){
		s->setMaterial(&black_stone_mat);
		s->setEffect(QGL::StandardEffect::LitMaterial);	// no texture, just diffuse
		s->updated();
	}

	for(QGLSceneNode* s : m_redglowing_stone->allChildren()){
		s->setMaterial(&redglowing_stone_mat);
		s->setEffect(QGL::StandardEffect::LitMaterial);	// no texture, just diffuse
		s->updated();
	}
	
	stone_size = 3;


	// for tests
	white_stones.push_back(QPoint(245,90));
	redglowing_stones.push_back(QPoint(309,334));
	redglowing_stones.push_back(QPoint(376,335));
}
 
void AugmentedView::paintGL(QGLPainter * painter)
{
	float scene_width = camera()->eye().length() / tan(camera()->fieldOfView() * degToRad) * 2 * cont_width/cont_height;
	float scene_height = camera()->eye().length() / tan(camera()->fieldOfView() * degToRad) * 2;

	float px_fct_w = scene_width / img_width;
	float px_fct_h = scene_height / img_height;
	
	//x:532, y:419
	int posX = 123; //- img_width/2 + 2; // +2 correction value
	int posY = 426;
	
	for (QPoint coords : white_stones){
		painter->modelViewMatrix().push();
		painter->modelViewMatrix().translate(px_fct_w * (coords.x() - img_width/2 + 2), 0, px_fct_h * (coords.y() - img_height/2));
		painter->modelViewMatrix().scale(stone_size/(cont_width/cont_height),stone_size, stone_size);
		m_white_stone->draw(painter);
		painter->modelViewMatrix().pop();
	}
	
	for (QPoint coords : black_stones){
		painter->modelViewMatrix().push();
		painter->modelViewMatrix().translate(px_fct_w * (coords.x() - img_width/2 + 2), 0, px_fct_h * (coords.y() - img_height/2));
		painter->modelViewMatrix().scale(stone_size/(cont_width/cont_height),stone_size, stone_size);
		m_black_stone->draw(painter);
		painter->modelViewMatrix().pop();
	}
	
	for (QPoint coords : redglowing_stones){
		painter->modelViewMatrix().push();
		painter->modelViewMatrix().translate(px_fct_w * (coords.x() - img_width/2 + 2), 0, px_fct_h * (coords.y() - img_height/2));
		painter->modelViewMatrix().scale(stone_size/(cont_width/cont_height),stone_size, stone_size);
		m_redglowing_stone->draw(painter);
		painter->modelViewMatrix().pop();
	}
	
	// Resetting MV-Matrix to show camera picture
	painter->modelViewMatrix().setToIdentity();
	painter->modelViewMatrix().translate(0,0,-camera()->eye().length());
	painter->modelViewMatrix().scale(cont_width/cont_height, 1, 1);
	m_background->draw(painter);
}

void AugmentedView::setImage(QImage image){
	background_tex.setImage(image);
	background_tex.setSize(image.size());
	background_mat.setTexture(&background_tex);
	img_width = image.width() * 1.0f;
	img_height = image.height() * 1.0f;
	//qDebug() << ratio;
}

void AugmentedView::updateDifferences(QList<QPoint>* errors){
	// just for testing
	//errors = new QList<QPoint>();
	//errors->push_back(QPoint(245,90));
	//errors->push_back(QPoint(376,335));
	this->redglowing_stones = *errors;
}

void AugmentedView::resize(int w, int h){
	float distance = camera()->eye().length();
	float width = distance / tan(camera()->fieldOfView() * degToRad) / 4.5f; // 4.5?!
	//ratio = w * 1.0f/h;
	//qDebug() << ratio;
}

void AugmentedView::resize(QSize size){
	qDebug() << "resized!";
	//camera()->setViewSize(size);
	cont_width = size.width();
	cont_height = size.height();

	
}
#include "AugmentedView.hpp"

AugmentedView::AugmentedView(QWidget *parent)
	: m_scene(0)
    , m_rootNode(0)
{}
 
AugmentedView::~AugmentedView()
{
    delete m_scene;
}
 
void AugmentedView::initializeGL(QGLPainter * painter)
{
    // Background color
    painter->setClearColor(QColor(10, 10, 10, 0));

	// Viewing Volume
    camera()->setFieldOfView(25);
    camera()->setNearPlane(1);
    camera()->setFarPlane(1000);
 
    // Position of the camera
    camera()->setEye(QVector3D(0, 50, 50));
 
    // Direction that the camera is pointing
    camera()->setCenter(QVector3D(0, 0, 0));
 
    // Load the 3d model from the file
	m_scene = QGLAbstractScene::loadScene("res/models/example_board.3ds");
    m_rootNode = m_scene->mainNode();
	QGLMaterial* board = new QGLMaterial();
	QGLTexture2D* board_tex = new QGLTexture2D();
	board_tex->setImage(QImage("res/textures/go_board_13.png"));
	board_tex->setSize(QSize(200,200));
	board->setTexture(board_tex);
	board->setAmbientColor(QColor(100,100,100));
	board->setDiffuseColor(QColor(100,100,100));
	for (QGLSceneNode* s : m_rootNode->allChildren())
		s->setMaterial(board);

	m_rootNode->setMaterial(board);
}
 
void AugmentedView::paintGL(QGLPainter * painter)
{
    m_rootNode->draw(painter);
}

void AugmentedView::setImage(QImage image){
	//this->set(QSurfaceFormat(
}

void AugmentedView::resize(int i, int a){
	qDebug() << "resized!";
}

void AugmentedView::resize(QSize size){
		qDebug() << "resized!";
}
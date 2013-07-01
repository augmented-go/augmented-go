#include "virtualview.hpp"
#include "qglbuilder.h"
#include "qglscenenode.h"
#include "qglteapot.h"
#include "QGLAbstractScene.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>

VirtualView::VirtualView(QWidget *parent){
    this->setParent(parent);
}
VirtualView::~VirtualView(){
}

void VirtualView::createAndSetScene(QSize size)
{
	QGraphicsScene* scene = new QGraphicsScene();

	//directories of the images
    QString texture_path = "res/textures/";
	QString board_string = QString(texture_path + "go_board_19.png");
	QString black_stone_string = QString(texture_path + "black_stone.png");
	QString white_stone_string = QString(texture_path + "white_stone.png");

	//loads the images
	QImage* board_image = new QImage(board_string);
	QImage* black_stone_image = new QImage(black_stone_string);
	QImage* white_stone_image = new QImage(white_stone_string);

	if (board_image->isNull())
		QMessageBox::warning(this, "file loading error", "could not load board image!");
	if (black_stone_image->isNull())
		QMessageBox::warning(this, "file loading error", "could not load black stone image!");
	if (white_stone_image->isNull())
		QMessageBox::warning(this, "file loading error", "could not laod white stone image!");

	//scale the images to the right size

	float scale_x = size.width() / float(board_image->width());
	float scale_y = size.height() / float(board_image->height());

	QPixmap board_image_scaled = QPixmap::fromImage(*board_image);
	board_image_scaled = board_image_scaled.scaled(size.width(),size.height());

	QPixmap black_stone_image_scaled = QPixmap::fromImage(*black_stone_image);
	black_stone_image_scaled = black_stone_image_scaled.scaled(black_stone_image->width()*scale_x, black_stone_image->height()*scale_y);

	QPixmap white_stone_image_scaled = QPixmap::fromImage(*white_stone_image);
	white_stone_image_scaled = white_stone_image_scaled.scaled(white_stone_image->width()*scale_x, white_stone_image->height()*scale_y);

	if (board_image_scaled.isNull())
		QMessageBox::warning(this, "image scale error", "could not scale board!");
	if (black_stone_image_scaled.isNull())
		QMessageBox::warning(this, "image scale error", "could not scale black stone!");
	if (white_stone_image_scaled.isNull())
		QMessageBox::warning(this, "image scale error", "could not scale white stone!");

	//add the board image to the scene
	scene->addItem(new QGraphicsPixmapItem(board_image_scaled));

	//this is a 19x19 test array to display a board with stones. (0 = noone; 1 = black; 2 = white)
	int test [19][19] = {	{1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
							{1,1,2,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0},
							{0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,1,2,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,2,1,1,0,0,0,0,0,0},
							{0,0,0,0,0,0,1,1,1,1,2,2,1,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0,0,0,2,2,2,2,1,1,1,0,0,0} }; 
	
	//add the stones to the scene at the right position like in the test array above
	for(int row = 0; row<19; row++)
	{
		for(int col = 0; col<19; col++)
		{
			if(test[row][col] == 1)
			{
				QGraphicsPixmapItem* black_stone_item = new QGraphicsPixmapItem(black_stone_image_scaled);
				black_stone_item->setPos(black_stone_image_scaled.width()*float(col),black_stone_image_scaled.width()*float(row));
				black_stone_item->setOffset(black_stone_image_scaled.width()/2.0,black_stone_image_scaled.height()/2.0);
				scene->addItem(black_stone_item);
			}
			if(test[row][col] == 2)
			{
				QGraphicsPixmapItem* white_stone_item = new QGraphicsPixmapItem(white_stone_image_scaled);
				white_stone_item->setPos(white_stone_image_scaled.width()*float(col),white_stone_image_scaled.width()*float(row));
				white_stone_item->setOffset(white_stone_image_scaled.width()*0.5,white_stone_image_scaled.height()*0.5);
				scene->addItem(white_stone_item);
			}
		}
	}



	this->setScene(scene);
}


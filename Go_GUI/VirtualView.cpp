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

	//TODO: load board size from backend
	int board_size = 19;

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

	//scale_x and scale_y are the scaling factors of the virtual board
	float scale_x = size.width() / float(board_image->width());
	float scale_y = size.height() / float(board_image->height());

	float cell_width = float(board_image->width()) / (board_size+1);
	float cell_height = float(board_image->height()) / (board_size+1);

	//scale the images to the right size
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
	int test [19][19] = {	{1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1}, 
							{1,1,2,0,0,0,0,0,0,2,0,0,0,0,0,0,0,1,1},
							{0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1},
							{0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,1},
							{0,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,1},
							{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,1,1,2,0,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,1,2,1,1,0,0,0,0,0,1},
							{0,0,0,0,0,0,1,1,1,1,2,2,1,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1},
							{0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,1},
							{1,0,0,0,0,0,0,0,0,2,2,2,2,1,1,1,0,0,1} }; 
	
	//add the stones to the scene at the right position like in the test array above
	for(int row = 0; row<19; row++)
	{
		for(int col = 0; col<19; col++)
		{
			if(test[row][col] == 1)
			{
				QGraphicsPixmapItem* black_stone_item = new QGraphicsPixmapItem(black_stone_image_scaled);
				black_stone_item->setPos(cell_width * scale_x * col, cell_height * scale_y * row);
				black_stone_item->setOffset(cell_width * scale_x - black_stone_image_scaled.width()/2, cell_height * scale_y - black_stone_image_scaled.height()/2);
				scene->addItem(black_stone_item);
			}
			if(test[row][col] == 2)
			{
				QGraphicsPixmapItem* white_stone_item = new QGraphicsPixmapItem(white_stone_image_scaled);
				white_stone_item->setPos(cell_width * scale_x * col, cell_height * scale_y * row);
				white_stone_item->setOffset(cell_width * scale_x - white_stone_image_scaled.width()/2, cell_height * scale_y - white_stone_image_scaled.height()/2);
				scene->addItem(white_stone_item);
			}
		}
	}



	this->setScene(scene);
}


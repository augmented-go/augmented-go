#include "virtualview.hpp"

#include <QGLBuilder>
#include <QGLSceneNode>
#include <QGLAbstractScene>
#include <QMessageBox>
#include <QGraphicsPixmapItem>

#include <GoBoard.h>

VirtualView::VirtualView(QWidget *parent){
    this->setParent(parent);

    // directories of the images
    QString texture_path = "res/textures/";
    QString board_directory_size9 = QString(texture_path + "go_board_" + QString::number(9)+".png");
    QString board_directory_size13 = QString(texture_path + "go_board_" + QString::number(13)+".png");
    QString board_directory_size19 = QString(texture_path + "go_board_" + QString::number(19)+".png");
    QString black_stone_directory = QString(texture_path + "black_stone.png");
    QString white_stone_directory = QString(texture_path + "white_stone.png");

    // loads the images and checks if the image could loaded
    board_image_size9 = QImage(board_directory_size9);
    board_image_size13 = QImage(board_directory_size13);
    board_image_size19 = QImage(board_directory_size19);
    black_stone_image = QImage(black_stone_directory);
    white_stone_image = QImage(white_stone_directory);

}
VirtualView::~VirtualView(){
}

/**
 * @brief	Creates the virtual board of the go game and set the scene
 * @param	QSize	size of the container	
 */
void VirtualView::createAndSetScene(QSize size, const GoBoard * game_board)
{
    if (game_board == nullptr)
        return;

    scene.clear();
    
    // loads the board size and checks if its a valid size
    int board_size = game_board->Size();

    QImage board_image;

    switch(board_size){
    case 9:
        board_image = board_image_size9;
        break;
    case 13:
        board_image = board_image_size13;
        break;
    case 19:
        board_image = board_image_size19;
        break;
    default:
        QMessageBox::warning(this, "board size error", "invalid size of the board!");
    }

    if (board_image.isNull())
        QMessageBox::warning(this, "file loading error", "could not load board image!");
    if (black_stone_image.isNull())
        QMessageBox::warning(this, "file loading error", "could not load black stone image!");
    if (white_stone_image.isNull())
        QMessageBox::warning(this, "file loading error", "could not laod white stone image!");

    // scale_x and scale_y are the scaling factors of the virtual board
    float scale_x = size.width() / float(board_image.width());
    float scale_y = size.height() / float(board_image.height());

    float cell_width = float(board_image.width()) / (board_size+1);
    float cell_height = float(board_image.height()) / (board_size+1);

    // scale the images to the right size
    QPixmap board_image_scaled = QPixmap::fromImage(board_image);
    board_image_scaled = board_image_scaled.scaled(size.width(),size.height());

    QPixmap black_stone_image_scaled = QPixmap::fromImage(black_stone_image);
    black_stone_image_scaled = black_stone_image_scaled.scaled(black_stone_image.width()*scale_x, black_stone_image.height()*scale_y);

    QPixmap white_stone_image_scaled = QPixmap::fromImage(white_stone_image);
    white_stone_image_scaled = white_stone_image_scaled.scaled(white_stone_image.width()*scale_x, white_stone_image.height()*scale_y);

    if (board_image_scaled.isNull())
        QMessageBox::warning(this, "image scale error", "could not scale board!");
    if (black_stone_image_scaled.isNull())
        QMessageBox::warning(this, "image scale error", "could not scale black stone!");
    if (white_stone_image_scaled.isNull())
        QMessageBox::warning(this, "image scale error", "could not scale white stone!");

    // add the board image to the scene
    scene.addItem(new QGraphicsPixmapItem(board_image_scaled));

    // get all stone positions for each color and add them on the right position to the scene
    auto black_stones = game_board->All(SG_BLACK);
    for (auto iter = SgSetIterator(black_stones); iter; ++iter) {
        auto point = *iter;

        // Reducing by -1 because board starts at 1,1
        auto col = SgPointUtil::Col(point) - 1;
        auto row = SgPointUtil::Row(point) - 1;

        //Vertically mirroring stones 
        row = board_size - row - 1;        

        QGraphicsPixmapItem* black_stone_item = new QGraphicsPixmapItem(black_stone_image_scaled);
        black_stone_item->setPos(cell_width * scale_x * col, cell_height * scale_y * row);
        black_stone_item->setOffset(cell_width * scale_x - black_stone_image_scaled.width()/2, cell_height * scale_y - black_stone_image_scaled.height()/2);
        scene.addItem(black_stone_item);
    }

    auto white_stones = game_board->All(SG_WHITE);
    for (auto iter = SgSetIterator(white_stones); iter; ++iter) {
        auto point = *iter;

        // Reducing by -1 because board starts at 1,1
        auto col = SgPointUtil::Col(point) - 1;
        auto row = SgPointUtil::Row(point) - 1;

        //Vertically mirroring stones 
        row = board_size - row - 1;

        QGraphicsPixmapItem* black_stone_item = new QGraphicsPixmapItem(white_stone_image_scaled);
        black_stone_item->setPos(cell_width * scale_x * col, cell_height * scale_y * row);
        black_stone_item->setOffset(cell_width * scale_x - white_stone_image_scaled.width()/2, cell_height * scale_y - black_stone_image_scaled.height()/2);
        scene.addItem(black_stone_item);
    }
    this->setScene(&scene);
}


#include "virtualview.hpp"

#include <QGLBuilder>
#include <QGLSceneNode>
#include <QGLAbstractScene>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QtGui\QMouseEvent>

#include <GoBoard.h>


VirtualView::VirtualView(QWidget *parent){
    this->setParent(parent);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->resize(parent->size());

    this->setMouseTracking(true);
    this->setting_stone_valid = false;

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

    // initialize with nullptr to be able to test against that
    ghost_stone = nullptr;
}
VirtualView::~VirtualView(){
}

void VirtualView::createAndSetScene(QSize size, const GoBoard * game_board)
{
    if (game_board == nullptr)
        return;

    this->resize(size);

    scene.clear();
    scene.setSceneRect(0,0, size.width(), size.height());
    fitInView(this->sceneRect());

    // loads the board size and checks if its a valid size
    board_size = game_board->Size();

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

    cell_width = static_cast<qreal>(board_image.width()) / (board_size+1);
    cell_height = static_cast<qreal>(board_image.height()) / (board_size+1);
    

    // scale the images to the right size
    QPixmap board_image_scaled = QPixmap::fromImage(board_image);
    board_image_scaled = board_image_scaled.scaled(size.width(),size.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPixmap black_stone_image_scaled = QPixmap::fromImage(black_stone_image);
    black_stone_image_scaled = black_stone_image_scaled.scaled(black_stone_image.width()*scale_x, black_stone_image.height()*scale_y, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPixmap white_stone_image_scaled = QPixmap::fromImage(white_stone_image);
    white_stone_image_scaled = white_stone_image_scaled.scaled(white_stone_image.width()*scale_x, white_stone_image.height()*scale_y, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

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
    
    // Stone that could be placed on board when user chooses to
    if (this->virtual_game_mode){
        if (ghost_stone == nullptr)
            this->ghost_stone = new QGraphicsEllipseItem(QRectF());
        
        ghost_stone->setRect(selection_ellipse);
        QBrush ghost_brush = game_board->ToPlay() == SG_BLACK ? 
                            QBrush(Qt::GlobalColor::black):
                            QBrush(Qt::GlobalColor::white);
            
        this->ghost_stone->setOpacity(0.5);
        this->ghost_stone->setBrush(ghost_brush);
        this->scene.addItem(this->ghost_stone);
    }
    this->setScene(&scene);

    setting_stone_valid = true;
}

void VirtualView::resizeVirtualView(){
    this->resize(this->parentWidget()->size());
    this->fitInView(scene.sceneRect());
}

//SLOTS
void VirtualView::slot_setVirtualGameMode(bool checked){
    this->virtual_game_mode = checked;
    if (!checked && scene.isActive())
        scene.removeItem(ghost_stone);
}

void VirtualView::mousePressEvent(QMouseEvent* event){
    if (!virtual_game_mode)
        return;

    if (event->button() == Qt::LeftButton && setting_stone_valid){
        // the mouse_hover_coord is calculated starting from upper left corner
        // game board starts at left bottom corner -> mirror vertically
        int ycoord = board_size - mouse_hover_coord.y() + 1;    
        emit signal_virtualViewplayMove(mouse_hover_coord.x(), ycoord);
        setting_stone_valid = false;
    }
}

void VirtualView::mouseMoveEvent(QMouseEvent* event){
    if (!virtual_game_mode)
        return;

    int pic_boarder_x = 60, pic_boarder_y = 55, board_pix; 

    switch(this->board_size){
    case 9: 
        board_pix = this->board_image_size9.size().width();
        break;
    case 13:
        board_pix = this->board_image_size13.size().width();
        break;
    case 19:
        board_pix = this->board_image_size19.size().width();
        break;
    default:
        return;
        break;
    }

    // calculate the scale of picture in viewport
    qreal scale_x = scene.sceneRect().size().width() * qreal(1.0)/board_pix;
    qreal scale_y = scene.sceneRect().size().height() * qreal(1.0)/board_pix;

    // getting transformation of scene (fitInView() scales the scene!)
    qreal transform_x = qreal(1.0) / this->transform().m11();
    qreal transform_y = qreal(1.0) / this->transform().m22();
    
    // mapping mousecoordinates to gameboard coordinates (example: x=0,2515, y=2,7622)
    qreal xCoordAccurate = (event->pos().x() * transform_x - pic_boarder_x * scale_x) / (this->cell_width * scale_x);
    qreal yCoordAccurate = (event->pos().y() * transform_y - pic_boarder_y * scale_y) / (this->cell_height * scale_y);
    
    // Rounding half up  (example: x=0, y=3)
    int board_x_coord = static_cast<int>(xCoordAccurate);
    int board_y_coord = static_cast<int>(yCoordAccurate);
    board_x_coord = xCoordAccurate - board_x_coord < 0.5f ? board_x_coord : board_x_coord + 1; 
    board_y_coord = yCoordAccurate - board_y_coord < 0.5f ? board_y_coord : board_y_coord + 1; 

    // Until now we calculated from 0,0. Game starts counting at 1,1 
    QPoint new_mouse_hover_coord = QPoint(board_x_coord + 1,board_y_coord + 1);
    
    // If mouse hover coordinates differ -> move ellipse to new coordinates
    if (new_mouse_hover_coord != mouse_hover_coord
        && board_x_coord < board_size && board_y_coord < board_size){

        // calculate exact position of where to draw new ellipse
        qreal ellipse_xPos = (pic_boarder_x * scale_x) + (board_x_coord * this->cell_width * scale_x) - (this->cell_width * scale_x)/2.0f;
        qreal ellipse_yPos = (pic_boarder_y * scale_y) + (board_y_coord * this->cell_height * scale_y) - (this->cell_height * scale_y)/2.0f;

        QRectF new_selection_ellipse = QRectF(ellipse_xPos, ellipse_yPos, this->cell_width * scale_x, this->cell_height * scale_y);
        if (ghost_stone == nullptr)
            this->ghost_stone = new QGraphicsEllipseItem(QRectF());
        ghost_stone->setRect(new_selection_ellipse);
        selection_ellipse = new_selection_ellipse;

        // save new mouse hover coordinates
        mouse_hover_coord = new_mouse_hover_coord;
    }
}
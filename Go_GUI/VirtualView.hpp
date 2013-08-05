#pragma once
#include <QApplication>
#include <QWidget>
#include <QGraphicsView>

class GoBoard;


class QGLSceneNode;

class VirtualView : public QGraphicsView
{
    Q_OBJECT
public:
    VirtualView(QWidget *parent = 0);
    ~VirtualView();

    void resizeEvent(QResizeEvent* event){
        this->resize(this->parentWidget()->size());
        this->fitInView(scene.sceneRect());
    }
    void createAndSetScene(QSize size, const GoBoard * game_board = nullptr);

signals:
    // sends a signal with game board coordinates where to set a new stone
    void signal_virtualViewplayMove(const int coord_x, const int coord_y);

private:
    int board_size;
    QGraphicsScene scene;
    QImage board_image_size9, board_image_size13, board_image_size19,
        black_stone_image, white_stone_image;

    void mousePressEvent(QMouseEvent *event);
    void VirtualView::mouseMoveEvent(QMouseEvent* event);
    bool setting_stone_valid;
    float cell_width, cell_height;
    QPoint mouse_hover_coord;
    QRectF selection_ellipse;
    QGraphicsEllipseItem* ghost_stone;
};

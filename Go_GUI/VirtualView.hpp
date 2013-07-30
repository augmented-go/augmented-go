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
    void createAndSetScene(QSize size, const GoBoard * game_board = nullptr);

private:
    QGraphicsScene scene;
    QImage board_image_size9, board_image_size13, board_image_size19,
        black_stone_image, white_stone_image;
};

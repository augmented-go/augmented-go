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

};

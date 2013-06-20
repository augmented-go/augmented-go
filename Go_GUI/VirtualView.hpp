#pragma once
#include <QApplication>
#include <QGLView>

class VirtualView : public QGLView
{
    Q_OBJECT
public:
    VirtualView(QWidget *parent = 0){
    }
    void initializeGL(){}
    void paintGL(){}
    ~VirtualView(){};
};

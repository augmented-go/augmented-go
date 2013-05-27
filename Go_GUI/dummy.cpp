//! [class-defn]
#include <memory>

#include <QApplication>
#include <QGuiApplication>

#include "QtWidgets/qwidget.h"

#include "Qt3D/qglview.h"
#include "Qt3D/qglscenenode.h"
#include "Qt3D/qglbuilder.h"
#include "Qt3D/qglteapot.h"

class TeapotView : public QGLView
{
    Q_OBJECT
public:
    TeapotView(QWindow *parent = 0)
        : QGLView(parent),
        teapot(nullptr)
    {}
    ~TeapotView();

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

private:
    std::unique_ptr<QGLSceneNode> teapot;
};
//! [class-defn]

//! [initialize]
void TeapotView::initializeGL(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::LitMaterial);
//! [build-geometry]
    QGLBuilder builder;
    builder << QGLTeapot();
    auto scene = builder.finalizedSceneNode();
    teapot.reset(scene);
}
//! [initialize]

//! [destructor]
TeapotView::~TeapotView()
{
}
//! [destructor]

//! [paint]
void TeapotView::paintGL(QGLPainter *painter)
{
    teapot->draw(painter);
}
//! [paint]

int mainQ3D(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    TeapotView view;

    view.setTitle("Pure Q3D!");
    view.resize(800, 600);
    view.show();

    return app.exec();
}

int mainQ3DWidget(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TeapotView view;

    // ownership is still at view! so don't delete the 'teapot' pointer!
    QWidget* teapot(QWidget::createWindowContainer(&view));
    teapot->setWindowTitle("Q3D as a QWidget!");
    teapot->resize(800, 600);
    teapot->show();

    return app.exec();
}

//! [main]
int main(int argc, char *argv[]) {
    //return mainQ3DWidget(argc, argv);
    return mainQ3D(argc, argv);
}
//! [main]

#include "dummy.moc"
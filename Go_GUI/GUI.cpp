#include "GUI.hpp"

#include <Qt3D/qglview.h>
#include "Qt3D/qglbuilder.h"
#include "VirtualView.hpp"


namespace Go_GUI {

void GUI::init(){
	this->setWindowTitle("Augmented Go");
	QWidget* central = this->centralWidget();
	
	
	QGridLayout* view_big = central->findChild<QGridLayout *>("view_big");
	QGridLayout* view_small = central->findChild<QGridLayout *>("view_small");
	
	QWidget* virtual_view = QWidget::createWindowContainer(new VirtualView());
	QWidget* augmented_view = QWidget::createWindowContainer(new VirtualView());

	view_big->addWidget(virtual_view);
	view_small->addWidget(augmented_view);
}

void GUI::RenderGame(GoBackend::Game game) {
    // blaa
	//virtual_view->show();
	this->show(); // shows Qt5 Window
}

} // namespace Go_GUI
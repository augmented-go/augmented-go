#include <stdio.h>
#include "GUI.hpp"
#include "qapplication"

 

int main( int argc, const char* argv[] ){
	Q_INIT_RESOURCE(application);

	QApplication a(argc, argv);
	Go_GUI::GUI gui;

	return a.exec();
}
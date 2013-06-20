#pragma once
#include <QApplication>
#include <qlabel.h>
#include <QFile>
#include <QMessageBox>
#include <QPicture>
#include "qwidget.h"
#include "qglview.h"
#include "qglbuilder.h"

class QGLSceneNode;

class AugmentedView : public QLabel
{
    Q_OBJECT
public:
	AugmentedView(QWidget *parent = 0) {
		this->setParent(parent);

		// loading in testpicture
		inputImage = QImage("../Go_GUI/textures/example_pic.jpg");
		if (inputImage.isNull())
			QMessageBox::warning(this, "file loading error", "could not load file!");
		printf("%d", inputImage.format());
		uchar* image_data = inputImage.bits();
		setImage(image_data, inputImage.width(), inputImage.height());

	}
	~AugmentedView(){};

	//QImage: Fast to draw to, slow to display.
	//QPixmap: Slow to draw to, fast to display.
	void setImage(const uchar* pData, int width, int height){
		image = QImage(pData, width, height, QImage::Format_ARGB32_Premultiplied);	// augmented picture : QImage::Format_ARGB32_Premultiplied	
		picture = QPixmap::fromImage(image);//.scaled(this->size());
		//picture.loadFromData(pData, 8*width*height, "JPG");

		this->setPixmap(picture);
	}

	/**
	 * @brief		scales the pixmap to the width of size
	 * @parameter	QSize
	 */
	void rescaleImage(QSize size){
		setPixmap(picture.scaledToWidth(size.width()));
	}

private:
	QPixmap picture;
	QImage image, inputImage;

};
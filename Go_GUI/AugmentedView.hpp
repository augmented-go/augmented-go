// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>

class AugmentedView : public QLabel
{
    Q_OBJECT
public:

    /**
     * @brief	Constructor
     *          Sets its parent widget,
     *          Sets the alignment of its content to center
     *          Sets up the Augmented Logo as its standard picture it shows and resizes it
     */
    AugmentedView(QWidget *parent = 0) {
        this->setParent(parent);
        this->setAlignment(Qt::AlignCenter);

        // Setting up augmented logo as standard picture
        QString augmented_logo_path = "res/textures/augmented_logo.png";
        if(QImage(augmented_logo_path).isNull())
            QMessageBox::critical(this, "GUI element not found", "Warning image of AugmentedView not found!\n searched relative to exe in" + augmented_logo_path);
        else{
            setImage(QImage(augmented_logo_path));
            rescaleImage(parentWidget()->size());
        }
    }
    ~AugmentedView(){};

    /**
     * @brief		Sets the new image that shall be displayed.
     *				If the new image is empty, show the same picture as before.
     *				Default picture is "no_camera_picture.png".
     * @parameter	QImage	image that shall be displayed. 
     */
    void setImage(QImage image){
        if(!image.isNull())		// if image is empty take old picture!
            picture = QPixmap::fromImage(image);

        this->setPixmap(picture);
        this->show();
    }

    /**
     * @brief		scales the pixmap to the width of size
     * @parameter	QSize
     */
    void rescaleImage(QSize size){
        this->resize(size);
        if (!picture.isNull())
            setPixmap(picture.scaled(size, Qt::KeepAspectRatio));
    }

private:
    QPixmap picture;
};
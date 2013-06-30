#pragma once
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>

class AugmentedView : public QLabel
{
    Q_OBJECT
public:
    AugmentedView(QWidget *parent = 0) {
        this->setParent(parent);

        // Setting up warning as standard picture
        QString warning_image_path = "res/textures/No_camera_picture.png";
        if(QImage(warning_image_path).isNull())
            QMessageBox::critical(this, "GUI element not found", "Warning image of AugmentedView not found!\n searched relative to exe in" + warning_image_path);
        else{
            setImage(QImage(warning_image_path));
        }
    }
    ~AugmentedView(){};

    /**
     * @brief		scales the pixmap to the width of size
     * @parameter	QSize
     */
    void setImage(QImage image){
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
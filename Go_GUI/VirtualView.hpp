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

    /**
     * @brief   Sets several settings:
     *              disabling scrollbars
     *              enabling mousetracking
     *              texture paths
     */
    VirtualView(QWidget *parent = 0);
    ~VirtualView();

    /**
     * @brief   override
     *          calls public function resizeVirtualView
     */
    void resizeEvent(QResizeEvent* event){
        this->resizeVirtualView();
    }

    /**
     * @brief	Creates the virtual board of the go game and set the scene
     * @param	QSize	size of the container	
     */
    void createAndSetScene(QSize size, const GoBoard * game_board = nullptr);

    /**
     * @brief   resizes the scene of virtual view to the container it is embedded to.
     */
    void resizeVirtualView();

signals:
    // sends a signal with game board coordinates where to set a new stone
    void signal_virtualViewplayMove(const int coord_x, const int coord_y);

public slots:
    /*
     * @brief   sets the size of this widget and uses fitInView() to scale scene to correct size
     *          IMPORTANT: fitInView() changes transformation matrix of scene!
     */
    void slot_setVirtualGameMode(bool checked);

private:
    bool virtual_game_mode;

    int board_size;
    QGraphicsScene scene;
    QImage board_image_size9, board_image_size13, board_image_size19,
        black_stone_image, white_stone_image;

    /**
     * @brief   override
     *          checks for mousebuttons.
     *          If a left mousebutton was pressed, the current board_position that
     *          is currently pointed at, gets send to backend to play a move.
     *          Only in virtual game mode!
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief   If in virtual game mode, the position the mouse hovers over,
     *          gets calculated into game board coordinates and a transparent
     *          circle is shown on that position.
     *          Only if the position changes to another board coordinate
     *          that circle changes its position.
     */
    void mouseMoveEvent(QMouseEvent* event);
    
    bool setting_stone_valid;
    qreal cell_width, cell_height;
    QPoint mouse_hover_coord;
    QRectF selection_ellipse;
    QGraphicsEllipseItem* ghost_stone;
};

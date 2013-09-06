// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once
#include <QApplication>
#include <QWidget>
#include <QGraphicsView>
#include <GoBoard.h>

class GoBoard;
class QGLSceneNode;

/**
 * @class   VirtualView
 * @brief   The VirtualView is part of Augmented-Go Gui.
 *          The virtual view shows a virtual representation of the game.
 *          Red circles represent differences of the game on the camera picture
 *          and the virtual board.
 *          If the game is in virtual mode, the user can place stones by clicking on the board.
 */
class VirtualView : public QGraphicsView
{
    Q_OBJECT
public:

    /**
     * @brief   Sets several settings:
     *              disabling scrollbars
     *              enabling mousetracking
     *              texture paths
     * @param   QWidget     parent widget
     */
    VirtualView(QWidget *parent = 0);
    ~VirtualView();

    /**
     * @brief   override
     *          calls public function resizeVirtualView
     * @param   QResizeEvent    event when widget is resized
     */
    void resizeEvent(QResizeEvent* event){
        this->resizeVirtualView();
    }

    /**
     * @brief   Creates the virtual board of the go game and set the scene
     * @param   QSize       size of the container
     * @param   SgPointSet  set of all differences between real board and virtual board
     * @param   GoBoard     current go game board
     */
    void createAndSetScene(QSize size, SgPointSet difference_points, const GoBoard * game_board = nullptr);

    /*
     * @brief   Sets the size of this widget and uses fitInView() to scale scene to correct size
     *          IMPORTANT: fitInView() changes transformation matrix of scene!
     */
    void resizeVirtualView();

signals:
    /** 
     * @brief   Sends a signal with game board coordinates where to set a new stone
     *          Should only to be used in virtual game mode
     * @param   int coord_x     x coordinate of new stone
     * @param   int coord_y     y coordinate of new stone
     */
    void signal_virtualViewplayMove(const int coord_x, const int coord_y);

public slots:
    /**
     * @brief   Sets the virtual view into virtual game mode.
     * @param   bool
     */
    void slot_setVirtualGameMode(bool checked);

private:
    bool virtual_game_mode;

    int board_size;
    QGraphicsScene scene;
    QImage board_image_size9, board_image_size13, board_image_size19,
        black_stone_image, white_stone_image, illegal_stone_image;

    bool setting_stone_valid;
    /** dimensions of cells when scene was created*/
    qreal cell_width, cell_height;
    /** coordinate of mouse */
    QPoint mouse_hover_coord;
    /** dimensions of ellipse of ghost_stone*/
    QRectF selection_ellipse;
    /** stone that appears when hovering over virtual board in virtual game mode*/
    QGraphicsEllipseItem* ghost_stone;

    /**
     * @brief   override
     *          Checks for mousebuttons.
     *          If a left mousebutton was pressed, the current board_position that
     *          is currently pointed at, gets send to backend to play a move.
     *          Only in virtual game mode!
     * @param   QMouseEvent     event triggered by mouse
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief   If in virtual game mode, the position the mouse hovers over,
     *          gets calculated into game board coordinates and a transparent
     *          circle is shown on that position.
     *          Only if the position changes to another board coordinate
     *          that circle changes its position.
     * @param   QMouseEvent     event triggered by mouse
     */
    void mouseMoveEvent(QMouseEvent* event);
};

// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include <QDialog>

#include "ui_ChangeScanRateDialog.h"
#include "GUI.hpp"

namespace Go_GUI {

class ChangeScanRateDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeScanRateDialog(Go_GUI::GUI *parent, int current_scan_rate_fps){
       ui_changescanrate.setupUi(this);
       
       connect(this, &ChangeScanRateDialog::signal_change_scan_rate, parent, &Go_GUI::GUI::slot_changeScanRate);
       connect(ui_changescanrate.scan_rate_slider, &QAbstractSlider::valueChanged, this, &ChangeScanRateDialog::valueChanged);

       valueChanged(current_scan_rate_fps);
    };
    ~ChangeScanRateDialog(){};

    /**
     * @brief      overides the "OK"-Button of the dialog.
     *             If OK is pressed, the new scanning rate is sent to the main GUI.
     */
    void accept(){
        emit signal_change_scan_rate(ui_changescanrate.scan_rate_slider->value());
        close();
    }

    void valueChanged(int new_value) {
        ui_changescanrate.scan_rate_label->setText(QString("Scanning rate: %1 fps").arg(new_value));
        ui_changescanrate.scan_rate_slider->setValue(new_value);
    }

signals:
    void signal_change_scan_rate(int fps);

private:
    Ui::changeScanRate ui_changescanrate;
};

}
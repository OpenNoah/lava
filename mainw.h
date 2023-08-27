#pragma once

#include <qt.h>
#include "lava_widget.h"

class MainW: public QMainWindow
{
    Q_OBJECT

public:
    MainW(QWidget *parent = 0);

public slots:
    void fullscreen();
    void exit_fullscreen();
    void switch_colour();

private:
    LavaWidget *lavaw;
};

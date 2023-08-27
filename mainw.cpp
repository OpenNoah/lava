#include "mainw.h"

MainW::MainW(QWidget *parent) : QMainWindow(parent, 0, Qt::WStyle_Customize | Qt::WStyle_NoBorderEx)
{
    setCaption("Lava Qt2");

    setCentralWidget(lavaw = new LavaWidget(this));

    QMenuBar *menu = menuBar();
    int mitem;
    //mitem = menu->insertItem(tr("切换颜色"));
    //menu->connectItem(mitem, this, SLOT(switch_colour()));
    mitem = menu->insertItem(tr("全屏显示"));
    menu->connectItem(mitem, this, SLOT(fullscreen()));
    mitem = menu->insertItem(tr("退出程序"));
    menu->connectItem(mitem, this, SLOT(close()));

    connect(lavaw, SIGNAL(touched()), this, SLOT(fullscreen()));
}

void MainW::fullscreen()
{
    lavaw->setBackgroundColor(Qt::black);
    lavaw->reparent(0, Qt::WStyle_Customize | Qt::WStyle_NoBorderEx, QPoint(0, 0), false);
    lavaw->showFullScreen();
    disconnect(lavaw, SIGNAL(touched()), this, SLOT(fullscreen()));
    connect(lavaw, SIGNAL(touched()), this, SLOT(exit_fullscreen()));
}

void MainW::exit_fullscreen()
{
    lavaw->setBackgroundColor(backgroundColor());
    setCentralWidget(lavaw);
    disconnect(lavaw, SIGNAL(touched()), this, SLOT(exit_fullscreen()));
    connect(lavaw, SIGNAL(touched()), this, SLOT(fullscreen()));
}

void MainW::switch_colour()
{
    lavaw->set_colours(Qt::green, Qt::black);
}

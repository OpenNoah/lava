#pragma once

#include <qt.h>

class LavaThread;

class LavaWidget: public QWidget
{
    Q_OBJECT

public:
    LavaWidget(QWidget *parent = 0);
    ~LavaWidget();

    void set_colours(QColor fg, QColor bg);

signals:
    void touched();

protected:
    virtual void customEvent(QCustomEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);

private:
    LavaThread *th_lava;
};

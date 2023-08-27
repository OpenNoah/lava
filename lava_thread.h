#pragma once

#include <stdint.h>
#include <qt.h>
#include <queue>

enum {
    EventExit = QEvent::User + 0,
};

class LavaThread: public QThread
{
public:
    explicit LavaThread(QWidget *parent);
    ~LavaThread();

    virtual void run();

    // To be called from GUI thread
    void terminate();
    QImage fb();
	void set_colours(QRgb fg, QRgb bg);
    void keyPress(QKeyEvent *e);
    void keyRelease(QKeyEvent *e);

    // For LAVA runtime
    void lava_init(int w, int h);
    void lava_exit(int v);

    void lava_delay(int ms);
    void lava_refresh(uint8_t *pfb);

    char lava_keycode(Qt::Key qkey);
    char lava_getchar();
    char lava_check_key(uint8_t key);
    char lava_in_key();
    void lava_release_key(uint8_t key);

    const QRgb &colour_bg() {return clr_bg;}
    const QRgb &colour_fg() {return clr_fg;}

private:
    QWidget *parent;
    QRgb clr_bg;
    QRgb clr_fg;
    QSize scn_size;
    QMutex img_mtx;
    QImage img;
    int rc;
    bool is_terminate;

    QSemaphore *key_sm;
    QMutex key_mtx;
    QMap<Qt::Key, bool> key_state;
    QMap<Qt::Key, QTime> key_released;
    std::queue<Qt::Key> key_seq;
    QMap<Qt::Key, char> key_map;
};

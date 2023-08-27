#include <stdio.h>
#include "lava_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void lava_main();

#ifdef __cplusplus
}
#endif

static bool lava_init();

static LavaThread *th;

LavaThread::LavaThread(QWidget *parent): parent(parent), rc(-1), QThread()
{
    key_sm = new QSemaphore(1);
    *key_sm += key_sm->available();
    set_colours(qRgb(255, 255, 255), qRgb(64, 64, 64));
}

LavaThread::~LavaThread()
{
    delete key_sm;
}

void LavaThread::run()
{
    th = this;
    if (!::lava_init())
        return;
    ::lava_main();
    lava_exit(0);
}

void LavaThread::terminate()
{
    // Note this is called from GUI thread
    is_terminate = true;
    // Notify to LAVA thread, in case it is waiting on key events
    (*key_sm)--;
}

QImage LavaThread::fb()
{
    // Note this is called from GUI thread
    img_mtx.lock();
    QImage ret_img = img;
    img_mtx.unlock();
    return ret_img;
}

void LavaThread::set_colours(QRgb fg, QRgb bg)
{
    // Note this is called from GUI thread
    clr_fg = fg;
    clr_bg = bg;
}

void LavaThread::keyPress(QKeyEvent *e)
{
    // Note this is called from GUI thread

    //fprintf(stderr, QObject::tr(QString("%0: %3, %1: %2\n").arg(__func__).arg(e->key()).arg(e->text()).arg(e->isAutoRepeat())));

    if (e->isAutoRepeat()) {
        // Try to ignore auto repeat events (doesn't always work)
        e->accept();
        return;
    }

    Qt::Key qkey = (Qt::Key)e->key();

    key_mtx.lock();
    key_state[qkey] = true;

#if 1
    // Workaround for auto-repeat issues
    // If key is pressed again within delta_ms, ignore
    const int delta_ms = 32;
    QMap<Qt::Key, QTime>::Iterator it = key_released.find(qkey);
    if (it == key_released.end() || it.data().msecsTo(QTime::currentTime()) >= delta_ms)
        key_seq.push(qkey);
    if (it != key_released.end()) {
        //fprintf(stderr, QObject::tr("%0: %1\n").arg(__func__).arg(it.data().msecsTo(QTime::currentTime())));
        key_released.remove(it);
    }
#else
    key_seq.push(qkey);
#endif

    key_mtx.unlock();
    e->accept();

    // Notify to LAVA thread
    (*key_sm)--;
}

void LavaThread::keyRelease(QKeyEvent *e)
{
    // Note this is called from GUI thread

    if (e->isAutoRepeat()) {
        // Try to ignore auto repeat events (doesn't always work)
        e->accept();
        return;
    }

    //fprintf(stderr, QObject::tr(QString("%0: %1, %2\n").arg(__func__).arg(e->key()).arg(e->text())));

    Qt::Key qkey = (Qt::Key)e->key();

    key_mtx.lock();
    key_state.remove(qkey);
    key_released[qkey] = QTime::currentTime();
    key_mtx.unlock();
    e->accept();

    // Notify to LAVA thread
    (*key_sm)--;
}

void LavaThread::lava_init(int w, int h)
{
    scn_size = QSize(w, h);
    img.create(scn_size, 32);
}

void LavaThread::lava_exit(int v)
{
    fprintf(stderr, "%s: %d\n", __PRETTY_FUNCTION__, v);
    rc = v;
    if (!is_terminate)
        postEvent(parent, new QCustomEvent(EventExit));
    exit();
}

void LavaThread::lava_delay(int ms)
{
    if (is_terminate)
        lava_exit(-1);
    msleep(ms);
}

void LavaThread::lava_refresh(uint8_t *pfb)
{
    if (is_terminate)
        lava_exit(-1);

    QImage new_img(img.size(), img.depth());
    const int w = new_img.width();
    const int h = new_img.height();

    uint8_t *p = pfb;
    for (int y = 0; y < h; y++) {
        QRgb *prgb = (QRgb *)new_img.scanLine(y);
        for (int x = 0; x < w; x++) {
            prgb[x] = qRgb(p[0], p[1], p[2]);
            p += 3;
        }
    }

    img_mtx.lock();
    img = new_img;
    img_mtx.unlock();
    postEvent(parent, new QPaintEvent(parent->rect()));
}

char LavaThread::lava_keycode(Qt::Key qkey)
{
    // Convert from Qt key code to LAVA key code
    if (qkey == (Qt::Key)0)
        return 0;

    enum {
        KEY_UP    = 20,
        KEY_DOWN  = 21,
        KEY_RIGHT = 22,
        KEY_LEFT  = 23,
        KEY_F1    = 28,
        KEY_F2    = 29,
        KEY_ENTER = 13,
        KEY_SPACE = 20,
        KEY_HELP  = 25,
        KEY_ESC   = 27,
    };

    if (key_map.isEmpty()) {
        // Initialisation
        key_map[Qt::Key_Up]      = KEY_UP;
        key_map[Qt::Key_Down]    = KEY_DOWN;
        key_map[Qt::Key_Left]    = KEY_LEFT;
        key_map[Qt::Key_Right]   = KEY_RIGHT;
        key_map[Qt::Key_F1]      = KEY_F1;
        key_map[Qt::Key_F2]      = KEY_F2;
        key_map[Qt::Key_Return]  = KEY_ENTER;
        key_map[Qt::Key_Enter]   = KEY_ENTER;
        key_map[Qt::Key_F5]      = KEY_HELP;
        key_map[Qt::Key_Escape]  = KEY_ESC;

        key_map[Qt::Key_Space]   = KEY_SPACE;
        for (int v = Qt::Key_A; v <= Qt::Key_Z; v++)
            key_map[(Qt::Key)v] = 'a' + v - Qt::Key_A;
        for (int v = Qt::Key_0; v <= Qt::Key_9; v++)
            key_map[(Qt::Key)v] = '0' + v - Qt::Key_0;
    }

    const QMap<Qt::Key, char>::ConstIterator it = key_map.find(qkey);
    if (it == key_map.end()) {
        fprintf(stderr, "%s: Unknown key 0x%04x\n", __PRETTY_FUNCTION__, (int)qkey);
        return 0;
    }
    //fprintf(stderr, "%s: key 0x%04x -> %d\n", __PRETTY_FUNCTION__, key, (int)*it);
    return *it;
}

char LavaThread::lava_getchar()
{
    // This function read key buffer, blocking
    //fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    int qkey = 0;
    for (;;) {
        // Wait for a key press event
        while (key_seq.empty()) {
            (*key_sm)++;
            if (is_terminate)
                lava_exit(-1);
        }

        key_mtx.lock();
        qkey = key_seq.front();
        key_seq.pop();
        key_mtx.unlock();
        break;
    }
    return th->lava_keycode((Qt::Key)qkey);
}

char LavaThread::lava_check_key(uint8_t key)
{
    // This function read the current pressed key status
    //fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    if (is_terminate)
        lava_exit(-1);

    // Check current key pressed status
    key_mtx.lock();
    if (!key_state.isEmpty()) {
        if (key >= 128) {
            // Check for any pressed keys
            Qt::Key qkey = key_state.begin().key();
            key_mtx.unlock();
            return th->lava_keycode(qkey);
        } else {
            QMap<Qt::Key, bool>::ConstIterator it;
            for (it = key_state.begin(); it != key_state.end(); it++) {
                if (key == th->lava_keycode(it.key())) {
                    key_mtx.unlock();
                    return key;
                }
            }
        }
    }
    key_mtx.unlock();
    return 0;
}

char LavaThread::lava_in_key()
{
    // This function read key buffer, non-blocking
    //fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    if (is_terminate)
        lava_exit(-1);

    // Check and read any pending pressed key sequence
    key_mtx.lock();
    Qt::Key qkey = (Qt::Key)0;
    if (!key_seq.empty()) {
        qkey = key_seq.front();
        key_seq.pop();
    }
    key_mtx.unlock();
    return th->lava_keycode(qkey);
}

void LavaThread::lava_release_key(uint8_t key)
{
    // This function clears certain keys in key buffer
    //fprintf(stderr, "%s: %d\n", __PRETTY_FUNCTION__, (int)key);
    if (key >= 128) {
        key_mtx.lock();
        key_seq = std::queue<Qt::Key>();
        key_mtx.unlock();
    } else {
        fprintf(stderr, "%s: Unknown %d\n", __PRETTY_FUNCTION__, (int)key);
    }
}

#include "stdint.h"
// This stupid file redefines int
#include "lava.h"
#include "font12.h"

static uint8_t fb[LCD_H][LCD_W][3];
static QByteArray font12;

bool lava_init()
{
    if (font12.isEmpty()) {
        QFile f("font12.bin");
        if (!f.open(IO_ReadOnly)) {
            fprintf(stderr, "%s: Font file not found\n", __func__);
            return false;
        }
        font12 = f.readAll();
        f.close();
    }

    th->lava_init(LCD_W, LCD_H);
    ClearScreen();
    Refresh();
    return true;
}

void lava_exit(short v)
{
    th->lava_exit(v);
}

void Delay(short ms)
{
    th->lava_delay(ms);
}

// Graphs

enum {
    GraphicTypeBackground = 0,
    GraphicTypeForeground = 1,
    GraphicTypeInvert     = 2,
};

static inline void copy_rgb(uint8_t *pfb, QRgb rgb)
{
    pfb[0] = qRed(rgb);
    pfb[1] = qGreen(rgb);
    pfb[2] = qBlue(rgb);
}

void ClearScreen()
{
    QRgb c = th->colour_bg();
    for (int y = 0; y < LCD_H; y++)
        for (int x = 0; x < LCD_W; x++)
            copy_rgb(&fb[y][x][0], c);
}

void Refresh()
{
    th->lava_refresh(&fb[0][0][0]);
}

void BlockGX(short x0, short y0, short x1, short y1, short type)
{
    switch (type) {
    case GraphicTypeBackground:
    case GraphicTypeForeground: {
            QRgb c = type == 1 ? th->colour_fg() : th->colour_bg();
            for (short y = y0; y <= y1; y++)
                for (short x = x0; x <= x1; x++)
                    copy_rgb(&fb[y][x][0], c);
        }
        break;
    case GraphicTypeInvert: {
            for (short y = y0; y <= y1; y++) {
                for (short x = x0; x <= x1; x++) {
                    QRgb c = qRgb(fb[y][x][0], fb[y][x][1], fb[y][x][2]);
                    c = c == th->colour_fg() ? th->colour_bg() : th->colour_fg();
                    copy_rgb(&fb[y][x][0], c);
                }
            }
        }
        break;
    default:
        fprintf(stderr, "%s: Unknown type: %d\n", __func__, (long)type);
        break;
    }
}

void RectangleGX(short x0, short y0, short x1, short y1, short type)
{
    BlockGX(x0, y0, x1, y0, type);
    BlockGX(x0, y1, x1, y1, type);
    BlockGX(x0, y0, x0, y1, type);
    BlockGX(x1, y0, x1, y1, type);
}

void WriteBlockGX(short x0, short y0, short w, short h, short type, const char* bmp)
{
    switch (type) {
    case GraphicTypeBackground:
    case GraphicTypeForeground: {
            QRgb fg = type == 0 ? th->colour_bg() : th->colour_fg();
            QRgb bg = type == 0 ? th->colour_fg() : th->colour_bg();
            uint32_t ofs = 0;
            for (short y = 0; y < h; y++) {
                for (short x = 0; x < w; x++) {
                    QRgb c = bmp[ofs / 8] & (0x80 >> (ofs % 8)) ? fg : bg;
                    copy_rgb(&fb[y0 + y][x0 + x][0], c);
                    ofs++;
                }
                ofs += (8 - (ofs % 8)) % 8;
            }
        }
        break;
    default:
        fprintf(stderr, "%s: Unknown type: %d: %d,%d %d,%d\n", __func__,
                (long)type, (long)x0, (long)y0, (long)w, (long)h);
        return;
    }
}

void TextOut_chs12(short x, short y, const char* str, short type)
{
    switch (type) {
    case GraphicTypeBackground:
    case GraphicTypeForeground:
        break;
    default:
        fprintf(stderr, "%s: Unknown type: %d\n", __func__, (long)type);
        return;
    }

    short num, x0;
    u32 pos;
    u8 ch, fw, fh;
    u16* buf;
    u16 _buf[16];

    fw = 6; fh = 12;
    buf = &_buf[0];

    x0 = x;
    while (x < 1024 /*_lcd_w*/) {
        num = 0;
        ch = *str++;
        if (ch == 0)
            break;
#if _USE_AUTOWRAP == 1
        if (ch == '\r')
        {
            continue;
        }
        if (ch == '\n')
        {
            x = x0;
            y += fh;
            continue;
        }
#else
        if (false /*is_printf*/) {
            if (ch == '\r')
                continue;
            if (ch == '\n') {
                x = 0;
                y += fh;
                continue;
            }
        }
#endif
        if (ch & 0x80) {
            // 中文字体
            pos = (ch - (ch >= 0xB0 ? 0xA7 : 0xA1)) * 0x5E + *str++ - 0xA1;
            pos *= 22;
            if (pos < 167508) {
                memcpy((void *)buf, (void *)(font12.data() + pos), 22);
                buf[11] = 0x0000;
#if USE_GRAY_MODE != 0
                if (1 /*_lava_graph_mode*/ != 1) {
                    //draw_font(x, y, fh, fh, type, (u8*)_buf);
                } else
#endif
                {
                    WriteBlock(x, y, fh, fh, type, (u8*)_buf);
                }
                x += fw * 2;
                continue;
            }
            ch = '?';
            num = 2;
        }
        do
        {
            // 英文字体
            if (ch < 32) ch = 32;
            pos = 167508 + (ch - 32) * 12;
            memcpy((void *)buf, (void *)(font12.data() + pos), 12);
            buf[6] = 0x0000;
#if USE_GRAY_MODE != 0
            if (1 /*_lava_graph_mode*/ != 1) {
                //draw_font(x, y, fw, fh, type, (u8*)_buf + ((type & DRAW_FONT_L) ? 2 : 0));
            } else
#endif
            {
                WriteBlock(x, y, fw, fh, type, (u8*)_buf + ((type & DRAW_FONT_L) ? 2 : 0));
            }
            x += fw;
        } while (--num > 0);
    }
}

// Input & output

short CheckKey(char key)
{
    return th->lava_check_key(key);
}

char Inkey(void)
{
    return th->lava_in_key();
}

void ReleaseKey(char key)
{
    th->lava_release_key(key);
}

char lava_getchar(void)
{
    return th->lava_getchar();
}

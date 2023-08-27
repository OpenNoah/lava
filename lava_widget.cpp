#include "lava_widget.h"
#include "lava_thread.h"

LavaWidget::LavaWidget(QWidget *parent) : QWidget(parent, 0, Qt::WStyle_Customize | Qt::WStyle_NoBorderEx)
{
	setFocusPolicy(QWidget::StrongFocus);
	setKeyCompression(false);
	th_lava = new LavaThread(this);
    th_lava->set_colours(qRgb(0, 0, 0), qRgb(240, 240, 240));
	th_lava->start();
}

LavaWidget::~LavaWidget()
{
	th_lava->terminate();
	th_lava->wait();
}

void LavaWidget::set_colours(QColor fg, QColor bg)
{
	th_lava->set_colours(fg.rgb(), bg.rgb());
}

void LavaWidget::customEvent(QCustomEvent *e)
{
	switch (e->type()) {
	case EventExit:
		qApp->quit();
		break;
	default:
		QMessageBox::warning(this, tr("Warning"), tr("Unknown event: %1").arg((int)e->type()));
	}
}

void LavaWidget::keyPressEvent(QKeyEvent *e)
{
	th_lava->keyPress(e);
}

void LavaWidget::keyReleaseEvent(QKeyEvent *e)
{
	th_lava->keyRelease(e);
}

void LavaWidget::mouseReleaseEvent(QMouseEvent *e)
{
	emit touched();
}

void LavaWidget::paintEvent(QPaintEvent *e)
{
	QImage img = th_lava->fb();
	if (img.isNull()) {
		QWidget::paintEvent(e);
		return;
	}

#if 0
	QImage scaled = img;
#else
	const int xscale = size().width() / img.width();
	const int yscale = size().height() / img.height();
	int scale = xscale >= yscale ? yscale : xscale;
	if (scale <= 0)
		scale = 1;
	int w = img.width() * scale;
	int h = img.height() * scale;
	w = w >= width() ? w : width();
	h = h >= height() ? h : height();

	QImage scaled(w, h, img.depth());
	int x0 = (scaled.width() - img.width() * scale) / 2;
	int y0 = (scaled.height() - img.height() * scale) / 2;
	scaled.fill(backgroundColor().rgb());
	for (int y = 0; y < img.height() * scale; y++) {
		for (int x = 0; x < img.width() * scale; x++) {
        	QRgb *pis = (QRgb *)img.scanLine(y / scale);
        	QRgb *pos = (QRgb *)scaled.scanLine(y0 + y);
			pos[x0 + x] = pis[x / scale];
		}
	}
#endif

	QPixmap pix;
	pix = scaled;
	setBackgroundPixmap(pix);
}

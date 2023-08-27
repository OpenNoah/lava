#include <stdlib.h>
#include "mainw.h"
#include "cgucontrol.h"
#include "regs.h"
#include "boot.h"
#include "bootdialog.h"
#include "options.h"
#include "backup.h"

MainW::MainW(QWidget *parent) : QMainWindow(parent, 0, Qt::WStyle_Customize | Qt::WStyle_NoBorderEx)
{
	QTabWidget *tab = new QTabWidget(this);
	setCentralWidget(tab);

	BootDialog::init();
	tab->addTab(new Boot(this), tr("启动项"));

	Regs *regs = new Regs(this);
	if (regs->init())
		tab->addTab(new CGUControl(this), tr("时钟频率"));

	Backup *bkp = new Backup(this);
	tab->addTab(bkp, tr("备份/恢复"));

	tab->addTab(new Options(this), tr("选项"));

	connect(bkp, SIGNAL(enabled(bool)), this, SLOT(setEnabled(bool)));
}

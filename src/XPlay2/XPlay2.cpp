#include "XPlay2.h"
#include <QFileDialog>
#include <QDebug>
#include "XDemuxThread.h"
#include <QMessageBox>

static XDemuxThread dt;

XPlay2::XPlay2(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    dt.Start();
	startTimer(40);
}

XPlay2::~XPlay2()
{
	dt.Close();
}

void XPlay2::timerEvent(QTimerEvent* e)
{
	long long total = dt.totalMs;
	if (total)
	{
		double pos = (double)dt.pts / (double)total;
		int v = ui.playPos->maximum()* pos;
		ui.playPos->setValue(v);
	}
}

void XPlay2::resizeEvent(QResizeEvent* e)
{
	ui.playPos->move(50, this->height() - 100);
	ui.playPos->resize(this->width() - 100, ui.playPos->height());
	ui.pushButton->move(100, this->height() - 150);
	ui.btnPause->move(ui.pushButton->x() + ui.pushButton->width() + 10, ui.pushButton->y());
	ui.video->resize(this->size());
}

void XPlay2::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (isFullScreen())
		this->showNormal();
	else
		this->showFullScreen();
}

void XPlay2::OpenFile()
{
    //选择文件
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
	if (name.isEmpty())return;
	this->setWindowTitle(name);
	if (!dt.Open(name.toLocal8Bit(), ui.video))
	{
		QMessageBox::information(nullptr, "error", "open file failed!");
		return;
	}
}

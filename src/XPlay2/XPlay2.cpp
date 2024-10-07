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
	if (isSliderPressed) return;
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

void XPlay2::setPause(bool isPause)
{
	if (isPause)
	{
		ui.btnPause->setText("play");
	}
	else
	{
		ui.btnPause->setText("pause");
	}
}

void XPlay2::PlayorPause()
{
	bool isPause = !dt.isPause;
	setPause(isPause);
	dt.SetPause(isPause);
}

void XPlay2::sliderPress()
{
	isSliderPressed = true;
}

void XPlay2::sliderRelease()
{
	isSliderPressed = false;
	double pos = 0.0;
	pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();
	dt.Seek(pos);
}

void XPlay2::OpenFile()
{
    //选择文件
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
	if (name.isEmpty())return;
	this->setWindowTitle(name);
	dt.Open(name.toLocal8Bit(), ui.video);
	setPause(dt.isPause);
}

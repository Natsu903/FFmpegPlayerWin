#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "XDemux.h"
#include "XDecode.h"
#include <QThread>

class TestThread :public QThread
{
public:
	void Init()
	{
		//香港卫视
		const char* url = "v1080.mp4";;
		std::cout << "demux.Open = " << demux.Open(url);
		demux.Read();
		demux.Clear();
		demux.Close();
		std::cout << "demux.Open = " << demux.Open(url);
		std::cout << "CopyVPara = " << demux.CopyVPara() << std::endl;
		std::cout << "CopyAPara = " << demux.CopyAPara() << std::endl;
		//cout << "seek=" << demux.Seek(0.95) << endl;

		/////////////////////////////
		std::cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << std::endl;
		//vdecode.Clear();
		//vdecode.Close();
		std::cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << std::endl;
	}
	void run() override
	{
		for (;;)
		{
			AVPacket* pkt = demux.Read();
			if (demux.IsAudio(pkt))
			{
				//adecode.Send(pkt);
				//AVFrame *frame = adecode.Recv();
				//cout << "Audio:" << frame << endl;
			}
			else
			{
				vdecode.Send(pkt);
				AVFrame* frame = vdecode.Recv();
				video->Repaint(frame);
				msleep(40);
				//cout << "Video:" << frame << endl;
			}
			if (!pkt)break;
		}
	}
	///测试XDemux
	XDemux demux;
	///解码测试
	XDecode vdecode;
	XDecode adecode;
	XVideoWidget* video;

};

int main(int argc, char *argv[])
{
	//初始化显示
	/*

	for (;;)
	{
		AVPacket *pkt = demux.Read();
		if (demux.IsAudio(pkt))
		{
			adecode.Send(pkt);
			AVFrame *frame = adecode.Recv();
			//cout << "Audio:" << frame << endl;
		}
		else
		{
			vdecode.Send(pkt);
			AVFrame *frame = vdecode.Recv();
			//cout << "Video:" << frame << endl;
		}
		if (!pkt)break;
	}*/
	TestThread tt;
	tt.Init();

	QApplication a(argc, argv);
	XPlay2 w;
	w.show();

	//初始化gl窗口
	w.ui.video->Init(tt.demux.width, tt.demux.height);
	tt.video = w.ui.video;
	tt.start();
	return a.exec();
}

#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include <QThread>
#include "XAudioPlay.h"
#include "XAudioThread.h"
#include "XVideoThread.h"
#include "XDemuxThread.h"


class TestThread :public QThread
{
public:
	XAudioThread at;
	XVideoThread vt;
	void Init()
	{
		//香港卫视
		const char* url = "v1080.mp4";;
		std::cout << "demux.Open = " << demux.Open(url);
		demux.Read();
		demux.Clear();
		demux.Close();
		std::cout << "demux.Open = " << demux.Open(url);
		std::cout << "at.Open() = " << at.Open(demux.CopyAPara(), demux.sampleRate, demux.channels) << std::endl;
		vt.Open(demux.CopyVPara(), video, demux.width, demux.height);
		at.start();
		vt.start();
	}
	unsigned char* pcm = new unsigned char[1024 * 1024];
	void run() override
	{
		for (;;)
		{
			AVPacket* pkt = demux.Read();
			if (demux.IsAudio(pkt))
			{
				at.Push(pkt);
			}
			else
			{
				vt.Push(pkt);
			}
			if (!pkt)
			{
				demux.Seek(0);
			}
		}
	}
	XDemux demux;
	XVideoWidget* video = nullptr;

};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	XPlay2 w;
	w.show();
	//XDemuxThread dt;
	////const char* testurl = "http://zhibo.hkstv.tv/livestream/mutfysrq/playlist.m3u8";
	//const char* testurl = "v1080p";
	//dt.Open(testurl, w.ui.video);
	//dt.Start();

	return a.exec();
}

#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include <QThread>
#include "XAudioPlay.h"
#include "XAudioThread.h"

class TestThread :public QThread
{
public:
	XAudioThread at;
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
		//std::cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << std::endl;
		//std::cout << "resample.Open() = " << resample.Open(demux.CopyAPara()) << std::endl;
		//XAudioPlay::Get()->channels = demux.channels;
		//XAudioPlay::Get()->sampleRate = demux.sampleRate;
		//std::cout << "XAudioPlay::Get()->Open() = " << XAudioPlay::Get()->Open() << std::endl;
		std::cout << "at.Open() = " << at.Open(demux.CopyAPara(), demux.sampleRate, demux.channels) << std::endl;
		at.start();
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
				/*adecode.Send(pkt);
				AVFrame *frame = adecode.Recv();
				int len = resample.Resample(frame, pcm);
				std::cout << "Resample:" << len << " ";
				while (len > 0)
				{
					if (XAudioPlay::Get()->GetFree() >= len)
					{
						XAudioPlay::Get()->Write(pcm, len);
						break;
					}
					msleep(1);
				}*/
				//std::cout << "Audio:" << frame << std::endl;
			}
			else
			{
				vdecode.Send(pkt);
				AVFrame* frame = vdecode.Recv();
				video->Repaint(frame);
				/*msleep(40);*/
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
	XResample resample;
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

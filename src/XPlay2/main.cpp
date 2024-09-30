#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "XDemux.h"
#include "XDecode.h"

int main(int argc, char *argv[])
{
    XDemux demux;
    demux.Read();
	demux.Clear();
	demux.Close();
    const char* url = "v1080.mp4";
    std::cout << demux.Open(url) << std::endl;
    std::cout << "CopyVPara()" << demux.CopyVPara() << std::endl;
    std::cout << "CopyAPara()" << demux.CopyAPara() << std::endl;
    std::cout << "seek()" << demux.Seek(0.95) << std::endl;

    XDecode vdecode;
    std::cout << "vdecode.Open()" << vdecode.Open(demux.CopyVPara()) << std::endl;
	//vdecode.Clear();
    //vdecode.Close();
    XDecode adecode;
    std::cout << "adecode.Open()" << adecode.Open(demux.CopyAPara()) << std::endl;

    //初始化显示
	/*for (;;)
	{
		AVPacket* pkt = demux.Read();
		if (demux.IsAudio(pkt))
		{
			adecode.Send(pkt);
			AVFrame* frame = adecode.Recv();
			std::cout << "Audio:" << frame << std::endl;
		}
		else
		{
			vdecode.Send(pkt);
			AVFrame* frame = vdecode.Recv();
			std::cout << "video:" << frame << std::endl;
		}
		if (!pkt)break;
	}*/
        
    QApplication a(argc, argv);
    XPlay2 w;
    w.show();
    //初始化gl窗口
    w.ui.video->Init(demux.width, demux.height);

    return a.exec();
}

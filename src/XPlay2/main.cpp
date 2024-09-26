#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "XDemux.h"
#include "XDecode.h"

int main(int argc, char *argv[])
{
    XDemux demux;
    std::cout << demux.Open("v1080.mp4") << std::endl;
    std::cout << "CopyVPara()" << demux.CopyVPara() << std::endl;
    std::cout << "CopyAPara()" << demux.CopyAPara() << std::endl;
    std::cout << "seek()" << demux.Seek(0.95) << std::endl;

    XDecode vdecode;
    std::cout << "vdecode.Open()" << vdecode.Open(demux.CopyVPara()) << std::endl;
    vdecode.Clear();
    vdecode.Close();
    XDecode adecode;
    std::cout << "adecode.Open()" << adecode.Open(demux.CopyAPara()) << std::endl;

	for (;;)
	{
		AVPacket* pkt = demux.Read();
		if (!pkt)break;
	}
        
    QApplication a(argc, argv);
    XPlay2 w;
    w.show();
    return a.exec();
}

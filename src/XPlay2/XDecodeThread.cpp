#include "XDecodeThread.h"
#include "XDecode.h"

XDecodeThread::XDecodeThread()
{
	//打开解码器
	if (!decode) decode = new XDecode();
}

XDecodeThread::~XDecodeThread()
{
	//等待线程退出
	isExit = true;
	wait();
}

void XDecodeThread::Push(AVPacket* pkt)
{
	if (!pkt)return;
	while (!isExit)
	{
		mux.lock();
		if (packets.size() < maxList)
		{
			packets.push_back(pkt);
			mux.unlock();
			break;
		}
		mux.unlock();
		msleep(1);
	}
}

void XDecodeThread::Clear()
{
	mux.lock();
	decode->Clear();
	while (!packets.empty())
	{
		AVPacket* pkt = packets.front();
		XFreePacket(&pkt);
		packets.pop_front();
	}
	mux.unlock();
}

void XDecodeThread::Close()
{
	Clear();
	//等待线程退出
	isExit = true;
	wait();
	decode->Close();
	mux.lock();
	delete decode;
	decode = nullptr;
	mux.unlock();
}

AVPacket* XDecodeThread::Pop()
{
	mux.lock();
	if (packets.empty())
	{
		mux.unlock();
		return nullptr;
	}
	AVPacket* pkt = packets.front();
	packets.pop_front();
	mux.unlock();
	return pkt;
}

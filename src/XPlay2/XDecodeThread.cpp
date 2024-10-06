#include "XDecodeThread.h"
#include "XDecode.h"

XDecodeThread::XDecodeThread()
{
	//�򿪽�����
	if (!decode) decode = new XDecode();
}

XDecodeThread::~XDecodeThread()
{
	//�ȴ��߳��˳�
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
	//�ȴ��߳��˳�
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

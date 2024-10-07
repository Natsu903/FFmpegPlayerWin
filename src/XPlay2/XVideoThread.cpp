#include "XVideoThread.h"
#include <iostream>
#include "XDecode.h"

bool XVideoThread::RepaintPts(AVPacket* pkt, long long seekpts)
{
	vmux.lock();
	bool re = decode->Send(pkt);
	if (!re)
	{
		vmux.unlock();
		return true;//��ʾ��������
	}
	AVFrame* frame = decode->Recv();
	if (!frame)
	{
		vmux.unlock();
		return false;
	}
	if (decode->pts >= seekpts)
	{
		if (call) call->Repaint(frame);
		vmux.unlock();
		return true;
	}
	XFreeFrame(&frame);
	vmux.unlock();
	return false;
}

bool XVideoThread::Open(AVCodecParameters* para, IVideoCall* call, int width, int height)
{
	if (!para)return false;
	Clear();
	vmux.lock();
	synpts = 0;
	//��ʼ����ʾ����
	this->call = call;
	if (call)
	{
		call->Init(width, height);
	}
	vmux.unlock();
	int re = true;
	if (!decode->Open(para))
	{
		std::cout << "audio XDecode open error" << std::endl;
		re = false;
	}
	std::cout << "XAudioThread::Open:" << re << std::endl;
	return re;
}




void XVideoThread::run()
{
	while (!isExit)
	{
		vmux.lock();
		if (isPause)
		{
			vmux.unlock();
			msleep(5);
			continue;
		}
		//����Ƶͬ��
		if (synpts > 0 && synpts < decode->pts)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}

		AVPacket* pkt = Pop();
		bool re = decode->Send(pkt);
		if (!re)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}
		//һ��send,���Recv
		while (!isExit)
		{
			AVFrame* frame = decode->Recv();
			if (!frame)break;
			//��ʾ��Ƶ
			if (call)
			{
				call->Repaint(frame);
			}
		}
		vmux.unlock();
	}
}

XVideoThread::XVideoThread()
{
}

XVideoThread::~XVideoThread()
{
}

void XVideoThread::SetPause(bool isPause)
{
	vmux.lock();
	this->isPause = isPause;
	vmux.unlock();
}

#include "XVideoThread.h"
#include <iostream>
#include "XDecode.h"
#include <iostream>

bool XVideoThread::Open(AVCodecParameters* para, IVideoCall* call, int width, int height)
{
	if (!para)return false;
	mux.lock();
	//初始化显示窗口
	this->call = call;
	if (call)
	{
		call->Init(width, height);
	}
	//打开解码器
	if (!decode) decode = new XDecode(); 
	int re = true;
	if (!decode->Open(para))
	{
		std::cout << "audio XDecode open error" << std::endl;
		re = false;
	}
	mux.unlock();
	std::cout << "XAudioThread::Open:" << re << std::endl;
	return re;
}

void XVideoThread::Push(AVPacket* pkt)
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

void XVideoThread::run()
{
	while (!isExit)
	{
		mux.lock();
		//没有数据
		if (packets.empty() || !decode)
		{
			mux.unlock();
			msleep(1);
			continue;
		}
		AVPacket* pkt = packets.front();
		packets.pop_front();
		bool re = decode->Send(pkt);
		if (!re)
		{
			mux.unlock();
			msleep(1);
			continue;
		}
		//一次send,多次Recv
		while (!isExit)
		{
			AVFrame* frame = decode->Recv();
			if (!frame)break;
			//显示视频
			if (call)
			{
				call->Repaint(frame);
			}
		}
		mux.unlock();
	}
}

XVideoThread::XVideoThread()
{
}

XVideoThread::~XVideoThread()
{
	//等待线程退出
	isExit = true;
	wait();
}

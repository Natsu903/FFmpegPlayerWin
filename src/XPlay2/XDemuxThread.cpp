#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>

bool XDemuxThread::Open(const char* url, IVideoCall* call)
{
	if (url == nullptr || url[0] == '\0') return false;
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	if (!at) at = new XAudioThread();
	//打开解封装 
	bool re = demux->Open(url);
	if (!re)
	{
		mux.unlock();
		std::cout << "demux->Open(url) error" << std::endl;
		return false;
	}
	//打开视频解码器和处理线程
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		re = false;
		std::cout << "vt->Open error" << std::endl;
	}
	//打开音频解码器和处理线程
	if (at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels))
	{
		re = false;
		std::cout << "at->Open error" << std::endl;
	}
	mux.unlock();
	std::cout << "XDemuxThread::Open :" << re << std::endl;
	return re;
}

void XDemuxThread::Start()
{
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	if (!at) at = new XAudioThread();
	//启动当前线程
	QThread::start();
	if (vt)vt->start();
	if (at)at->start();
	mux.unlock();
}

void XDemuxThread::run()
{
	while (!isExit)
	{
		mux.lock();
		if (!demux)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//音视频同步
		if (vt && at)
		{
			vt->synpts = at->pts;
		}
		AVPacket* pkt = demux->Read();
		if (!pkt)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//判断数据是音频
		if(demux->IsAudio(pkt))
		{
			if(at) at->Push(pkt);
		}
		else//视频
		{
			if (vt) vt->Push(pkt);
		}
		mux.unlock();
		msleep(1);
	}
}

XDemuxThread::XDemuxThread()
{
}

XDemuxThread::~XDemuxThread()
{
	isExit = true;
	wait();
}

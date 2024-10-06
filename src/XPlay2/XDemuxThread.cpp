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
	//�򿪽��װ 
	bool re = demux->Open(url);
	if (!re)
	{
		mux.unlock();
		std::cout << "demux->Open(url) error" << std::endl;
		return false;
	}
	//����Ƶ�������ʹ����߳�
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		re = false;
		std::cout << "vt->Open error" << std::endl;
	}
	//����Ƶ�������ʹ����߳�
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
	//������ǰ�߳�
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
		//����Ƶͬ��
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
		//�ж���������Ƶ
		if(demux->IsAudio(pkt))
		{
			if(at) at->Push(pkt);
		}
		else//��Ƶ
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

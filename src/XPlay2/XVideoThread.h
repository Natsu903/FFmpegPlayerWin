#pragma once
#include <QThread>
#include <mutex>
#include <list>
#include "IVideoCall.h"

struct AVPacket;
struct AVCodecParameters;
class XDecode;

//解码和显示视频部分
class XVideoThread:public QThread
{
public:
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	virtual void Push(AVPacket* pkt);
	void run();

	XVideoThread();
	virtual ~XVideoThread();

	//最大队列
	int maxList = 100;
	bool isExit = false;

protected:
	std::list<AVPacket*>packets;
	std::mutex mux;
	XDecode* decode = nullptr;
	IVideoCall* call = nullptr;
};


#pragma once
#include "XDecodeThread.h"
#include <mutex>
#include <list>
#include "IVideoCall.h"

struct AVPacket;
struct AVCodecParameters;
class XDecode;

//解码和显示视频部分
class XVideoThread:public XDecodeThread
{
public:
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	void run() override;
	XVideoThread();
	virtual ~XVideoThread();
	//同步时间由外部传入 
	long long synpts = 0;

protected:
	std::mutex vmux;
	IVideoCall* call = nullptr;
};


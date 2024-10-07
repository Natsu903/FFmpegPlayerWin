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
	//解码pts，如果接收到的解码数据pts>=seekpts return true,并显示画面
	virtual bool RepaintPts(AVPacket* pkt, long long seekpts);
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	void run() override;
	XVideoThread();
	virtual ~XVideoThread();
	//同步时间由外部传入 
	long long synpts = 0;

	void SetPause(bool isPause);
	bool isPause = false;


protected:
	IVideoCall* call = nullptr;
	std::mutex vmux;
};


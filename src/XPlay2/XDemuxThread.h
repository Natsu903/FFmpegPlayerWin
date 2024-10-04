#pragma once
#include <QThread>
#include "IVideoCall.h"
#include <mutex>

class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread:public QThread
{
public:
	//创建对象并打开
	virtual bool Open(const char* url, IVideoCall* call);
	//启动所有线程
	virtual void Start();

	void run();
	XDemuxThread();
	virtual ~XDemuxThread();
	bool isExit = false;
protected:
	std::mutex mux;
	XDemux* demux = nullptr;
	XVideoThread* vt = nullptr;
	XAudioThread* at = nullptr;
};


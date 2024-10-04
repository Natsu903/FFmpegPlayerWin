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
	//�������󲢴�
	virtual bool Open(const char* url, IVideoCall* call);
	//���������߳�
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


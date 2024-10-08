#pragma once
#include <QThread>
#include <mutex>
#include <list>

struct AVPacket;
class XDecode;

class XDecodeThread:public QThread
{
public:
	XDecodeThread();
	virtual ~XDecodeThread();
	virtual void Push(AVPacket* pkt);

	//清理队列
	virtual void Clear();

	//清理资源，停止线程
	virtual void Close();

	//取出一帧数据并出栈，如果没有返回null
	virtual AVPacket* Pop();

	//最大队列
	int maxList = 100;
	bool isExit = false;

protected:
	std::list<AVPacket*> packets;
	std::mutex mux;

	XDecode* decode = nullptr;
};


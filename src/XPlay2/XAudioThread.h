#pragma once
#include <QThread>
#include <mutex>
#include <list>
#include "XDecodeThread.h"

struct AVCodecParameters;
class XAudioPlay;
class XResample;

class XAudioThread:public XDecodeThread
{
public:
	//当前播放音频的pts
	long long pts = 0;
	//打开，不管成功与否都清理
	virtual bool Open(AVCodecParameters* para,int sampleRate,int channels);
	//停止线程，清理资源
	virtual void Close();
	void run() override;

	XAudioThread();
	virtual ~XAudioThread();

protected:
	XAudioPlay* ap = nullptr;
	XResample* res = nullptr;

	std::mutex amux;
};


#pragma once
#include <QThread>
#include <mutex>

struct AVCodecParameters;
class XDecode;
class XAudioPlay;
class XResample;

class XAudioThread:public QThread
{
public:
	//�򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para,int sampleRate,int channels);
	void run() override;
	XAudioThread();
	virtual ~XAudioThread();
protected:
	XDecode* decode=nullptr;
	XAudioPlay* ap = nullptr;
	XResample* res = nullptr;
	std::mutex mux;
};


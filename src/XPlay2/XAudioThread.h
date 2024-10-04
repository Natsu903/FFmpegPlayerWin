#pragma once
#include <QThread>
#include <mutex>
#include <list>

struct AVCodecParameters;
struct AVPacket;
class XDecode;
class XAudioPlay;
class XResample;

class XAudioThread:public QThread
{
public:
	//�򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para,int sampleRate,int channels);
	virtual void Push(AVPacket* pkt);
	void run() override;

	XAudioThread();
	virtual ~XAudioThread();
	
	int maxList = 100;//������
	bool isExit = false;

protected:
	XDecode* decode=nullptr;
	XAudioPlay* ap = nullptr;
	XResample* res = nullptr;

	std::mutex mux;
	std::list<AVPacket*> packets;
};


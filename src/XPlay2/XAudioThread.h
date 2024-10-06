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
	//��ǰ������Ƶ��pts
	long long pts = 0;
	//�򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para,int sampleRate,int channels);
	//ֹͣ�̣߳�������Դ
	virtual void Close();
	void run() override;

	XAudioThread();
	virtual ~XAudioThread();

protected:
	XAudioPlay* ap = nullptr;
	XResample* res = nullptr;

	std::mutex amux;
};


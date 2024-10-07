#pragma once
#include "XDecodeThread.h"
#include <mutex>
#include <list>
#include "IVideoCall.h"

struct AVPacket;
struct AVCodecParameters;
class XDecode;

//�������ʾ��Ƶ����
class XVideoThread:public XDecodeThread
{
public:
	//����pts��������յ��Ľ�������pts>=seekpts return true,����ʾ����
	virtual bool RepaintPts(AVPacket* pkt, long long seekpts);
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	void run() override;
	XVideoThread();
	virtual ~XVideoThread();
	//ͬ��ʱ�����ⲿ���� 
	long long synpts = 0;

	void SetPause(bool isPause);
	bool isPause = false;


protected:
	IVideoCall* call = nullptr;
	std::mutex vmux;
};


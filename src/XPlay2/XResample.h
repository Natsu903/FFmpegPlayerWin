#pragma once
#include <mutex>

struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class XResample
{
public:
	//����������������һ�³��˲�����ʽ�����Ϊs16
	virtual bool Open(AVCodecParameters* para, bool isClearPara = false);
	virtual void Close();

	//�����ز�����С,���ܳɹ�����ͷ�indata�ռ�
	virtual int Resample(AVFrame* indata, unsigned char* data);

	XResample();
	~XResample();

protected:
	std::mutex mux;
	SwrContext* actx = nullptr;
};


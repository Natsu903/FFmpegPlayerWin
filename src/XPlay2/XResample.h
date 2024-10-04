#pragma once
#include <mutex>

struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class XResample
{
public:
	//输出参数和输入参数一致除了采样格式，输出为s16
	virtual bool Open(AVCodecParameters* para, bool isClearPara = false);
	virtual void Close();

	//返回重采样大小,不管成功与否都释放indata空间
	virtual int Resample(AVFrame* indata, unsigned char* data);

	XResample();
	~XResample();

protected:
	std::mutex mux;
	SwrContext* actx = nullptr;
};


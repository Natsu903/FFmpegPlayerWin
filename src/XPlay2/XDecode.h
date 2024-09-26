#pragma once
#include <mutex>

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
class XDecode
{
public:
	bool isAudio = false;
	//打开解码器，不管成功与否都释放para空间
	virtual bool Open(AVCodecParameters *para);

	//发送到解码线程，不管成功与否都释放pkt空间(对象和媒体内容)
	virtual bool Send(AVPacket* pkt);
	//获取解码后数据，一次send可能需要多次recv,最后获取缓冲中的Send NULL再Recv多次
	virtual AVFrame* Recv();

	virtual void Close();
	virtual void Clear();


	XDecode();
	virtual ~XDecode();

protected:
	AVCodecContext* codec;
	std::mutex mux;
};


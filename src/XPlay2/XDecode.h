#pragma once
#include <mutex>

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

extern void XFreePacket(AVPacket** pkt);

class XDecode
{
public:
	bool isAudio = false;

	//当前解码到的pts
	long long pts = 0;

	//打开解码器，不管成功与否都释放para空间
	virtual bool Open(AVCodecParameters *para);

	//发送到解码线程，不管成功与否都释放pkt空间(对象和媒体内容)
	virtual bool Send(AVPacket* pkt);
	//获取解码后数据，一次send可能需要多次recv,最后获取缓冲中的Send NULL再Recv多次，每次复制一次要由调用者释放av_frame_free
	virtual AVFrame* Recv();

	virtual void Close();
	virtual void Clear();
	
	
	XDecode();
	virtual ~XDecode();

protected:
	AVCodecContext* codec = nullptr;
	std::mutex mux;
};


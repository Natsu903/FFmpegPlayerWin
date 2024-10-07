#pragma once
#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
	//打开媒体文件或流媒体(rtmp http rtsp)
	virtual bool Open(const char* url);

	//空间需要调用者释放，释放AVPacket和数据空间 av_packet_free
	virtual AVPacket* Read();

	//只读视频，丢弃音频，空间释放
	virtual AVPacket* ReadVideo();

	virtual bool IsAudio(AVPacket* pkt);

	//获取视频参数，返回的空间需要清理 avcodec_parameters_free()
	virtual AVCodecParameters* CopyVPara();

	//获取音频参数，返回的空间需要清理 avcodec_parameters_free()
	virtual AVCodecParameters* CopyAPara();

	//seek位置pos 0.0~1.0
	virtual bool Seek(double pos);

	//清空读取缓冲
	virtual void Clear();
	//关闭
	virtual void Close();

	XDemux();
	virtual ~XDemux();

	//媒体总时长(毫秒)
	int totalMs = 0;
	int width = 0;
	int height = 0;
	int sampleRate = 0;
	int channels = 0;

protected:
	std::mutex mux;
	//解封装上下文
	AVFormatContext* ic = nullptr;

	//音视频索引，用于区分流信息
	int videoStream = 0;
	int audioStream = 1;
};


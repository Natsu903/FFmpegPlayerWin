#include "XDemux.h"
#include <iostream>


extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

bool XDemux::Open(const char* url)
{
	Close();
	//参数设置
	AVDictionary* opts = nullptr;
	//设置rtsp流已经tcp打开
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);

	//网络延时时间
	av_dict_set(&opts, "max_delay", "500", 0);

	mux.lock();
	int re = avformat_open_input(&ic, url, nullptr, &opts);//参数设置为rtsp的延时时间
	if (re != 0)
	{
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		std::cout << "open" << url << "failed" << buf << std::endl;
		return false;
	}
	std::cout << "open" << url << "success" << std::endl;

	//获取流信息
	re = avformat_find_stream_info(ic, nullptr);

	//总时长
	int totalMs = ic->duration / (AV_TIME_BASE / 1000);
	std::cout << "totalMs=" << totalMs << std::endl;

	//打印视频流详细信息
	av_dump_format(ic, 0, url, 0);


	/**  
	//遍历获取音视频流信息
	for (int i = 0; i < ic->nb_streams; i++)
	{
		AVStream* as = ic->streams[i];
		std::cout << "codec_id=" << as->codecpar->codec_id << std::endl;
		std::cout << "format=" << as->codecpar->format << std::endl;

		//音频 AVMEDIA_TYPE_AUDIO
		if (as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStream = i;
			std::cout << i << "音频信息" << std::endl;
			std::cout << "sample_rate = " << as->codecpar->sample_rate << std::endl;
			//音频通道数
			std::cout << "channels = " << as->codecpar->ch_layout.nb_channels << std::endl;
			//单帧数据
			std::cout << "frame_size = " << as->codecpar->frame_size << std::endl;
		}

		//视频 AVMEDIA_TYPE_VIDEO
		if (as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = i;
			std::cout << i << "视频信息" << std::endl;
			std::cout << "width = " << as->codecpar->width << std::endl;
			std::cout << "height = " << as->codecpar->height << std::endl;
			//帧数
			std::cout << "video_fps = " << r2d(as->avg_frame_rate) << std::endl;
		}
	}
	*/

	//获取视频流
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	AVStream* as = ic->streams[videoStream];
	width = as->codecpar->width;
	height = as->codecpar->height;
	//打印视频信息
	std::cout << "=================================================" << std::endl;
	std::cout << "codec_id=" << as->codecpar->codec_id << std::endl;
	std::cout << "format=" << as->codecpar->format << std::endl;
	std::cout << videoStream << " video infomation" << std::endl;
	std::cout << "width = " << width << std::endl;
	std::cout << "height = " << height << std::endl;
	//帧数
	std::cout << "video_fps = " << r2d(as->avg_frame_rate) << std::endl;


	//获取音频流
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	as = ic->streams[audioStream];
	std::cout << "=================================================" << std::endl;
	//打印音频信息
	sampleRate = as->codecpar->sample_rate;
	channels = as->codecpar->ch_layout.nb_channels;
	std::cout << "codec_id=" << as->codecpar->codec_id << std::endl;
	std::cout << "format=" << as->codecpar->format << std::endl;
	std::cout << audioStream << " audio information" << std::endl;
	std::cout << "sample_rate = " << as->codecpar->sample_rate << std::endl;
	//音频通道数
	std::cout << "channels = " << as->codecpar->ch_layout.nb_channels << std::endl;
	//单帧数据
	std::cout << "frame_size = " << as->codecpar->frame_size << std::endl;
	mux.unlock();
	return true;
}

AVPacket* XDemux::Read()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return nullptr;
	}
	AVPacket* pkt = av_packet_alloc();
	//读取一帧并分配空间
	int re = av_read_frame(ic, pkt);
	if (re != 0)
	{
		mux.unlock();
		av_packet_free(&pkt);
		return nullptr;
	}
	//pts转换为毫秒
	pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	mux.unlock();
	//std::cout << pkt->pts << " " << std::flush;
	return pkt;
}

bool XDemux::IsAudio(AVPacket* pkt)
{
	if (!pkt)return false;
	if (pkt->stream_index == videoStream)
		return false;
	return true;
}

AVCodecParameters* XDemux::CopyVPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return nullptr;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
	mux.unlock();
	return pa;
}

AVCodecParameters* XDemux::CopyAPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return nullptr;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
	mux.unlock();
	return pa;
}

bool XDemux::Seek(double pos)
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return false;
	}
	//清理读取缓冲
	avformat_flush(ic);

	long long seekPos = 0;
	seekPos = ic->streams[videoStream]->duration * pos;
	int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	mux.unlock();
	if (re < 0) return false;
	return true;
}

void XDemux::Clear()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	//清理读取缓冲
	avformat_flush(ic);
	mux.unlock();
}

void XDemux::Close()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	//清理读取缓冲
	avformat_close_input(&ic);
	totalMs = 0;
	mux.unlock();
}

XDemux::XDemux()
{
	static bool isFirst = true;
	static std::mutex dmux;
	dmux.lock();
	if (isFirst)
	{
		//初始化网络库
		avformat_network_init();
		isFirst = false;
	}
	dmux.unlock();
}

XDemux::~XDemux()
{
}

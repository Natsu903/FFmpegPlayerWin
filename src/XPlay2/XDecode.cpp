#include "XDecode.h"
#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
}

bool XDecode::Open(AVCodecParameters* para)
{
	if (!para) return false;
	Close();
	//////////////////////////////////////////////////////////
	///解码器打开
	///找到解码器
	const AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
	if (!vcodec)
	{
		avcodec_parameters_free(&para);
		std::cout << "can't find the codec id " << para->codec_id << std::endl;
		return false;
	}
	std::cout << "find the AVCodec " << para->codec_id << std::endl;

	mux.lock();
	codec = avcodec_alloc_context3(vcodec);

	///配置解码器上下文参数
	avcodec_parameters_to_context(codec, para);
	avcodec_parameters_free(&para);

	//16线程解码
	codec->thread_count = 16;

	///打开解码器上下文
	int re = avcodec_open2(codec, nullptr, nullptr);
	if (re != 0)
	{
		avcodec_free_context(&codec);
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		std::cout << "avcodec_open2  failed! :" << buf << std::endl;
		return false;
	}
	mux.unlock();
	std::cout << " avcodec_open2 success!" << std::endl;
	return true;
}

bool XDecode::Send(AVPacket* pkt)
{
	if (!pkt || pkt->size <= 0 || !pkt->data) return false;
	mux.lock();
	if (!codec)
	{
		mux.unlock();
		return false;
	}
	int re = avcodec_send_packet(codec, pkt);
	mux.unlock();
	av_packet_free(&pkt);
	if (re != 0)return false;
	return true;
}

AVFrame* XDecode::Recv()
{
	mux.lock();
	if (!codec)
	{
		mux.unlock();
		return nullptr;
	}
	AVFrame* frame = av_frame_alloc();
	int re = avcodec_receive_frame(codec, frame);
	mux.unlock();
	if (re != 0)
	{
		av_frame_free(&frame);
		return nullptr;
	}
	pts = frame->pts;
	return frame;
}

void XDecode::Close()
{
	mux.lock();
	if (codec)
	{
		avcodec_close(codec);
		avcodec_free_context(&codec);
	}
	pts = 0;
	mux.unlock();
}

void XDecode::Clear()
{
	mux.lock();
	//清理解码缓冲
	if (codec)
	{
		avcodec_flush_buffers(codec);
	}
	mux.unlock();
}

XDecode::XDecode()
{
}

XDecode::~XDecode()
{
}

#include "XDecode.h"
#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
}

bool XDecode::Open(AVCodecParameters* para)
{
	if (!para)return false;
	Close();
	//打开解码器
	//找到解码器
	const AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
	if (!vcodec)
	{
		avcodec_parameters_free(&para);
		std::cout << "can't find the codec_id" << std::endl;
		return false;
	}
	std::cout << "find the AVCodec" << para->codec_id << std::endl;
	mux.lock();
	codec = avcodec_alloc_context3(vcodec);

	//配置解码器上下文参数
	avcodec_parameters_to_context(codec, para);
	//多线程解码
	codec->thread_count = 16;

	//打开解码器上下文
	int re = avcodec_open2(codec, 0, 0);
	if (re != 0)
	{
		avcodec_free_context(&codec);
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		std::cout << "avcodec_open2 failed" << std::endl;
		return false;
	}
	mux.unlock();
	std::cout << "avcodec_open2 success" << std::endl;
	avcodec_parameters_free(&para);
	return true;
}

void XDecode::Close()
{
	mux.lock();
	if (codec)
	{
		avcodec_free_context(&codec);
	}
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

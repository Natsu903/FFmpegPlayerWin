#include "XResample.h"
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#include <iostream>

#pragma comment(lib,"swresample.lib")

bool XResample::Open(AVCodecParameters* para, bool isClearPara)
{
	if (!para)return false;
	mux.lock();
	//��Ƶ�ز����������ĳ�ʼ��
	
	AVChannelLayout in_ch_layout = AV_CHANNEL_LAYOUT_STEREO;//�����ʽ
	AVSampleFormat in_sample_fmt = (AVSampleFormat)para->format;//����������ʽ
	int in_sample_rate = para->sample_rate;//���������

	AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;//�����ʽ
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;//���������ʽ
	int out_sample_rate = para->sample_rate;//���������

	int re = swr_alloc_set_opts2(&actx,
		&out_ch_layout,out_sample_fmt,out_sample_rate,
		&in_ch_layout,in_sample_fmt,out_sample_rate,
		0, nullptr);

	if (isClearPara)
	{
		avcodec_parameters_free(&para);
	}

	if (re != 0)
	{
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		std::cout << "open" << swr_alloc_set_opts2 << "failed" << buf << std::endl;
		return false;
	}

	re = swr_init(actx);
	mux.unlock();
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		std::cout << "open" << swr_alloc_set_opts2 << "failed" << buf << std::endl;
		return false;
	}
	return true;
}

void XResample::Close()
{
	mux.lock();
	if (actx)
	{
		swr_free(&actx);
	}
	mux.unlock();
}

int XResample::Resample(AVFrame* indata, unsigned char* d)
{
	if (!indata)return 0;
	if (!d)
	{
		av_frame_free(&indata);
		return 0;
	}
	uint8_t* data[2] = { nullptr };
	data[0] = d;
	int re = swr_convert(actx, data, indata->nb_samples, 
		(const uint8_t**)indata->data, indata->nb_samples);
	int outSize=re*indata->ch_layout.nb_channels*2;
	return outSize;
}

XResample::XResample()
{
}

XResample::~XResample()
{
}

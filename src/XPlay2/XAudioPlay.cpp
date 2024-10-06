#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioSink>
#include <mutex>

class CXAudioPlay :public XAudioPlay
{
public:
	QAudioSink* output = nullptr;
	QIODevice* io = nullptr;
	std::mutex mux;
	bool Open() override
	{
		Close();
		QAudioFormat fmt;
		fmt.setSampleRate(44100);
		fmt.setChannelCount(2);
		fmt.setSampleFormat(QAudioFormat::Int16);
		mux.lock();
		output = new QAudioSink(fmt);
		io = output->start();
		mux.unlock();
		if(io) return true;
		return false;
	}

	bool Close() override
	{
		mux.lock();
		if (io)
		{
			io->close();
			io = nullptr;
		}

		if (output)
		{
			output->stop();
			delete output;
			output = nullptr;
		}
		mux.unlock();
		return true;
	}

	bool  
		Write(const unsigned char* data, int datasize) override
	{
		if (!data || datasize <= 0) return false;
		mux.lock();
		if (!output || !io)
		{
			mux.unlock();
			return false;
		}
		int size = io->write((char*)data, datasize);
		mux.unlock();
		if (datasize != size) return false;
		return true;
	}

	int GetFree() override
	{
		mux.lock();
		if (!output)
		{
			mux.unlock();
			return 0;
		}
		int free = output->bytesFree();
		mux.unlock();
		return free;
	}

	long long GetNoPlayMs() override
	{
		mux.lock();
		if (!output)
		{
			mux.unlock();
			return 0;
		}
		long long pts = 0;
		//还未播放的字节数
		double size = output->bufferSize() - output->bytesFree();
		//一秒音频的字节大小
		double secSize = sampleRate * (sampleSize / 8) * channels;
		if (secSize <= 0)
		{
			pts = 0;
		}
		else
		{
			pts = (size / secSize) * 1000;
		}
		mux.unlock();
		return pts;
	}
};

XAudioPlay* XAudioPlay::Get()
{
	static CXAudioPlay play;
	return &play;
}

XAudioPlay::XAudioPlay()
{
}

XAudioPlay::~XAudioPlay()
{
}

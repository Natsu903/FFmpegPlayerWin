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

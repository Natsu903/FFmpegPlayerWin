#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <iostream>

bool XAudioThread::Open(AVCodecParameters* para, int sampleRate, int channels)
{
    if (!para)return false;
    mux.lock();
    if (!decode) decode = new XDecode();
    if (!res) res = new XResample();
    if (!ap) ap = XAudioPlay::Get();
    bool re = true;
    if (!res->Open(para, false))
    {
        std::cout << "XResample open Error" << std::endl;
        re = false;
    }
    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if (!ap->Open())
    {
        re = false;
        std::cout << "XAudioPlay open error" << std::endl;
    }
    if (!decode->Open(para))
    {
        std::cout << "audio XDecode open error" << std::endl;
        re = false;
    }
    mux.unlock();
    std::cout << "XAudioThread::Open:" << re << std::endl;
    return re;
}

void XAudioThread::run()
{
}

XAudioThread::XAudioThread()
{
}

XAudioThread::~XAudioThread()
{
}

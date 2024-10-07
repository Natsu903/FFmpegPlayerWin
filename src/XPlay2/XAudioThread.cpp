#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <iostream>

bool XAudioThread::Open(AVCodecParameters* para, int sampleRate, int channels)
{
    if (!para)return false;
    Clear();
    amux.lock();
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
    amux.unlock();
    std::cout << "XAudioThread::Open:" << re << std::endl;
    return re;
}


void XAudioThread::Close()
{
    XDecodeThread::Close();
    if (res)
    {
        res->Close();
        amux.lock();
        delete res;
        res = nullptr;
        amux.unlock();
    }
    if (ap)
    {
        ap->Close();
		amux.lock();
        ap = nullptr;
		amux.unlock();
    }
}

void XAudioThread::Clear()
{
    XDecodeThread::Clear();
    mux.lock();
    if (ap) ap->Clear();
    mux.unlock();
}

void XAudioThread::run()
{
    unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        amux.lock();
        if (isPause)
        {
            amux.unlock();
            msleep(5);
            continue;
        }
        AVPacket* pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            amux.unlock();
			msleep(1);
			continue;
        }
        //һ��send,���Recv
        while (!isExit)
        {
			AVFrame* frame = decode->Recv();
            if (!frame)break;
            //��ȥ������δ���ŵ�ʱ��ms
            pts = decode->pts - ap->GetNoPlayMs();
            std::cout << "audio pts=" << pts << std::endl;
            //�ز���
            int size = res->Resample(frame, pcm);
            //������Ƶ
            while (!isExit)
            {
                if(size<=0)break;
                //����δ���꣬�ռ䲻��
                if (ap->GetFree() < size || isPause)
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
        amux.unlock();
    }
    delete []pcm;
}

XAudioThread::XAudioThread()
{
	if (!res) res = new XResample();
	if (!ap) ap = XAudioPlay::Get();
}

XAudioThread::~XAudioThread()
{
    //�ȴ��߳��˳�
    isExit = true;
    wait();
}

void XAudioThread::SetPause(bool isPause)
{
    this->isPause = isPause;
    if (ap)
        ap->SetPause(isPause);
}

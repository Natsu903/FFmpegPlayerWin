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

void XAudioThread::Push(AVPacket* pkt)
{
    if (!pkt)return;
    while (!isExit)
	{
		mux.lock();
        if (packets.size() < maxList)
        {
			packets.push_back(pkt);
            mux.unlock();
            break;
        }
		mux.unlock();
        msleep(1);
    }
}

void XAudioThread::run()
{
    unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        mux.lock();
        //û������
        if (packets.empty() || !decode || !res || !ap)
        {
            mux.unlock();
            msleep(1);
            continue;
        }
        AVPacket* pkt = packets.front();
        packets.pop_front();
        bool re = decode->Send(pkt);
        if (!re)
        {
            mux.unlock();
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
                if (ap->GetFree() < size)
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
        mux.unlock();
    }
    delete []pcm;
}

XAudioThread::XAudioThread()
{
}

XAudioThread::~XAudioThread()
{
    //�ȴ��߳��˳�
    isExit = true;
    wait();
}

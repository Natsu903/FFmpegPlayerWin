#pragma once
#include <QThread>
#include <mutex>
#include <list>

struct AVPacket;
class XDecode;

class XDecodeThread:public QThread
{
public:
	XDecodeThread();
	virtual ~XDecodeThread();
	virtual void Push(AVPacket* pkt);

	//�������
	virtual void Clear();

	//������Դ��ֹͣ�߳�
	virtual void Close();

	//ȡ��һ֡���ݲ���ջ�����û�з���null
	virtual AVPacket* Pop();

	//������
	int maxList = 100;
	bool isExit = false;

protected:
	std::list<AVPacket*> packets;
	std::mutex mux;
	XDecode* decode = nullptr;
};


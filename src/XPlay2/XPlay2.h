#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlay2.h"

class XPlay2 : public QWidget
{
    Q_OBJECT

public:
    XPlay2(QWidget *parent = nullptr);
    ~XPlay2();

    //��ʱ������������ʾ
    void timerEvent(QTimerEvent* e) override;

    //���ڳߴ�仯
    void resizeEvent(QResizeEvent* e) override;

    //˫��ȫ��
    void mouseDoubleClickEvent(QMouseEvent* e) override;

    //���ţ���ͣ
    void setPause(bool isPause);

public slots:
    void OpenFile();
    void PlayorPause();
    void sliderPress();
    void sliderRelease();

private:
    bool isSliderPressed = false;
    Ui::XPlay2Class ui;
};

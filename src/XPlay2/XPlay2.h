#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlay2.h"

class XPlay2 : public QWidget
{
    Q_OBJECT

public:
    XPlay2(QWidget *parent = nullptr);
    ~XPlay2();

    //定时器做滑动条显示
    void timerEvent(QTimerEvent* e) override;

    //窗口尺寸变化
    void resizeEvent(QResizeEvent* e) override;

    //双击全屏
    void mouseDoubleClickEvent(QMouseEvent* e) override;

    //播放，暂停
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

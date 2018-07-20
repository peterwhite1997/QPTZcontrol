#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QThread>

#include <iostream>
#include "dhnetsdk.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>



namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    QThread workerThread;

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void ConsoleAppend(QString newString);
    QString ConsoleString = "";
    QImage cvMat2QImage(const cv::Mat& mat);
    void KNN();


    // 大华控制函数的声明
    BOOL g_bNetSDKInitFlag;
    LLONG g_lLoginHandle = 0L;
    char g_szDevIp[32] = "192.168.73.110";
    WORD g_nPort = 36956;

    char g_szUserName[64] = "admin";
    char g_szPasswd[64] = "123456";

    void InitTest();
    void CALLBACK HaveReConnect(long long lLoginID, char *pchDVRIP, LONG nDVRPort, long long dwUser);
    void CALLBACK DisconnectFunc(long long lLoginID, char *pchDVRIP, LONG nDVRPort, long long dwUser);




private slots:
    void on_LoginButton_clicked();

    void on_upButton_pressed();

    void on_upButton_released();

    void on_DownButton_pressed();

    void on_DownButton_released();

    void on_LeftButton_pressed();

    void on_LeftButton_released();

    void on_RightButton_pressed();

    void on_RightButton_released();

    void on_GoButton_clicked();

    void on_LogoutButton_clicked();

    void get_One_frame();

    void on_plusButton_pressed();

    void on_plusButton_released();

    void on_minusButton_pressed();

    void on_minusButton_released();

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);

    //void wheelEvent(QWheelEvent*event);
signals:
    void operate();

private:
    Ui::Widget *ui;
    QTimer *timer;
    bool keyW;
    int SPD = 6;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief The Worker class
///////////////////////////////////////////////////////////////////////////////


class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0){}
    QImage cvMat2QImage(const cv::Mat& mat);
signals:

public slots:
    void rtsp_Stream();

};


#endif // WIDGET_H

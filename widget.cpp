#include "widget.h"
#include "ui_widget.h"

cv::Mat l_frame;
cv::VideoCapture *capture;
QImage image;  // 存放已经转换好的图像

// 先预设登录信息
char szDevIp[64] = {"lab.zhuzhuguowang.cn"};
NET_DEVICEINFO stDevInfo = {0};
int nError = 0;
int nPort = 36956;
long lLoginHandle = 0;

// 以下是内建函数
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 固定窗口大小
    //this->setMaximumSize(800,600);
    //this->setMinimumSize(800,600);

    // 初始化定时器timer, 用于固定时间间隔获取视频帧
    timer = new QTimer(this);
    // timeout信号发出后，显示一帧处理过的图像
    connect(timer, SIGNAL(timeout()), this, SLOT(get_One_frame()));

    // 以下是线程的相关设定
    Worker *worker = new Worker;
    // 调用moveToThread将该任务交割workerThread
    worker->moveToThread(&workerThread);
    // operate信号发射后开始在后台进行图像处理
    connect(this, SIGNAL(operate()), worker, SLOT(rtsp_Stream()));


}
Widget::~Widget(){
    workerThread.quit();
    delete ui;
}

void Widget::ConsoleAppend(QString newString)
{
    this->ConsoleString += newString;
    ui->ConsoleBrowser->setText(this->ConsoleString);
}

void Widget::on_LoginButton_clicked()
{
    lLoginHandle = CLIENT_LoginEx2(szDevIp, nPort, "admin", "123456", EM_LOGIN_SPEC_CAP_TCP,NULL,NULL, &nError);
    if(!lLoginHandle)
    {
        ConsoleAppend("Login Failed!\n");

        // 登录失败时播放测试用视频
        capture = new cv::VideoCapture("VideoTest.avi");
        if (!capture->isOpened())
        {
            ConsoleAppend("video not open.\n");
            return;
        }

        double rate = capture->get(CV_CAP_PROP_FPS);
        int delay = 1000 / rate;
        timer->start(delay);
    }
    else  // 登录成功
    {
        qDebug() << "Login Succeeded!" << endl;
        ConsoleAppend("Login Succeeded! loginhandle:");
        ConsoleAppend(QString::number(lLoginHandle));
        ConsoleAppend("\n");
        capture = new cv::VideoCapture("rtsp://admin:123456@lab.zhuzhuguowang.cn:36955/cam/realmonitor?channel=1&subtype=0");

       // 多线程工作信号的激活
        workerThread.start();
        emit operate();

        // 定时器启动
        timer->start(66);
    }
}



void Widget::on_upButton_pressed()
{
    CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_UP_CONTROL, 0, 1, 0, FALSE,NULL);
    //ConsoleAppend("Up!\n");
}

void Widget::on_upButton_released()
{
    CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_UP_CONTROL, 0, 1, 0, TRUE,NULL);
    //ConsoleAppend("Stop!\n");
}

void Widget::on_DownButton_pressed()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_DOWN_CONTROL,0,1,0,FALSE,NULL);
    //ConsoleAppend("Down!\n");
}

void Widget::on_DownButton_released()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_DOWN_CONTROL,0,1,0,TRUE,NULL);
    //ConsoleAppend("Stop!\n");
}

void Widget::on_LeftButton_pressed()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_LEFT_CONTROL,0,1,0,FALSE,NULL);
    //ConsoleAppend("Left!\n");
}

void Widget::on_LeftButton_released()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_LEFT_CONTROL,0,1,0,TRUE,NULL);
    //ConsoleAppend("Stop!\n");
}

void Widget::on_RightButton_pressed()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_RIGHT_CONTROL,0,1,0,FALSE,NULL);
    //ConsoleAppend("Right!\n");
}

void Widget::on_RightButton_released()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_RIGHT_CONTROL,0,1,0,TRUE,NULL);
    //ConsoleAppend("Stop!\n");
}

void Widget::on_plusButton_pressed()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_ADD_CONTROL,0,1,0,FALSE,NULL);
}

void Widget::on_plusButton_released()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_ADD_CONTROL,0,1,0,TRUE,NULL);
}

void Widget::on_minusButton_pressed()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_DEC_CONTROL,0,1,0,FALSE,NULL);
}

void Widget::on_minusButton_released()
{
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_DEC_CONTROL,0,1,0,TRUE,NULL);
}

void Widget::on_GoButton_clicked()
{
    int h = 0;
    int v = 0;
    int x = 1;
    h = this->ui->hLineEdit->text().toInt();
    v = this->ui->vLineEdit->text().toInt();
    x = this->ui->xLineEdit->text().toInt();
    CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_EXTPTZ_EXACTGOTO,h,v,x,FALSE,NULL);
    ConsoleAppend("h = ");
    ConsoleAppend(QString::number(h));
    ConsoleAppend("\n");
    ConsoleAppend("v = ");
    ConsoleAppend(QString::number(v));
    ConsoleAppend("\n");
    ConsoleAppend("x = ");
    ConsoleAppend(QString::number(x));
    ConsoleAppend("\n");
}

void Widget::on_LogoutButton_clicked()
{
    timer->stop();
    CLIENT_Logout(lLoginHandle);
    CLIENT_Cleanup();
    this->close();
}

void Widget::get_One_frame()
{


    ui->FrameLabel->setPixmap(QPixmap::fromImage(image));


}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    switch(event->key())
    {

    case Qt::Key_W: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_UP_CONTROL, 0, SPD, 0, FALSE, NULL);
                    break;

    case Qt::Key_S: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_DOWN_CONTROL, 0, SPD, 0, FALSE, NULL);
                    break;

    case Qt::Key_A: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_LEFT_CONTROL, 0, SPD, 0, FALSE, NULL);
                    break;

    case Qt::Key_D: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_RIGHT_CONTROL, 0, SPD, 0, FALSE, NULL);
                    break;
    }

}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    switch(event->key())
    {

    case Qt::Key_W: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_UP_CONTROL, 0, SPD, 0, TRUE, NULL);
                    break;

    case Qt::Key_S: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_DOWN_CONTROL, 0, SPD, 0, TRUE, NULL);
                    break;

    case Qt::Key_A: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_LEFT_CONTROL, 0, SPD, 0, TRUE, NULL);
                    break;

    case Qt::Key_D: CLIENT_DHPTZControlEx2(lLoginHandle, 0, DH_PTZ_RIGHT_CONTROL, 0, SPD, 0, TRUE, NULL);
                    break;
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{

        if(event->button() == Qt::LeftButton)
        {
            QPoint eventpoint = event->globalPos();
            eventpoint = ui->FrameLabel->mapFromGlobal(eventpoint);
            //鼠标指针坐标
            ConsoleAppend("x=");
            ConsoleAppend(QString::number(eventpoint.x()));
            ConsoleAppend("\n");

        }
}


//一个失败的函数，原来打算用滚轮控制焦距的
/*void Delay(int time)  // time*1000为秒数
{
    clock_t now = clock();

    while(clock()-now < time);
}

void Widget::wheelEvent(QWheelEvent*event)
{

    if(event->delta()>0)  // 如果滚轮往上滚
       {
            CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_ADD_CONTROL,0,1,0,FALSE,NULL);

            CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_ADD_CONTROL,0,1,0,TRUE,NULL);
       }

    else  // 同样的
    {
       CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_DEC_CONTROL,0,1,0,FALSE,NULL);
       CLIENT_DHPTZControlEx2(lLoginHandle,0,DH_PTZ_ZOOM_DEC_CONTROL,0,1,0,TRUE,NULL);
    }
}*/


////////////////////////////////////////////////////////////////


void Worker::rtsp_Stream()
{
        using namespace cv;
        using namespace std;


        qDebug() << "rtsp Start!" << endl;

        while(capture->isOpened())  // 在后台不停循环，将处理好的图像放在全局变量l_frame中
        {
            capture->read(l_frame);
            cv::resize(l_frame,l_frame,cv::Size(530,390),0,0,cv::INTER_AREA );
            image = cvMat2QImage(l_frame);
        }
        return;

}

QImage Worker ::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}


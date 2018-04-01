#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    camTimer = new QTimer();
    connect(camTimer, SIGNAL(timeout()), this, SLOT(cam_timeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    bool isOpened = cap.open(0);
    if( !isOpened )  // check if we succeeded
    {
        qDebug()<<"cannot open";
        return;
    }
    cap.open(0);
    camTimer->start(30);
}

void MainWindow::cam_timeout()
{
    Mat frame;
    cap >> frame;
    cv::resize(frame,frame,Size(640,480),0,0,INTER_CUBIC);
    cvtColor(frame, frame, COLOR_BGR2RGB);
    QImage imgIn;
    imgIn= QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    ui->image_label->setPixmap(QPixmap::fromImage(imgIn));
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ofi = new OpenFaceInterface();

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

    Mat face = ofi->detectingLandmarks(frame);
    cv::resize(face,face,Size(640,480),0,0,INTER_CUBIC);
    cvtColor(face, face, COLOR_BGR2RGB);
    QImage imgIn;
    imgIn= QImage((uchar*) face.data, face.cols, face.rows, face.step, QImage::Format_RGB888);
    ui->image_label->setPixmap(QPixmap::fromImage(imgIn));
}

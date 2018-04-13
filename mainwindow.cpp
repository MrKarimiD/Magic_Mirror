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

    QStringList output_items;
    output_items<<"Input"<<"Output";
    ui->output_comboBox->addItems(output_items);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    bool isOpened = cap.open(1);
    if( !isOpened )  // check if we succeeded
    {
        qDebug()<<"cannot open";
        return;
    }
    ui->output_comboBox->setEnabled(true);
    cap.open(1);
    camTimer->start(30);
}

void MainWindow::cam_timeout()
{
    Mat frame;
    cap >> frame;

    Mat face = ofi->detectingLandmarks(frame);

    Mat output;
    if(ui->output_comboBox->currentText() == "Output")
        face.copyTo(output);
    else
        frame.copyTo(output);

    cv::resize(output,output,Size(640,480),0,0,INTER_CUBIC);
    cvtColor(output, output, COLOR_BGR2RGB);
    QImage imgIn;
    imgIn= QImage((uchar*) output.data, output.cols, output.rows, output.step, QImage::Format_RGB888);
    ui->image_label->setPixmap(QPixmap::fromImage(imgIn));
}

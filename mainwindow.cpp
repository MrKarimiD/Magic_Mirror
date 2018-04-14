#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    beatNum = 0;

    ofi = new OpenFaceInterface();

    camTimer = new QTimer();
    connect(camTimer, SIGNAL(timeout()), this, SLOT(cam_timeout()));

    colorTimer = new QTimer();
    connect(colorTimer, SIGNAL(timeout()), this, SLOT(color_timeout()));

    QStringList output_items;
    output_items<<"Input"<<"Output";
    ui->output_comboBox->addItems(output_items);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    cv::resize(output,output,Size(1280,960),0,0,INTER_CUBIC);
    //cvtColor(output, output, COLOR_BGR2RGB);
    QImage imgIn;
    imgIn= QImage((uchar*) output.data, output.cols, output.rows, output.step, QImage::Format_RGB888);
    ui->image_label->setPixmap(QPixmap::fromImage(imgIn));
}

void MainWindow::color_timeout()
{
    qDebug()<<"color_timout";

    ofi->changeTheRange();

    if( beatNum >= beats.size() )
    {
        colorTimer->stop();
        return;
    }

    float startB = (float)beats.at(beatNum);
    float endB = (float)beats.at(beatNum+1);
    colorTimer->start(endB - startB);
    beatNum++;
}

void MainWindow::on_musicAddr_button_clicked()
{
    QString fileAddress = QFileDialog::getOpenFileName(this,tr("Select Your Source Audio"), "/home", tr("Audio Files (*.mp3)"));
    ui->musicAddr_lineEdit->setText(fileAddress);
}

void MainWindow::on_openCam_button_clicked()
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

void MainWindow::on_MACProcess_button_clicked()
{
    ui->mac_progressBar->setEnabled(true);
    MusicAnalysis music_processor;
//    if( ui->bits_rButton->isChecked() )
        beats = music_processor.beatDetection(ui->musicAddr_lineEdit->text(), "output");
//    else if( ui->pitch_rButton->isChecked() )
//        music_processor.pitchDetection(ui->musicAddr_lineEdit->text(), "output");

    qDebug()<<"Done";
}

void MainWindow::on_play_button_clicked()
{
    ofi->changeTheRange();

    player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile(ui->musicAddr_lineEdit->text()));
    player->setVolume(80);
    player->play();

    beatNum = 0;

    float startB = (float)beats.at(beatNum);
    float endB = (float)beats.at(beatNum+1);
    colorTimer->start(endB - startB);
    beatNum++;
}

void MainWindow::on_stop_button_clicked()
{
    player->pause();
    colorTimer->stop();
}

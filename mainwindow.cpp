#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap play_pixmap("play.png");
    QIcon PlayIcon(play_pixmap);
    ui->play_button->setIcon(PlayIcon);
    ui->play_button->setIconSize(QSize(51,41));

    QPixmap stop_pixmap("stop.png");
    QIcon StopIcon(stop_pixmap);
    ui->stop_button->setIcon(StopIcon);
    ui->stop_button->setIconSize(QSize(51,41));

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
    cvtColor(frame, frame, COLOR_BGR2RGB);

    Mat face = ofi->detectingLandmarks(frame, ui->random_checkBox->isChecked(), ui->face_checkBox->isChecked());

    Mat output;
    if(ui->output_comboBox->currentText() == "Output")
        face.copyTo(output);
    else
        frame.copyTo(output);

    cv::resize(output,output,Size(1280,960),0,0,INTER_CUBIC);
//    if(ui->output_comboBox->currentText() == "Input")
//        cvtColor(output, output, COLOR_BGR2RGB);
    flip(output, output, 1);
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
    qDebug()<<"Music Started";
    MusicAnalysis music_processor;
    //    if( ui->bits_rButton->isChecked() )
    beats = music_processor.beatDetection(ui->musicAddr_lineEdit->text(), "output");
    //    else if( ui->pitch_rButton->isChecked() )
    //        music_processor.pitchDetection(ui->musicAddr_lineEdit->text(), "output");

    qDebug()<<"Music Done";
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

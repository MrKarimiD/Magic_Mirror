#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QFileDialog>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "openfaceinterface.h"
#include "musicanalysis.h"


using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void cam_timeout();

    void color_timeout();

    void on_musicAddr_button_clicked();

    void on_openCam_button_clicked();

    void on_MACProcess_button_clicked();

    void on_play_button_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *camTimer;
    QTimer *colorTimer;
    VideoCapture cap;
    OpenFaceInterface* ofi;
    vector<Real> beats;
    int beatNum;
};

#endif // MAINWINDOW_H

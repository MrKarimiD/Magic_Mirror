#ifndef OPENFACEINTERFACE_H
#define OPENFACEINTERFACE_H

#include <QObject>
#include <QDebug>

#include "opencv2/face.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;
using namespace cv::face;

class OpenFaceInterface : public QObject
{
    Q_OBJECT
public:
    explicit OpenFaceInterface(QObject *parent = nullptr);
    Mat detectingLandmarks(Mat input);
    //bool myDetector(InputArray image, OutputArray faces);

signals:

public slots:

private:
    CascadeClassifier faceDetector;
    Ptr<Facemark> facemark;

};

#endif // OPENFACEINTERFACE_H

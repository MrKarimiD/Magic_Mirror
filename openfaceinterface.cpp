#include "openfaceinterface.h"

OpenFaceInterface::OpenFaceInterface(QObject *parent)
{
    faceDetector.load("haarcascade_frontalface_alt2.xml");
    facemark = FacemarkLBF::create();

    // Load landmark detector
    facemark->loadModel("lbfmodel.yaml");
}

Mat OpenFaceInterface::detectingLandmarks(Mat input)
{
    // Variable to store a video frame and its grayscale
    Mat frame, gray;
    input.copyTo(frame);
    Mat out(input.rows, input.cols, CV_8UC3, Scalar(0,0,0));

    // Find face
    vector<Rect> faces;
    // Convert frame to grayscale because
    // faceDetector requires grayscale image.
    medianBlur ( frame, frame, 7 );
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Detect faces
    faceDetector.detectMultiScale(gray, faces);
    for(int i = 0; i< faces.size(); i++)
    {
        qDebug()<<"face "<<i<<" : "<<faces.at(i).width << " , "<<faces.at(i).height;
    }

    // Variable for landmarks.
    // Landmarks for one face is a vector of points
    // There can be more than one face in the image. Hence, we
    // use a vector of vector of points.
    vector< vector<Point2f> > landmarks;

    // Run landmark detector
    bool success = facemark->fit(frame,faces,landmarks);

    if(success)
    {
        for(unsigned long i=0;i<faces.size();i++){
            for(unsigned long k=0;k<landmarks[i].size();k++)
                cv::circle(out,landmarks[i][k],5,cv::Scalar(0,0,255),FILLED);
        }
    }

    return out;
}

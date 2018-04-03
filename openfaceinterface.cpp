#include "openfaceinterface.h"

OpenFaceInterface::OpenFaceInterface(QObject *parent)
{
    faceDetector.load("haarcascade_frontalface_alt2.xml");
    facemark = FacemarkLBF::create();

    // Load landmark detector
    facemark->loadModel("lbfmodel.yaml");
}

static bool myDetector(InputArray image, OutputArray faces, CascadeClassifier *face_cascade)
{
    Mat gray;

    if (image.channels() > 1)
        cvtColor(image, gray, COLOR_BGR2GRAY);
    else
        gray = image.getMat().clone();

    equalizeHist(gray, gray);

    std::vector<Rect> faces_;
    face_cascade->detectMultiScale(gray, faces_, 1.4, 2, CASCADE_SCALE_IMAGE, Size(30, 30));
    Mat(faces_).copyTo(faces);
    return true;
}

Mat OpenFaceInterface::detectingLandmarks(Mat input)
{
    /*string filename = "lbfmodel.yaml";
    string cascade_name = "haarcascade_frontalface_alt.xml";

    Mat img;
    input.copyTo(img);

    //pass the face cascade xml file which you want to pass as a detector
    CascadeClassifier face_cascade;
    qDebug()<<"line1";
    face_cascade.load(cascade_name);
    qDebug()<<"line2";
    FacemarkKazemi::Params params;
    Ptr<FacemarkKazemi> facemark = FacemarkKazemi::create(params);
    qDebug()<<"line3";
    facemark->setFaceDetector((FN_FaceDetector)myDetector, &face_cascade);
    qDebug()<<"line4";
    facemark->loadModel(filename);
    qDebug()<<"line5";
    cout<<"Loaded model"<<endl;
    vector<Rect> faces;
    resize(img,img,Size(460,460), 0, 0, INTER_LINEAR_EXACT);
    qDebug()<<"line6";
    facemark->getFaces(img,faces);
    vector< vector<Point2f> > shapes;
    if(facemark->fit(img,faces,shapes))
    {
        for( size_t i = 0; i < faces.size(); i++ )
        {
            cv::rectangle(img,faces[i],Scalar( 255, 0, 0 ));
        }
        for(unsigned long i=0;i<faces.size();i++){
            for(unsigned long k=0;k<shapes[i].size();k++)
                cv::circle(img,shapes[i][k],5,cv::Scalar(0,0,255),FILLED);
        }
    }
    */

    // Variable to store a video frame and its grayscale
    Mat frame, gray;
    input.copyTo(frame);

    // Find face
    vector<Rect> faces;
    // Convert frame to grayscale because
    // faceDetector requires grayscale image.
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Detect faces
    faceDetector.detectMultiScale(gray, faces);

    // Variable for landmarks.
    // Landmarks for one face is a vector of points
    // There can be more than one face in the image. Hence, we
    // use a vector of vector of points.
    vector< vector<Point2f> > landmarks;

    // Run landmark detector
    bool success = facemark->fit(frame,faces,landmarks);

    if(success)
    {
        // If successful, render the landmarks on the face
//        for(int i = 0; i < landmarks.size(); i++)
//        {
//            drawLandmarks(frame, landmarks[i]);
//        }

        for(unsigned long i=0;i<faces.size();i++){
            for(unsigned long k=0;k<landmarks[i].size();k++)
                cv::circle(frame,landmarks[i][k],5,cv::Scalar(0,0,255),FILLED);
        }
    }

    // Display results
    //imshow("Facial Landmark Detection", frame);

    return frame;
}

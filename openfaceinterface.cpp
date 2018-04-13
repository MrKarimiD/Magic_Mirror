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
        for(unsigned long i=0;i<faces.size();i++)
        {
            voronoi_diagram(out, landmarks[i]);
            //for(unsigned long k=0;k<landmarks[i].size();k++)
                //cv::circle(out,landmarks[i][k],2,cv::Scalar(0,0,255),FILLED);
        }
    }

    rectangle(out, Point(0,0), Point(out.cols, out.rows), cv::Scalar(0,0,255), 5);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat gray2;
    cvtColor( out, gray2, CV_RGB2GRAY );
    findContours(gray2.clone(), contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<Point> approx;
    Mat dst = out.clone();
    RNG rng(12345);
    for (int i = 0; i < contours.size(); i++)
    {
        // Approximate contour with accuracy proportional
        // to the contour perimeter
       // approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        //drawContours(dst,contours, i, color, CV_FILLED, 4, hierarchy, 0);
        drawContours(dst,contours, i, color, CV_FILLED, 8, hierarchy, 0);
    }

    return dst;
}
void OpenFaceInterface::voronoi_diagram(Mat &input, vector<Point2f> landmarks)
{
    //consider some points
    std::vector<Point_2> points;
    for(int i=0;i<landmarks.size();i++)
    {
        points.push_back(Point_2(landmarks.at(i).x,landmarks.at(i).y));
    }

    Delaunay_triangulation_2 dt2;
    //insert points into the triangulation
    dt2.insert(points.begin(),points.end());

    //construct a rectangle
    Iso_rectangle_2 bbox(0, 0, input.cols, input.rows);
    Cropped_voronoi_from_delaunay vor(bbox);
    //extract the cropped Voronoi diagram
    dt2.draw_dual(vor);

    for(int i=0;i<vor.m_cropped_vd.size();i++)
    {
        Segment_2 tmp = vor.m_cropped_vd.at(i);
        Point_2 start = tmp.vertex(0) , end = tmp.vertex(1);
        line(input, Point(start.x(), start.y()), Point(end.x(), end.y()), cv::Scalar(0,0,255), 5);
    }
}

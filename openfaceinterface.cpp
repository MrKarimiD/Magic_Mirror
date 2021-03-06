#include "openfaceinterface.h"

OpenFaceInterface::OpenFaceInterface(QObject *parent)
{
    faceDetector.load("haarcascade_frontalface_alt2.xml");
    facemark = FacemarkLBF::create();

    srand (time(NULL));

    this->range = 0;

    // Load landmark detector
    facemark->loadModel("lbfmodel.yaml");
}

bool compareContourAreas( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 )
{
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i > j );
}

Mat OpenFaceInterface::detectingLandmarks(Mat input, bool random, bool useNormalFace)
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

        rectangle(out, Point(0,0), Point(out.cols, out.rows), cv::Scalar(0,0,255), 2);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        Mat gray2;
        cvtColor( out, gray2, CV_RGB2GRAY );
        findContours(gray2.clone(), contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE);

        //qDebug()<<"Contours size: "<< contours.size();

        // std::sort(contours.begin(), contours.end(), compareContourAreas);

        if( random )
        {
            Mat dst = out.clone();
            RNG rng(12345);
            for (int i = 0; i < contours.size(); i++)
            {
                Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                drawContours(dst, contours, i, color, CV_FILLED, CV_AA, hierarchy, 0);
            }
            dst.copyTo(out);
        }
        else
        {
            //  vector<Point> approx;
            Mat dst = out.clone();
            for (int i = 0; i < contours.size(); i++)
            {
                // Approximate contour with accuracy proportional
                // to the contour perimeter
                // approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                bool isInsideFaces = false;
                for(int j = 0; j < faces.size(); j++)
                {
                    RotatedRect rr = minAreaRect(contours.at(i));
                    if( faces.at(j).contains(rr.center) )
                        isInsideFaces = true;
                }

                if(isInsideFaces)
                {
                    Rect br = boundingRect(contours.at(i));
                    Scalar mean_color = mean( input(br) );
                    drawContours(dst, contours, i, mean_color, CV_FILLED, CV_AA, hierarchy, 0);
                }
                else
                    drawContours(dst, contours, i, getTheColor(), CV_FILLED, CV_AA, hierarchy, 0);
            }

            if( useNormalFace )
            {
                for(int j = 0; j < faces.size(); j++)
                {
                    Point2f center;
                    float radius;
                    minEnclosingCircle( landmarks.at(j), center, radius);
                    for(int x = faces.at(j).tl().x; x < faces.at(j).tl().x + faces.at(j).width; x++)
                    {
                        for(int y = faces.at(j).tl().y; y < faces.at(j).tl().y + faces.at(j).height; y++)
                        {
                            Point2f p(x,y);
                            Point diff = p - center;
                            if ( sqrt(diff.x*diff.x + diff.y*diff.y) < radius )
                            {
                                Vec3b color = input.at<Vec3b>(Point(x,y));
                                dst.at<Vec3b>(Point(x,y)) = color;
                            }
                        }
                    }
                    //ROI.copyTo(dst(faces.at(j)));
                }
            }
            dst.copyTo(out);
        }
    }
    return out;
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
        line(input, Point(start.x(), start.y()), Point(end.x(), end.y()), cv::Scalar(0,0,255), 1);
    }
}

void OpenFaceInterface::changeTheRange()
{
    this->range++;
    if(this->range >= 5)
        this->range = 0;
}

Scalar OpenFaceInterface::getTheColor()
{
    RNG rng(123456 * rand() % 10 + 123456);
    Scalar color;

    switch(this->range) {
    case 0 :
        //Red
        color = Scalar( rng.uniform(200, 255), rng.uniform(0,60), rng.uniform(0,60) );
        break;
    case 1 :
        //Magenta
        color = Scalar( rng.uniform(200, 255), rng.uniform(0,100), rng.uniform(200,255) );
        break;
    case 2 :
        //Green
        color = Scalar( rng.uniform(0, 60), rng.uniform(200,255), rng.uniform(0,60) );
        break;
    case 3 :
        //Orange
        color = Scalar( rng.uniform(200, 255), rng.uniform(70,150), rng.uniform(0,70) );
        break;
    case 4 :
        //Blue
        color = Scalar( rng.uniform(0, 60), rng.uniform(0,60), rng.uniform(200,255) );
        break;
    }

    return color;
}



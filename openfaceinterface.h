#ifndef OPENFACEINTERFACE_H
#define OPENFACEINTERFACE_H

#define MAXOFCONTOURS 2000

#include <QObject>
#include <QDebug>

#include "opencv2/face.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <iostream>
#include <vector>
#include <string>
#include <time.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Iso_rectangle_2 Iso_rectangle_2;
typedef K::Segment_2 Segment_2;
typedef K::Ray_2 Ray_2;
typedef K::Line_2 Line_2;
typedef CGAL::Delaunay_triangulation_2<K>  Delaunay_triangulation_2;

//A class to recover Voronoi diagram from stream.
//Rays, lines and segments are cropped to a rectangle
//so that only segments are stored
struct Cropped_voronoi_from_delaunay
{
    std::vector<Segment_2> m_cropped_vd;
    Iso_rectangle_2 m_bbox;

    Cropped_voronoi_from_delaunay(const Iso_rectangle_2& bbox):m_bbox(bbox){}

    template <class RSL>
    void crop_and_extract_segment(const RSL& rsl){
        CGAL::Object obj = CGAL::intersection(rsl,m_bbox);
        const Segment_2* s=CGAL::object_cast<Segment_2>(&obj);
        if (s) m_cropped_vd.push_back(*s);
    }

    void operator<<(const Ray_2& ray)    { crop_and_extract_segment(ray); }
    void operator<<(const Line_2& line)  { crop_and_extract_segment(line); }
    void operator<<(const Segment_2& seg){ crop_and_extract_segment(seg); }
};

using namespace std;
using namespace cv;
using namespace cv::face;

class OpenFaceInterface : public QObject
{
    Q_OBJECT
public:
    explicit OpenFaceInterface(QObject *parent = nullptr);
    Mat detectingLandmarks(Mat input, bool random, bool useNormalFace);
    //bool myDetector(InputArray image, OutputArray faces);
    void voronoi_diagram(Mat &input, vector<Point2f> landmarks);
    void changeTheRange();
    cv::Scalar getTheColor();

signals:

public slots:

private:
    CascadeClassifier faceDetector;
    Ptr<Facemark> facemark;
    int range;
};

#endif // OPENFACEINTERFACE_H

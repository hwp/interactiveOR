// saldet.h
// 
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#ifndef ___SALDET_SALDET_H_
#define ___SALDET_SALDET_H_

#include <opencv2/core/core.hpp>

#include <string>
#include <vector>

using cv::Vec;
using cv::Mat;
using cv::Point2d;
using cv::Point_;

using std::string;
using std::vector;
using std::pair;

typedef vector<string> vecS;
typedef vector<int> vecI;
typedef vector<float> vecF;
typedef vector<double> vecD;
typedef pair<double, int> CostIdx;
typedef pair<float, int> CostfIdx;
typedef Mat (*SAL_FUNC)(const Mat&);

const int SAL_TYPE_NUM = 5;

template<typename T> inline T sqr(T x) {
  return x * x;
}

template<class T> inline T vecDist3(const Vec<T, 3>& v1, const Vec<T, 3>& v2) {
  return sqrt(sqr(v1[0] - v2[0])+sqr(v1[1] - v2[1])+sqr(v1[2] - v2[2]));
}

template<class T> inline T vecSqrDist3(const Vec<T, 3>& v1, const Vec<T, 3>& v2) {
  return sqr(v1[0] - v2[0])+sqr(v1[1] - v2[1])+sqr(v1[2] - v2[2]);
}

/*template<class T1, class T2> inline void operator /= (Vec<T1, 3>& v1, const T2 v2) {
  v1[0] /= v2; v1[1] /= v2; v1[2] /= v2;
}*/

template<class T> inline T pntSqrDist(const Point_<T>& p1, const Point_<T>& p2) {
  return sqr(p1.x - p2.x) + sqr(p1.y - p2.y);
}

Mat getFT(const Mat& img3f);
Mat getHC(const Mat& img3f);
void getHC(const Mat& binColor3f, const Mat& weights1f, Mat& colorSaliency);
Mat getLC(const Mat& img3f);
Mat getSR(const Mat& img3f);
Mat getRC(const Mat& img3f);
Mat getRC(const Mat& img3f, double sigmaDist, double segK, int segMinSize, double segSigma);

int quantize(const Mat& img3f, Mat& idx1i, Mat& _color3f, Mat& _colorNum, double ratio = 0.95);
void smoothSaliency(const Mat& binColor3f, Mat& sal1d, float delta, const vector<vector<CostfIdx> >& similar);
void absAngle(const Mat& cmplx32FC2, Mat& mag32FC1, Mat& ang32FC1);
void getCmplx(const Mat& mag32F, const Mat& ang32F, Mat& cmplx32FC2);


struct Region{
  Region() { pixNum = 0;}
  int pixNum;  // Number of pixels
  vector<CostfIdx> freIdx;  // Frequency of each color and its index
  Point2d centroid;
};

void buildRegions(const Mat& regIdx1i, vector<Region>& regs, const Mat& colorIdx1i, int colorNum);
void regionContrast(const vector<Region>& regs, const Mat& color3fv, Mat& regSal1d, double sigmaDist);

#endif  // ___SALDET_SALDET_H_


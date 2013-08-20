// saldet.cpp
// Salient Object Detection
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#include "saldet.h"
#include "segment-image.h"

#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <map>

using namespace cv;

using std::vector;
using std::map;
using std::make_pair;

const SAL_FUNC gFuns[SAL_TYPE_NUM] = {getRC, getHC, getFT, getLC, getSR};

Mat getRC(const Mat& img3f) {
  return getRC(img3f, 0.4, 50, 50, 0.5);
}

Mat getRC(const Mat& img3f, double sigmaDist, double segK, int segMinSize, double segSigma) {
  Mat regIdx1i, colorIdx1i, regSal1v, tmp, _img3f, color3fv;
  if (quantize(img3f, colorIdx1i, color3fv, tmp) <= 2)  // Color quantization
    return Mat::zeros(img3f.size(), CV_32F);
  cvtColor(img3f, _img3f, CV_BGR2Lab);
  cvtColor(color3fv, color3fv, CV_BGR2Lab);
  int regNum = SegmentImage(_img3f, regIdx1i, segSigma, segK, segMinSize);
  vector<Region> regs(regNum);
  buildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
  regionContrast(regs, color3fv, regSal1v, sigmaDist);

  Mat sal1f = Mat::zeros(img3f.size(), CV_32F);
  normalize(regSal1v, regSal1v, 0, 1, NORM_MINMAX, CV_32F);
  float* regSal = (float*)regSal1v.data;
  for (int r = 0; r < img3f.rows; r++) {
    const int* regIdx = regIdx1i.ptr<int>(r);
    float* sal = sal1f.ptr<float>(r);
    for (int c = 0; c < img3f.cols; c++)
      sal[c] = regSal[regIdx[c]];
  }
  GaussianBlur(sal1f, sal1f, Size(3, 3), 0);
  return sal1f;
}

Mat getHC(const Mat& img3f) {
	// Quantize colors and
	Mat idx1i, binColor3f, colorNums1i, weight1f, _colorSal;
	quantize(img3f, idx1i, binColor3f, colorNums1i);
	cvtColor(binColor3f, binColor3f, CV_BGR2Lab);

	normalize(colorNums1i, weight1f, 1, 0, NORM_L1, CV_32F);
	getHC(binColor3f, weight1f, _colorSal);
	float* colorSal = (float*)(_colorSal.data);
	Mat salHC1f(img3f.size(), CV_32F);
	for (int r = 0; r < img3f.rows; r++)
	{
		float* salV = salHC1f.ptr<float>(r);
		int* _idx = idx1i.ptr<int>(r);
		for (int c = 0; c < img3f.cols; c++)
			salV[c] = colorSal[_idx[c]];
	}
	GaussianBlur(salHC1f, salHC1f, Size(3, 3), 0);
	normalize(salHC1f, salHC1f, 0, 1, NORM_MINMAX);
	return salHC1f;
}

void getHC(const Mat& binColor3f, const Mat& weight1f, Mat& _colorSal) {
	int binN = binColor3f.cols; 
	_colorSal = Mat::zeros(1, binN, CV_32F);
	float* colorSal = (float*)(_colorSal.data);
	vector<vector<CostfIdx> > similar(binN); // Similar color: how similar and their index
	Vec3f* color = (Vec3f*)(binColor3f.data);
	float *w = (float*)(weight1f.data);
	for (int i = 0; i < binN; i++)
	{
		vector<CostfIdx>& similari = similar[i];
		similari.push_back(make_pair(0.f, i));
		for (int j = 0; j < binN; j++)
		{
			if (i == j)
				continue;
			float dij = vecDist3<float>(color[i], color[j]);
			similari.push_back(make_pair(dij, j));
			colorSal[i] += w[j] * dij;
		}
		sort(similari.begin(), similari.end());
	}

	smoothSaliency(binColor3f, _colorSal, 4.0f, similar);
}

Mat getFT(const Mat& img3f) {
	CV_Assert(img3f.data != NULL && img3f.type() == CV_32FC3);
	Mat sal(img3f.size(), CV_32F), tImg;
	GaussianBlur(img3f, tImg, Size(3, 3), 0);
	cvtColor(tImg, tImg, CV_BGR2Lab);
	Scalar colorM = mean(tImg);
	for (int r = 0; r < tImg.rows; r++)
	{
		float *s = sal.ptr<float>(r);
		float *lab = tImg.ptr<float>(r);
		for (int c = 0; c < tImg.cols; c++, lab += 3)
			s[c] = (float)(sqr(colorM[0] - lab[0]) + sqr(colorM[1] - lab[1]) + sqr(colorM[2] - lab[2]));
	}
	normalize(sal, sal, 0, 1, NORM_MINMAX);
	return sal;
}

Mat getSR(const Mat &img3f) {
	Size sz(64, 64);
	Mat img1f[2], sr1f, cmplxSrc2f, cmplxDst2f;
	cvtColor(img3f, img1f[1], CV_BGR2GRAY);
	resize(img1f[1], img1f[0], sz, 0, 0, CV_INTER_AREA); 

	img1f[1] = Mat::zeros(sz, CV_32F);
	merge(img1f, 2, cmplxSrc2f);
	dft(cmplxSrc2f, cmplxDst2f);
	absAngle(cmplxDst2f, img1f[0], img1f[1]);

	log(img1f[0], img1f[0]);
	blur(img1f[0], sr1f, Size(3, 3));
	sr1f = img1f[0] - sr1f;

	exp(sr1f, sr1f);
	getCmplx(sr1f, img1f[1], cmplxDst2f);
	dft(cmplxDst2f, cmplxSrc2f, DFT_INVERSE | DFT_SCALE);
	split(cmplxSrc2f, img1f);

	pow(img1f[0], 2, img1f[0]);
	pow(img1f[1], 2, img1f[1]);
	img1f[0] += img1f[1];

	GaussianBlur(img1f[0], img1f[0], Size(3, 3), 0);
	normalize(img1f[0], img1f[0], 0, 1, NORM_MINMAX);
	resize(img1f[0], img1f[1], img3f.size(), 0, 0, INTER_CUBIC);

	return img1f[1];
}

Mat getLC(const Mat& img3f) {
	Mat img;
	cvtColor(img3f, img, CV_BGR2GRAY);
	img.convertTo(img, CV_8U, 255);
	double f[256], s[256];
	memset(f, 0, 256*sizeof(double));
	memset(s, 0, 256*sizeof(double));
	for (int r = 0; r < img.rows; r++)
	{
		unsigned char* data = img.ptr<unsigned char>(r);
		for (int c = 0; c < img.cols; c++)
			f[data[c]] += 1;
	}
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 256; j++)
			s[i] += abs(i - j) * f[j];
	Mat sal1f(img3f.size(), CV_64F);
	for (int r = 0; r < img.rows; r++)
	{
		unsigned char* data = img.ptr<unsigned char>(r);
		double* sal = sal1f.ptr<double>(r);
		for (int c = 0; c < img.cols; c++)
			sal[c] = s[data[c]];
	}
	normalize(sal1f, sal1f, 0, 1, NORM_MINMAX, CV_32F);
	return sal1f;
}



void smoothSaliency(const Mat& binColor3f, Mat& sal1d, float delta, const vector<vector<CostfIdx> >& similar) {
	if (sal1d.cols < 2)
		return;
	CV_Assert(binColor3f.size() == sal1d.size() && sal1d.rows == 1);
	int binN = binColor3f.cols;
	Vec3f* color = (Vec3f*)(binColor3f.data);
	Mat tmpSal;
	sal1d.copyTo(tmpSal);
	float *sal = (float*)(tmpSal.data);
	float *nSal = (float*)(sal1d.data);

	//* Distance based smooth
	int n = max(cvRound(binN/delta), 2);
	vecF dist(n, 0), val(n);
	for (int i = 0; i < binN; i++)
	{
		const vector<CostfIdx> &similari = similar[i];
		float totalDist = 0;

		val[0] = sal[i];
		for (int j = 1; j < n; j++)
		{
			int ithIdx =similari[j].second;
			dist[j] = similari[j].first;
			val[j] = sal[ithIdx];
			totalDist += dist[j];
		}
		float valCrnt = 0;
		for (int j = 0; j < n; j++)
			valCrnt += val[j] * (totalDist - dist[j]);

		nSal[i] =  valCrnt / ((n-1) * totalDist);
	}	
	//*/

	/* Gaussian smooth
	const float guassCoeff = -0.5f/(delta*delta);
	for (int i = 0; i < binN; i++)
	{
		const vector<CostfIdx> &similari = similar[i];
		float saliencyI = sal[i], totalW = 1;

		for (int j = 1; j < binN; j++)
		{
			float w = expf(sqr(similari[j].first)*guassCoeff);
			if (w < 1e-8f)
				break;
			saliencyI += w * sal[similari[j].second];
			totalW += w;
		}
		nSal[i] = saliencyI / totalW;
	}
	//*/
}



int quantize(const Mat& img3f, Mat& idx1i, Mat& _color3f, Mat& _colorNum, double ratio)
{
  static const int clrNums[3] = {12, 12, 12};
  static const float clrTmp[3] = {clrNums[0] - 0.0001f, clrNums[1] - 0.0001f, clrNums[2] - 0.0001f};
  static const int w[3] = {clrNums[1] * clrNums[2], clrNums[2], 1};

  CV_Assert(img3f.data != NULL);
  idx1i = Mat::zeros(img3f.size(), CV_32S);
  int rows = img3f.rows, cols = img3f.cols;
  if (img3f.isContinuous() && idx1i.isContinuous())
  {
    cols *= rows;
    rows = 1;
  }

  // Build color pallet
  map<int, int> pallet;
  for (int y = 0; y < rows; y++)
  {
    const float* imgData = img3f.ptr<float>(y);
    int* idx = idx1i.ptr<int>(y);
    for (int x = 0; x < cols; x++, imgData += 3)
    {
      idx[x] = (int)(imgData[0]*clrTmp[0])*w[0] + (int)(imgData[1]*clrTmp[1])*w[1] + (int)(imgData[2]*clrTmp[2]);
      pallet[idx[x]] ++;
    }
  }

  // Fine significant colors
  int maxNum = 0;
  {
    int count = 0;
    vector<pair<int, int> > num; // (num, color) pairs in num
    num.reserve(pallet.size());
    for (map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++)
      num.push_back(pair<int, int>(it->second, it->first)); // (color, num) pairs in pallet
    sort(num.begin(), num.end(), std::greater<pair<int, int> >());

    maxNum = (int)num.size();
    int maxDropNum = cvRound(rows * cols * (1-ratio));
    for (int crnt = num[maxNum-1].first; crnt < maxDropNum && maxNum > 1; maxNum--)
      crnt += num[maxNum - 2].first;
    maxNum = min(maxNum, 256); // To avoid very rarely case
    if (maxNum < 10)
      maxNum = min((int)pallet.size(), 100);
    pallet.clear();
    for (int i = 0; i < maxNum; i++)
      pallet[num[i].second] = i;

    vector<Vec3i> color3i(num.size());
    for (unsigned int i = 0; i < num.size(); i++)
    {
      color3i[i][0] = num[i].second / w[0];
      color3i[i][1] = num[i].second % w[0] / w[1];
      color3i[i][2] = num[i].second % w[1];
    }

    for (unsigned int i = maxNum; i < num.size(); i++)
    {
      int simIdx = 0, simVal = INT_MAX;
      for (int j = 0; j < maxNum; j++)
      {
        int d_ij = vecSqrDist3(color3i[i], color3i[j]);
        if (d_ij < simVal)
          simVal = d_ij, simIdx = j;
      }
      pallet[num[i].second] = pallet[num[simIdx].second];
    }
  }

  _color3f = Mat::zeros(1, maxNum, CV_32FC3);
  _colorNum = Mat::zeros(_color3f.size(), CV_32S);

  Vec3f* color = (Vec3f*)(_color3f.data);
  int* colorNum = (int*)(_colorNum.data);
  for (int y = 0; y < rows; y++)
  {
    const Vec3f* imgData = img3f.ptr<Vec3f>(y);
    int* idx = idx1i.ptr<int>(y);
    for (int x = 0; x < cols; x++)
    {
      idx[x] = pallet[idx[x]];
      color[idx[x]] += imgData[x];
      colorNum[idx[x]] ++;
    }
  }
  for (int i = 0; i < _color3f.cols; i++)
    color[i] /= (double)colorNum[i];

  return _color3f.cols;
}

void buildRegions(const Mat& regIdx1i, vector<Region>& regs, const Mat& colorIdx1i, int colorNum) {
  int rows = regIdx1i.rows, cols = regIdx1i.cols, regNum = (int)regs.size();
  Mat_<int> regColorFre1i = Mat_<int>::zeros(regNum, colorNum); // region color frequency
  for (int y = 0; y < rows; y++) {
    const int *regIdx = regIdx1i.ptr<int>(y);
    const int *colorIdx = colorIdx1i.ptr<int>(y);
    for (int x = 0; x < cols; x++, regIdx++, colorIdx++) {
      Region& reg = regs[*regIdx];
      reg.pixNum ++;
      reg.centroid.x += x;
      reg.centroid.y += y;
      regColorFre1i(*regIdx, *colorIdx)++;
    }
  }

  for (int i = 0; i < regNum; i++) {
    Region& reg = regs[i];
    reg.centroid.x /= reg.pixNum * cols;
    reg.centroid.y /= reg.pixNum * rows;
    int *regColorFre = regColorFre1i.ptr<int>(i);
    for (int j = 0; j < colorNum; j++) {
      float fre = (float)regColorFre[j]/(float)reg.pixNum;
      if (regColorFre[j])
        reg.freIdx.push_back(make_pair(fre, j));
    }
  }
}

void regionContrast(const vector<Region>& regs, const Mat& color3fv, Mat& regSal1d, double sigmaDist) {
  Mat_<float> cDistCache1f = Mat::zeros(color3fv.cols, color3fv.cols, CV_32F);
  {
    Vec3f* pColor = (Vec3f*)color3fv.data;
    for(int i = 0; i < cDistCache1f.rows; i++)
      for(int j= i+1; j < cDistCache1f.cols; j++)
        cDistCache1f[i][j] = cDistCache1f[j][i] = vecDist3(pColor[i], pColor[j]);
  }

  int regNum = (int)regs.size();
  Mat_<double> rDistCache1d = Mat::zeros(regNum, regNum, CV_64F);
  regSal1d = Mat::zeros(1, regNum, CV_64F);
  double* regSal = (double*)regSal1d.data;
  for (int i = 0; i < regNum; i++) {
    for (int j = 0; j < regNum; j++) {
      if(i<j) {
        double dd = 0;
        const vector<CostfIdx>& c1 = regs[i].freIdx;
        const vector<CostfIdx>& c2 = regs[j].freIdx;
        for (size_t m = 0; m < c1.size(); m++)
          for (size_t n = 0; n < c2.size(); n++)
            dd += cDistCache1f[c1[m].second][c2[n].second] * c1[m].first * c2[n].first;
        rDistCache1d[j][i] = rDistCache1d[i][j] = dd * exp(-pntSqrDist(regs[i].centroid, regs[j].centroid)/sigmaDist);
      }
      regSal[i] += regs[j].pixNum * rDistCache1d[i][j];
    }
  }
}

void absAngle(const Mat& cmplx32FC2, Mat& mag32FC1, Mat& ang32FC1) {
	CV_Assert(cmplx32FC2.type() == CV_32FC2);
	mag32FC1.create(cmplx32FC2.size(), CV_32FC1);
	ang32FC1.create(cmplx32FC2.size(), CV_32FC1);

	for (int y = 0; y < cmplx32FC2.rows; y++)
	{
		const float* cmpD = cmplx32FC2.ptr<float>(y);
		float* dataA = ang32FC1.ptr<float>(y);
		float* dataM = mag32FC1.ptr<float>(y);
		for (int x = 0; x < cmplx32FC2.cols; x++, cmpD += 2)
		{
			dataA[x] = atan2(cmpD[1], cmpD[0]);
			dataM[x] = sqrt(cmpD[0] * cmpD[0] + cmpD[1] * cmpD[1]);
		}
	}
}

void getCmplx(const Mat& mag32F, const Mat& ang32F, Mat& cmplx32FC2) {
	CV_Assert(mag32F.type() == CV_32FC1 && ang32F.type() == CV_32FC1 && mag32F.size() == ang32F.size());
	cmplx32FC2.create(mag32F.size(), CV_32FC2);
	for (int y = 0; y < mag32F.rows; y++)
	{
		float* cmpD = cmplx32FC2.ptr<float>(y);
		const float* dataA = ang32F.ptr<float>(y);
		const float* dataM = mag32F.ptr<float>(y);
		for (int x = 0; x < mag32F.cols; x++, cmpD += 2)
		{
			cmpD[0] = dataM[x] * cos(dataA[x]);
			cmpD[1] = dataM[x] * sin(dataA[x]);
		}
	}
}




int main(int argc, char* argv[]) {
  if (argc != 3 && argc != 4) {
    printf("Usage\n\t%s <imput> <output>\n", argv[0]);
    return 1;
  }

  Mat img = imread(argv[1]);
  img.convertTo(img, CV_32FC3, 1.0/255);
  SAL_FUNC f = getRC;
  if (argc == 4) {
    f = gFuns[atoi(argv[3])];
  }
  Mat tag = f(img) * 255;
  imwrite(argv[2], tag);

  return 0;
}


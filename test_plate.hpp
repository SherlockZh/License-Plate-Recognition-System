#pragma once
#ifndef EASYPR_PLATE_HPP
#define EASYPR_PLATE_HPP

#include "api.hpp"

using namespace cv;
using namespace std;
using namespace easypr;

int test_plate_locate(const string file)
{
	Mat src = imread(file);

	vector<Mat> resultVec;
	CPlateLocate plate;
	plate.setDebug(1);
	plate.setLifemode(true);

	int result = plate.plateLocate(src, resultVec);
	if (result == 0)
	{
		size_t num = resultVec.size();
		for (size_t j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("plate_locate", resultMat);
			waitKey(0);
		}
		destroyWindow("plate_locate");
	}

	return result;
}

int test_plate_judge(const string file)
{

	Mat src = imread(file);

	vector<Mat> matVec;
	vector<Mat> resultVec;

	CPlateLocate lo;
	lo.setDebug(0);
	lo.setLifemode(true);

	int resultLo = lo.plateLocate(src, matVec);

	if (0 != resultLo) return -1;

	cout << "plate_locate_img" << endl;
	size_t num = matVec.size();
	for (size_t j = 0; j < num; j++) 
	{
		Mat resultMat = matVec[j];
		imshow("plate_judge", resultMat);
		waitKey(0);
	}
	destroyWindow("plate_judge");

	int resultJu = PlateJudge::instance()->plateJudge(matVec, resultVec);

	if (0 != resultJu) return -1;

	cout << "plate_judge_img" << endl;
	num = resultVec.size();
	for (size_t j = 0; j < num; j++) 
	{
		Mat resultMat = resultVec[j];
		imshow("plate_judge", resultMat);
		waitKey(0);
	}
	destroyWindow("plate_judge");

	return resultJu;
}

int test_plate_detect(const string file)
{
	Mat src = imread(file);

	vector<CPlate> resultVec;
	CPlateDetect pd;
	pd.setPDLifemode(true);

	int result = pd.plateDetect(src, resultVec);
	if (result == 0)
	{
		size_t num = resultVec.size();
		for (size_t j = 0; j < num; j++)
		{
			CPlate resultMat = resultVec[j];
			imshow("plate_detect", resultMat.getPlateMat());
			waitKey(0);
		}
		destroyWindow("plate_detect");
	}

	return result;
}

int test_plate_recognize(const string file) 
{
	Mat src = imread(file);

	CPlateRecognize pr;
	pr.setLifemode(true);
	pr.setDebug(false);
	pr.setMaxPlates(4);
	pr.setDetectType(PR_DETECT_COLOR | PR_DETECT_SOBEL | PR_DETECT_CMSER);

	vector<CPlate> plateVec;

	int result = pr.plateRecognize(src, plateVec);
	if (result == 0)
	{
		size_t num = plateVec.size();
		for (size_t j = 0; j < num; j++)
		{
			cout << "plateRecognize: " << plateVec[j].getPlateStr() << endl;
		}
	}

	if (result != 0) cout << "result:" << result << endl;

	return result;
}

int test_chars_segment(const string file)
{
	Mat src = imread(file);

	vector<Mat> resultVec;
	CCharsSegment plate;

	int result = plate.charsSegment(src, resultVec);
	if (result == 0)
	{
		size_t num = resultVec.size();
		for (size_t j = 0; j < num; j++) 
		{
			Mat resultMat = resultVec[j];
			imshow("chars_segment", resultMat);
			waitKey(0);
		}
		destroyWindow("chars_segment");
	}

	return result;
}

int test_chars_identify(const string file)
{

	Mat plate = imread(file);

	vector<Mat> matChars;
	string license;

	CCharsSegment cs;

	int result = cs.charsSegment(plate, matChars);
	if (result == 0)
	{
		for (size_t block_i = 0; block_i < matChars.size(); block_i++)
		{
			auto block = matChars[block_i];
			if (block_i == 0)
			{
				auto character = CharsIdentify::instance()->identify(block, true);
				license.append(character.second);
			}
			else
			{
				auto character = CharsIdentify::instance()->identify(block, false);
				license.append(character.second);
			}
		}
		//for (auto block : matChars) {
		//  auto character = CharsIdentify::instance()->identify(block);
		//  license.append(character.second);
		//}
	}

	string plateLicense = "À’E771H6";
#ifdef OS_WINDOWS
	plateLicense = utils::utf8_to_gbk(plateLicense.c_str());
#endif
	cout << "plateLicense: " << plateLicense << endl;
	cout << "plateIdentify: " << license << endl;

	if (plateLicense != license) {
		cout << "Identify Not Correct!" << endl;
		return -1;
	}
	cout << "Identify Correct!" << endl;

	return result;
}

int test_chars_recognise(const string file)
{
	Mat src = imread(file);
	CCharsRecognise cr;
	return 0;
	string plateLicense = "";
	int result = cr.charsRecognise(src, plateLicense);
	if (result == 0)
		cout << "charsRecognise: " << plateLicense << endl;
	return 0;
}

QImage  Mat2QImage(cv::Mat cvImg)
{
	QImage qImg;
	if (cvImg.channels() == 3)                             //3 channels color image
	{

		cv::cvtColor(cvImg, cvImg, CV_BGR2RGB);
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_RGB888);
	}
	else if (cvImg.channels() == 1)                    //grayscale image
	{
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_Indexed8);
	}
	else
	{
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_RGB888);
	}

	return qImg;

}
#endif  // EASYPR_PLATE_HPP

#include "PRQT.h"
#include "qfiledialog.h"
#include "api.hpp"
#include "test_plate.hpp"


using namespace easypr;
using namespace std;
using namespace cv;

static string fileName;

PRQT::PRQT(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void PRQT::slot_LoadImage()
{
	QString imageFile = QFileDialog::getOpenFileName(this,  tr("open image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmp *.tif *.tiff)"));
	fileName = imageFile.toLocal8Bit().constData();

	QImage* img = new QImage;

	if (!(img->load(imageFile))) //º”‘ÿÕºœÒ
	{
		QMessageBox::information(this,
			tr("¥Úø™ÕºœÒ ß∞‹"),
			tr("¥Úø™ÕºœÒ ß∞‹!"));
		delete img;
		return;
	}

	ui.label_ImageSrc->setPixmap(QPixmap::fromImage(*img));
}

void PRQT::slot_plate_locate()
{
	Mat src = imread(fileName);

	vector<Mat> resultVec;
	CPlateLocate plate;
	plate.setDebug(0);
	plate.setLifemode(true);

	int result = plate.plateLocate(src, resultVec);
	size_t num = resultVec.size();
	//if (result == 0)
	//{
	//	size_t num = resultVec.size();
	//	for (size_t j = 0; j < num; j++)
	//	{
	//		Mat resultMat = resultVec[j];
	//		imshow("plate_locate", resultMat);
	//		waitKey(0);
	//	}
	//	destroyWindow("plate_locate");
	//}
	display(num, resultVec);
}

void PRQT::slot_plate_detect()
{
	Mat src = imread(fileName);

	vector<CPlate> resultVec;
	vector<Mat> resultMat;
	CPlateDetect pd;
	pd.setPDLifemode(true);

	int result = pd.plateDetect(src, resultVec);
	size_t num = resultVec.size();
	if (result == 0)
	{
		for (size_t j = 0; j < num; j++)
		{
			resultMat.push_back(resultVec[j].getPlateMat());
		}
	}
	
	display(num, resultMat);
}

void PRQT::slot_plate_judge()
{
	Mat src = imread(fileName);

	vector<Mat> matVec;
	vector<Mat> resultVec;
	vector<Mat> resultMat;
	CPlateLocate lo;
	lo.setDebug(0);
	lo.setLifemode(true);

	int resultLo = lo.plateLocate(src, matVec);

	if (0 != resultLo) return;

	int resultJu = PlateJudge::instance()->plateJudge(matVec, resultVec);

	if (0 != resultJu) return;

	auto num = resultVec.size();
	for (size_t j = 0; j < num; j++)
	{
		resultMat.push_back(resultVec[j]);

	}
	display(num, resultMat);
}

void PRQT::slot_plate_recognize()
{
	Mat src = imread(fileName);

	CPlateRecognize pr;
	pr.setLifemode(true);
	pr.setDebug(false);
	pr.setMaxPlates(2);
	pr.setDetectType(/*PR_DETECT_SOBEL|*/ PR_DETECT_COLOR | PR_DETECT_CMSER);
	
	vector<CPlate> plateVec;
	vector<Mat> resultMat;
	string licenseUtf8Str;

	int result = pr.plateRecognize(src, plateVec);
	if (result == 0)
	{
		size_t num = plateVec.size();
		for (size_t j = 0; j < num; j++)
		{
			licenseUtf8Str = plateVec[j].getPlateStr() + "\n" + licenseUtf8Str;
		}
		for (size_t i = 0; i < num; i++)
		{
			resultMat.push_back(plateVec[i].getPlateMat());
		}
	}
	QString license = QString::fromLocal8Bit(licenseUtf8Str.c_str());
	ui.label_license->setText(license);
	
	display(resultMat.size(), resultMat);
}

void PRQT::slot_chars_segment()
{
	Mat src = imread(fileName);

	vector<Mat> resultVec;
	vector<Mat> resultMat;
	CCharsSegment plate;

	int result = plate.charsSegment(src, resultVec);
	size_t num = resultVec.size();
	if (result == 0) 
	{
		for (size_t j = 0; j < num; j++) 
		{
			resultMat.push_back(resultVec[j]);
		}
		destroyWindow("chars_segment");
	}
	display(num, resultMat);
}

void PRQT::slot_chars_identify()
{
	Mat plate = imread("resources/image/chars_identify.jpg");

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
	}

	string plateLicense = "À’E771H6";
#ifdef OS_WINDOWS
	plateLicense = utils::utf8_to_gbk(plateLicense.c_str());
#endif
	cout << "plateLicense: " << plateLicense << endl;
	cout << "plateIdentify: " << license << endl;

	if (plateLicense != license) {
		cout << "Identify Not Correct!" << endl;
		return;
	}
}

void PRQT::slot_chars_recognise()
{
	Mat src = imread(fileName);
	CCharsRecognise cr;
	string plateLicense = "";
	int resultRec = cr.charsRecognise(src, plateLicense);

	QString license = QString::fromLocal8Bit(plateLicense.c_str());
	ui.label_license->setText(license);

	vector<Mat> resultVec;
	vector<Mat> resultMat;
	CCharsSegment plate;

	int result = plate.charsSegment(src, resultVec);
	size_t num = resultVec.size();
	if (result == 0)
	{
		for (size_t j = 0; j < num; j++)
		{
			resultMat.push_back(resultVec[j]);
		}
		destroyWindow("chars_segment");
	}
	display(num, resultMat);
}

void PRQT::display(size_t n, const vector<Mat> &mat)
{
	ui.label_ImageDst_0->clear();
	ui.label_ImageDst_1->clear();
	ui.label_ImageDst_2->clear();
	ui.label_ImageDst_3->clear();
	ui.label_ImageDst_4->clear();
	ui.label_ImageDst_5->clear();
	ui.label_ImageDst_6->clear();
	ui.label_ImageDst_7->clear();
	ui.label_ImageDst_8->clear();
	ui.label_ImageDst_9->clear();
	ui.label_ImageDst_10->clear();
	ui.label_ImageDst_11->clear();
	ui.label_ImageDst_12->clear();
	ui.label_ImageDst_13->clear();
	ui.label_ImageDst_14->clear();
	auto num = n;
	if (num == 0)
		return;
	else if (num == 1)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
	}
	else if (num == 2)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
	}
	else if (num == 3)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
	}
	else if (num == 4)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
	}
	else if (num == 5)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
	}
	else if (num == 6)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
	}
	else if (num == 7)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
	}
	else if (num == 8)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
	}
	else if (num == 9)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
	}
	else if (num == 10)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
		ui.label_ImageDst_9->setPixmap(QPixmap::fromImage(Mat2QImage(mat[9])));
	}
	else if (num == 11)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
		ui.label_ImageDst_9->setPixmap(QPixmap::fromImage(Mat2QImage(mat[9])));
		ui.label_ImageDst_10->setPixmap(QPixmap::fromImage(Mat2QImage(mat[10])));
	}
	else if (num == 12)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
		ui.label_ImageDst_9->setPixmap(QPixmap::fromImage(Mat2QImage(mat[9])));
		ui.label_ImageDst_10->setPixmap(QPixmap::fromImage(Mat2QImage(mat[10])));
		ui.label_ImageDst_11->setPixmap(QPixmap::fromImage(Mat2QImage(mat[11])));
	}
	else if (num == 13)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
		ui.label_ImageDst_9->setPixmap(QPixmap::fromImage(Mat2QImage(mat[9])));
		ui.label_ImageDst_10->setPixmap(QPixmap::fromImage(Mat2QImage(mat[10])));
		ui.label_ImageDst_11->setPixmap(QPixmap::fromImage(Mat2QImage(mat[11])));
		ui.label_ImageDst_12->setPixmap(QPixmap::fromImage(Mat2QImage(mat[12])));
	}
	else if (num == 14)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
		ui.label_ImageDst_9->setPixmap(QPixmap::fromImage(Mat2QImage(mat[9])));
		ui.label_ImageDst_10->setPixmap(QPixmap::fromImage(Mat2QImage(mat[10])));
		ui.label_ImageDst_11->setPixmap(QPixmap::fromImage(Mat2QImage(mat[11])));
		ui.label_ImageDst_12->setPixmap(QPixmap::fromImage(Mat2QImage(mat[12])));
		ui.label_ImageDst_13->setPixmap(QPixmap::fromImage(Mat2QImage(mat[13])));
	}
	else if (num == 15)
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
		ui.label_ImageDst_9->setPixmap(QPixmap::fromImage(Mat2QImage(mat[9])));
		ui.label_ImageDst_10->setPixmap(QPixmap::fromImage(Mat2QImage(mat[10])));
		ui.label_ImageDst_11->setPixmap(QPixmap::fromImage(Mat2QImage(mat[11])));
		ui.label_ImageDst_12->setPixmap(QPixmap::fromImage(Mat2QImage(mat[12])));
		ui.label_ImageDst_13->setPixmap(QPixmap::fromImage(Mat2QImage(mat[13])));
		ui.label_ImageDst_14->setPixmap(QPixmap::fromImage(Mat2QImage(mat[14])));
	}
	else
	{
		ui.label_ImageDst_0->setPixmap(QPixmap::fromImage(Mat2QImage(mat[0])));
		ui.label_ImageDst_1->setPixmap(QPixmap::fromImage(Mat2QImage(mat[1])));
		ui.label_ImageDst_2->setPixmap(QPixmap::fromImage(Mat2QImage(mat[2])));
		ui.label_ImageDst_3->setPixmap(QPixmap::fromImage(Mat2QImage(mat[3])));
		ui.label_ImageDst_4->setPixmap(QPixmap::fromImage(Mat2QImage(mat[4])));
		ui.label_ImageDst_5->setPixmap(QPixmap::fromImage(Mat2QImage(mat[5])));
		ui.label_ImageDst_6->setPixmap(QPixmap::fromImage(Mat2QImage(mat[6])));
		ui.label_ImageDst_7->setPixmap(QPixmap::fromImage(Mat2QImage(mat[7])));
		ui.label_ImageDst_8->setPixmap(QPixmap::fromImage(Mat2QImage(mat[8])));
		ui.label_ImageDst_9->setPixmap(QPixmap::fromImage(Mat2QImage(mat[9])));
		ui.label_ImageDst_10->setPixmap(QPixmap::fromImage(Mat2QImage(mat[10])));
		ui.label_ImageDst_11->setPixmap(QPixmap::fromImage(Mat2QImage(mat[11])));
		ui.label_ImageDst_12->setPixmap(QPixmap::fromImage(Mat2QImage(mat[12])));
		ui.label_ImageDst_13->setPixmap(QPixmap::fromImage(Mat2QImage(mat[13])));
		ui.label_ImageDst_14->setPixmap(QPixmap::fromImage(Mat2QImage(mat[14])));
	}
}
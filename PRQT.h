#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PRQT.h"
#include <QMessageBox>
#include <qlabel.h>
#include <QScrollArea>
#include "api.hpp"

class PRQT : public QMainWindow
{
	Q_OBJECT

public:
	PRQT(QWidget *parent = Q_NULLPTR);

	void display(size_t n, const vector<Mat> &mat);

private:
	Ui::PRQTClass ui;

public slots:
	void slot_LoadImage();

	void slot_plate_locate();
	void slot_plate_detect();
	void slot_plate_judge();
	void slot_plate_recognize();

	void slot_chars_segment();
	void slot_chars_identify();
	void slot_chars_recognise();
};

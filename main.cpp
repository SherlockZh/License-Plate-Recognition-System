#include "PRQT.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PRQT w;
	w.show();
	return a.exec();
}

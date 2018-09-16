#include <QtCore/QCoreApplication>
#include "requester_controller.h"

int main(int argc, char *argv[])
{
	QThread::currentThread()->setObjectName("Main");

	QCoreApplication a(argc, argv);
	CRequesterController controller;
	controller.Test1();
	controller.Test2();
	controller.Test3();
	controller.Test4();
	controller.Test5();
	controller.Test6();

	return a.exec();
}

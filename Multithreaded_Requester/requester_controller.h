#pragma once

#include <QObject>
#include <QThread>
#include "requester.h"

class CWorker: public QObject
{
public:
	void RequestInt( const Requester::CResponse<int>& res, const QString& str );
	void RequestNoResp( const QString& str, int num );
	void RequestNoVal( const Requester::CResponse<>& res );
};

class CRequesterController: public QObject
{
	Q_OBJECT
public:
	CRequesterController();
	~CRequesterController();

	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();

private:
	QThread  WorkerThread;
	CWorker* Worker;
};
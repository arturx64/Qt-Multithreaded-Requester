#include "requester_controller.h"
#include <QDebug>

static const QString VALUE_1_FMT = QString( "Value 1: %1." );
static const QString VALUE_2_FMT = QString( "Value 1: %1. Value 2: %2." );
static const QString SEND_1_FMT = QString( "Send 1: %1." );
static const QString SEND_2_FMT = QString( "Send 1: %1. Send 2: %2." );
static const QString JOIN_FMT = QString( "%1 %2" );

void Log( const QString& fnc, QString additional_data = QString() )
{
	printf( "Fnc: %-25s | Thread: %-5s | %s\n"
		, fnc.toStdString().c_str()
		, QThread::currentThread()->objectName().toStdString().c_str()
	    , additional_data.toStdString().c_str() );
}

void CWorker::RequestInt( const Requester::CResponse<int>& res, const QString& str )
{
	int send_val = 33;
	Log( "Request Int", JOIN_FMT.arg( VALUE_1_FMT.arg( str ) ).arg( SEND_1_FMT.arg( send_val ) ) );
	res( send_val );
}

void CWorker::RequestNoResp( const QString& str, int num )
{
	Log( "Request NoResp", VALUE_2_FMT.arg( str ).arg( num ) );
}

void CWorker::RequestNoVal( const Requester::CResponse<>& res )
{
	Log( "Request NoVal" );
	res();
}

CRequesterController::CRequesterController()
	: Worker( new CWorker() )
{
	Worker->moveToThread( &WorkerThread );
	WorkerThread.setObjectName( "Work" );
	connect( &WorkerThread, &QThread::finished, Worker, &QObject::deleteLater );
	WorkerThread.start();
}
CRequesterController::~CRequesterController()
{
	WorkerThread.quit();
	WorkerThread.wait();
}

void CRequesterController::Test1()
{
	Requester::CRequester::
		Request<Requester::CResponse<int>>( &CWorker::RequestInt, Worker, "Hello" ).
		Response( this, [] ( int val )
	{
		Log( "Response Int", VALUE_1_FMT.arg( val ) );
	} );
}

void CRequesterController::Test2()
{
	Requester::CRequester::
		Request<Requester::CResponse<float, float>>( Worker, [] ( const Requester::CResponse<float, float>& res )
	{
		float f1 = 3.14f;
		float f2 = 32.64f;
		Log( "Request Lambda Float2x", JOIN_FMT.arg( QString() ).arg( SEND_2_FMT.arg( f1 ).arg( f2 ) ) );
		res( f1, f2 );
	} ).
		Response( this, [] ( float f1, float f2 )
	{
		Log( "Response Lambda Float2x", VALUE_2_FMT.arg( f1 ).arg( f2 ) );
	} );
}

void CRequesterController::Test3()
{
	int g = 25;
	Requester::CRequester::Request( &CWorker::RequestNoResp, Worker, "Nothing", g );
}

void CRequesterController::Test4()
{
	Requester::CRequester::Request( Worker, [] ()
	{
		Log( "Request Lambda NoResp" );
	} );
}

void CRequesterController::Test5()
{
	Requester::CRequester::
		Request<Requester::CResponse<>>( &CWorker::RequestNoVal, Worker ).
		Response( this, [] ()
	{
		Log( "Response NoVal" );
	} );
}

void CRequesterController::Test6()
{
	Requester::CRequester::
		Request<Requester::CResponse<>>( Worker, [] ( const Requester::CResponse<>& res )
	{
		Log( "Request Lambda NoVal" );
		res();
	} ).
		Response( this, [] ()
	{
		Log( "Response Lambda NoVal");
	} );
}
#pragma once

#include <QObject>
#include <QTimer>
#include <functional>

namespace Requester
{
	template<typename... TArgs >
	class CResponse
	{
	public:
		typedef std::function<void( TArgs... )> TOperationType;

		CResponse()
			: ResponseExecutor( nullptr )
		{
		}

		CResponse( const TOperationType& operation, QObject* response_executor )
			: Operation( operation )
			, ResponseExecutor( response_executor )
		{
		}

		template< typename... TNewArgs >
		void operator()( TNewArgs&&... args ) const
		{
			if ( IsValid() )
			{
				auto operation = std::bind( Operation, std::forward<TNewArgs>( args )... );
				QTimer::singleShot( 0, ResponseExecutor, [res_operation{ std::move( operation ) }]()
				{
					res_operation();
				} );
			}
		}
		operator bool() const
		{
			return IsValid();
		}
	private:
		bool IsValid() const
		{
			return Operation && ResponseExecutor;
		}
		TOperationType Operation;
		QObject* ResponseExecutor;
	};

	class CRequester
	{
		template< typename TResp >
		class CProxyRequester
		{
		public:
			CProxyRequester( QObject* request_executor, const std::function<void( TResp )>& request_operation )
				: RequestExecutor( request_executor )
				, RequestObj( request_operation )
			{
			}

			void Response( QObject* response_executor, typename const TResp::TOperationType& resp_operation )
			{
				ResponseObj = TResp( resp_operation, response_executor );
				Start();
			}
		private:
			void Start()
			{
				if ( RequestObj )
				{
					QTimer::singleShot( 0, RequestExecutor, [req_operation{ std::move( RequestObj ) }, res_operation{ std::move( ResponseObj ) }]()
					{
						req_operation( res_operation );
					} );
				}
			}
			QObject* RequestExecutor;
			TResp ResponseObj;
			std::function<void( TResp )> RequestObj;
		};

	public:
		template< typename TResp, typename TReqClass, typename TReqFnc, typename... TArgs,
			class = typename std::enable_if<std::is_base_of<QObject, TReqClass>::value>::type >
			static CProxyRequester<TResp> Request( TReqFnc fnc, TReqClass* request_executor, TArgs&&... args )
		{
			std::function<void( TResp )> request_operation = std::bind( fnc, request_executor, std::placeholders::_1, std::forward<TArgs>( args )... );
			CProxyRequester<TResp> request_proxy( request_executor, request_operation );
			return request_proxy;
		}

		template< typename TReqClass, typename TReqFnc, typename... TArgs,
			class = typename std::enable_if<std::is_base_of<QObject, TReqClass>::value>::type >
			static void Request( TReqFnc fnc, TReqClass* request_executor, TArgs&&... args )
		{
			std::function<void()> request_operation = std::bind( fnc, request_executor, std::forward<TArgs>( args )... );
			QTimer::singleShot( 0, request_executor, [req_operation{ std::move( request_operation ) }]()
			{
				req_operation();
			} );
		}

		template< typename TResp, typename TReqFnc >
		static CProxyRequester<TResp> Request( QObject* request_executor, const TReqFnc& fnc )
		{
			std::function<void( TResp )> request_operation = std::bind( fnc, std::placeholders::_1 );
			CProxyRequester<TResp> request_proxy( request_executor, request_operation );
			return request_proxy;
		}

		template< typename TReqFnc >
		static void Request( QObject* request_executor, const TReqFnc& fnc )
		{
			std::function<void()> request_operation = std::bind( fnc );
			QTimer::singleShot( 0, request_executor, [req_operation{ std::move( request_operation ) }]()
			{
				req_operation();
			} );
		}
	};
}

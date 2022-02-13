#ifndef _SULOKI_UDP_H_
#define _SULOKI_UDP_H_

#include "suloki.h"
#include "packer.h"

#include "boost/asio.hpp"
#include "boost/lockfree/queue.hpp"
#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

namespace net = boost::asio;
using boost::asio::ip::udp;

namespace SulokiNet
{
	class UdpServer : public Suloki::Base, public Suloki::StateMachineInterface
	{
	public:
		UdpServer() {
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
			m_numberRecved = 0;
#endif
		}
		virtual ~UdpServer() {}
	public:
		virtual Suloki::Ret Init(void);
		virtual Suloki::Ret Start(void);
		virtual Suloki::Ret Run(void);
		virtual Suloki::Ret Stop(void);
		virtual Suloki::Ret Clear(void);
		//net interface
		virtual Suloki::Ret WriteAsync(const char* pData, int dataLen);
		virtual Suloki::Ret HandleReaded(const char* pData, int dataLen);
		//
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
		Suloki::Int GetNumRecved(void)
		{
			return m_numberRecved;
		}
#endif
	private:
		void do_write(void);
		void handle_receive_from(const boost::system::error_code& error,
			size_t bytes_recvd);
		void handle_send_to(const boost::system::error_code& /*error*/,
			size_t /*bytes_sent*/);
	protected:
		void RunFunction(void);
	protected:
		bool m_bRunned;
		std::auto_ptr<boost::thread> m_threadRunnedSmart;
		//
		boost::asio::io_context io_context;
		std::auto_ptr<udp::socket> socketSmart;
		udp::endpoint sender_endpoint_;
		enum { max_length = BLOCK_SIZE+sizeof(MsgHead)};
		char data_[max_length];
		//
		volatile Suloki::Int m_flagWrited;
		Suloki::Queue<Suloki::Data> m_queueSend;
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
		volatile Suloki::Int m_numberRecved;
#endif
	};
	class UdpClient : public UdpServer
	{
	public:
		UdpClient() {}
		virtual ~UdpClient() {}
		//StateMachine interface
		virtual Suloki::Ret Init(void);
		virtual Suloki::Ret Start(void);
		virtual Suloki::Ret Run(void);
		virtual Suloki::Ret Stop(void);
		virtual Suloki::Ret Clear(void);
		//net interface
		virtual Suloki::Ret WriteAsync(const char* pData, int dataLen);
		virtual Suloki::Ret HandleReaded(const char* pData, int dataLen);
	private:
		void do_write(void);
		void handle_receive_from(const boost::system::error_code& error,
			size_t bytes_recvd);
		void handle_send_to(const boost::system::error_code& /*error*/,
			size_t /*bytes_sent*/);
	protected:
		void RunFunction(void);
	protected:
		udp::endpoint endpointSended;
	};
}

#endif

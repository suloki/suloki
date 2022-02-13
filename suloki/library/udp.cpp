#include "udp.h"

namespace SulokiNet
{
	Suloki::Ret UdpServer::Init(void)
	{
		m_bRunned = true;
		m_flagWrited = 0;
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
		m_numberRecved = 0;
#endif
		//
		std::string serverIp;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_UDPSERVERIP_KEY, serverIp);
		std::string serverPort;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_UDPSERVERPORT_KEY, serverPort);
		if (serverPort.compare("0") == 0)
		{
			return Suloki::SUCCESS;
		}
		try {
			socketSmart = std::auto_ptr<udp::socket>(new udp::socket(io_context, udp::endpoint(udp::v4(), atoi(serverPort.c_str()))));
			if (socketSmart.get() == NULL)
			{
				return Suloki::FAIL;
			}
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpServer::Start(void)
	{
		try {
			m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&UdpServer::RunFunction, this)));
			if (m_threadRunnedSmart.get() == NULL)
			{
				return Suloki::FAIL;
			}
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpServer::Run(void)
	{
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpServer::Stop(void)
	{
		m_bRunned = false;
		//
		try {
			if (socketSmart.get() != NULL)
				socketSmart->close();
			io_context.stop();
			if (m_threadRunnedSmart.get() != NULL)
				m_threadRunnedSmart->join();
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpServer::Clear(void)
	{
		try {
			m_threadRunnedSmart.reset();
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpServer::WriteAsync(const char* pData, int dataLen)
	{
		if (!(pData != NULL && dataLen > 0 && dataLen < SULOKI_MAXLEN_SENDED_NET_SULOKIDEF))
			return Suloki::INVALIDPARA_ERRCODE;
		if (socketSmart.get() == NULL)
			return Suloki::INVALIDSTATE_ERRCODE;
		Suloki::SmartPtr< Suloki::Data > msgSmart = Suloki::DataStaticCreater::Create((Suloki::Int8*)pData, dataLen);
		if (msgSmart.get() == NULL)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
			return Suloki::NOMEMORY_ERRCODE;
		}
		m_queueSend.Push(msgSmart);
		//io_context.post(boost::bind(&UdpServer::do_write, this));
		if (SULOKI_INCREMENT_SULOKIDEF(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKIDEF(&m_flagWrited);
			return Suloki::SUCCESS;
		}
		do_write();
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpServer::HandleReaded(const char* pData, int dataLen)
	{
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
		m_numberRecved++;
#endif
		//std::cout << "server recv:" << std::string(data_, dataLen) << std::endl;
		socketSmart->async_send_to(
			boost::asio::buffer(pData, dataLen), sender_endpoint_,
			boost::bind(&UdpServer::handle_send_to, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		return Suloki::SUCCESS;
	}
	void UdpServer::do_write(void) {
		Suloki::SmartPtr< Suloki::Data > msgSmart = m_queueSend.Pop();
		if (msgSmart.IsNULL())
		{
			SULOKI_DECREMENT_SULOKIDEF(&m_flagWrited);
			return;
		}
		//
		Suloki::Int msgLen = 0;
		const Suloki::Int8* pMsg = msgSmart->Get(&msgLen);
		//
		socketSmart->async_send_to(
			boost::asio::buffer(pMsg, msgLen), sender_endpoint_,
			boost::bind(&UdpServer::handle_send_to, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	void UdpServer::handle_receive_from(const boost::system::error_code& error,
		size_t bytes_recvd)
	{
		if (!error && bytes_recvd > 0)
		{
			HandleReaded(data_, bytes_recvd);
		}
		socketSmart->async_receive_from(
			boost::asio::buffer(data_, max_length), sender_endpoint_,
			boost::bind(&UdpServer::handle_receive_from, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	void UdpServer::handle_send_to(const boost::system::error_code& /*error*/,
		size_t /*bytes_sent*/)
	{
		//if (m_queueSend.GetSize() > 0)
		//	do_write();
		//	//io_context.post(boost::bind(&UdpServer::do_write, this));
		SULOKI_DECREMENT_SULOKIDEF(&m_flagWrited);
		//
		if (SULOKI_INCREMENT_SULOKIDEF(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKIDEF(&m_flagWrited);
			return;
		}
		do_write();
	}
	void UdpServer::RunFunction(void)
	{
		if (socketSmart.get() == NULL)
			return;
		while (Suloki::GlobalStateMachineSingleton::Instance().GetState() < Suloki::STOP_STATEMACHINE && m_bRunned)
		{
			try
			{
				socketSmart->async_receive_from(
					boost::asio::buffer(data_, max_length), sender_endpoint_,
					boost::bind(&UdpServer::handle_receive_from, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				//
				io_context.run();
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << "\n";
			}
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "UdpServer::RunFunction will restart by exception", Suloki::LOG_ERROR_LEVEL);
			Suloki::Time::Sleep(256);
		}
	}
	//
	Suloki::Ret UdpClient::Init(void)
	{
		m_bRunned = true;
		m_flagWrited = 0;
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
		m_numberRecved = 0;
#endif
		//
		std::string serverIpRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_REMOTEUDPSERVERIP_KEY, serverIpRemoted);
		std::string serverPortRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_REMOTEUDPSERVERPORT_KEY, serverPortRemoted);
		if (serverPortRemoted.compare("0") == 0)
			return Suloki::SUCCESS;
		//
		try {
			socketSmart = std::auto_ptr<udp::socket>(new udp::socket(io_context, udp::endpoint(udp::v4(), 0)));
			if (socketSmart.get() == NULL)
				return Suloki::FAIL;
			udp::resolver resolver(io_context);
			udp::resolver::results_type endpoints = resolver.resolve(udp::v4(), serverIpRemoted, serverPortRemoted);
			endpointSended = *endpoints.begin();
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpClient::Start(void)
	{
		try {
			m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&UdpClient::RunFunction, this)));
			if (m_threadRunnedSmart.get() == NULL)
			{
				return Suloki::FAIL;
			}
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpClient::Run(void)
	{
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpClient::Stop(void)
	{
		m_bRunned = false;
		//
		try {
			if (socketSmart.get() != NULL)
				socketSmart->close();
			io_context.stop();
			if (m_threadRunnedSmart.get() != NULL)
				m_threadRunnedSmart->join();
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpClient::Clear(void)
	{
		try {
			m_threadRunnedSmart.reset();
		}
		catch (...) {
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpClient::WriteAsync(const char* pData, int dataLen)
	{
		if (!(pData != NULL && dataLen > 0 && dataLen < SULOKI_MAXLEN_SENDED_NET_SULOKIDEF))
			return Suloki::INVALIDPARA_ERRCODE;
		if (socketSmart.get() == NULL)
			return Suloki::INVALIDSTATE_ERRCODE;
		Suloki::SmartPtr< Suloki::Data > msgSmart = Suloki::DataStaticCreater::Create((Suloki::Int8*)pData, dataLen);
		if (msgSmart.get() == NULL)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
			return Suloki::NOMEMORY_ERRCODE;
		}
		m_queueSend.Push(msgSmart);
		//io_context.post(boost::bind(&UdpClient::do_write, this));
		if (SULOKI_INCREMENT_SULOKIDEF(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKIDEF(&m_flagWrited);
			return Suloki::SUCCESS;
		}
		do_write();
		return Suloki::SUCCESS;
	}
	Suloki::Ret UdpClient::HandleReaded(const char* pData, int dataLen)
	{
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
		m_numberRecved++;
#endif
		//std::cout << "client recv:" << std::string(data_, dataLen) << std::endl;
		return Suloki::SUCCESS;
	}
	void UdpClient::do_write(void) {
		Suloki::SmartPtr< Suloki::Data > msgSmart = m_queueSend.Pop();
		if (msgSmart.IsNULL())
			return;
		//
		Suloki::Int msgLen = 0;
		const Suloki::Int8* pMsg = msgSmart->Get(&msgLen);
		//
		socketSmart->async_send_to(
			boost::asio::buffer(pMsg, msgLen), endpointSended,
			boost::bind(&UdpClient::handle_send_to, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	void UdpClient::handle_receive_from(const boost::system::error_code& error,
		size_t bytes_recvd)
	{
		if (!error && bytes_recvd > 0)
		{
			HandleReaded(data_, bytes_recvd);
		}
		socketSmart->async_receive_from(
			boost::asio::buffer(data_, max_length), sender_endpoint_,
			boost::bind(&UdpClient::handle_receive_from, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	void UdpClient::handle_send_to(const boost::system::error_code& /*error*/,
		size_t /*bytes_sent*/)
	{
		//if (m_queueSend.GetSize() > 0)
		//	do_write();
		//	//io_context.post(boost::bind(&UdpClient::do_write, this));
		SULOKI_DECREMENT_SULOKIDEF(&m_flagWrited);
		//
		if (SULOKI_INCREMENT_SULOKIDEF(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKIDEF(&m_flagWrited);
			return;
		}
		do_write();

	}
	void UdpClient::RunFunction(void)
	{
		if (socketSmart.get() == NULL)
			return;
		while (Suloki::GlobalStateMachineSingleton::Instance().GetState() < Suloki::STOP_STATEMACHINE && m_bRunned)
		{
			try
			{
				socketSmart->async_receive_from(
					boost::asio::buffer(data_, max_length), sender_endpoint_,
					boost::bind(&UdpClient::handle_receive_from, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				//
				io_context.run();
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << "\n";
			}
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "UdpClient::RunClientFunction will restart", Suloki::LOG_ERROR_LEVEL);
			Suloki::Time::Sleep(256);
		}
	}
}
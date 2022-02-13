#ifndef _SULOKI_WS_H_
#define _SULOKI_WS_H_

#include "suloki.h"
//#include "udp.h"

#include <stdio.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

//????? this old code,look carefull

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

										//------------------------------------------------------------------------------

										// Report a failure
void fail(beast::error_code ec, char const* what);

class clientsession;
class ClientsessionSwrap : public Suloki::Base
{
public:
	ClientsessionSwrap()
	{}
	virtual ~ClientsessionSwrap()
	{}
	void SetSession(std::shared_ptr<clientsession> sessionSharedptr)
	{
		m_clientsessionSharedptr = sessionSharedptr;
	}
	std::shared_ptr<clientsession> GetSession(void)
	{
		return m_clientsessionSharedptr;
	}
private:
	ClientsessionSwrap(ClientsessionSwrap& ref) {}
	ClientsessionSwrap& operator=(ClientsessionSwrap& ref) { return *this; }
private:
	std::shared_ptr<clientsession> m_clientsessionSharedptr;
};
class clientsession : public std::enable_shared_from_this<clientsession>
{
	tcp::resolver resolver_;
	websocket::stream<beast::tcp_stream> ws_;
	beast::flat_buffer buffer_;
	std::string host_;
	std::string text_;

public:
	// Resolver and socket require an io_context
	explicit
		clientsession(net::io_context& ioc)
		: resolver_(net::make_strand(ioc))
		, ws_(net::make_strand(ioc)), m_bReady(false), m_flagWrited(0)
	{
	}
	virtual ~clientsession()
	{
		HandleException();
	}
protected:
	virtual void HandleException(void)
	{
		if (m_bReady)
		{
			m_bReady = false;
			Suloki::SmartPtr< Suloki::Base > baseptrSmart;
			Suloki::UrcSingleton::Instance().DelUr(m_urName, baseptrSmart);
		}
	}
	virtual void HandleConnectted(void)
	{
	}
public:
	void Close(void)
	{
		if (!m_bReady)
			return;
		try {
			boost::beast::websocket::close_reason reason;
			reason.reason = "none";
			ws_.close(reason);
		}
		catch (...) {
		}
	}
	Suloki::Ret ReqRes(Suloki::SmartPtr<Suloki::Message> reqSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::Int timeout = 0)
	{
		if (reqSmart.IsNULL())
			return Suloki::INVALIDPARA_ERRCODE;
		std::string strBusinessId = reqSmart->GetField(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF);
		std::string strMessageId = reqSmart->GetField(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF);
		std::string strSequenceId = reqSmart->GetField(SULOKI_SEQUENCEID_MSGFIELD_KEY_SULOKIDEF);
		std::string strGroupFrom = reqSmart->GetField(SULOKI_GROUPFROM_MSGFIELD_KEY_SULOKIDEF);
		std::string strServiceFrom = reqSmart->GetField(SULOKI_SERVICEFROM_MSGFIELD_KEY_SULOKIDEF);
		std::string strGroupTo = reqSmart->GetField(SULOKI_GROUPTO_MSGFIELD_KEY_SULOKIDEF);
		std::string strServiceTo = reqSmart->GetField(SULOKI_SERVICETO_MSGFIELD_KEY_SULOKIDEF);
		if (!(strBusinessId.length() > 0 && strMessageId.length() > 0 && strSequenceId.length() > 0 && strGroupFrom.length() > 0 && strServiceFrom.length() > 0 && strGroupTo.length() > 0 && strServiceTo.length() > 0))
			return Suloki::INVALIDPARA_ERRCODE;
		/*
		std::vector<std::string> wsObjs;
		if (Suloki::UrcSingleton::Instance().GetUr(Suloki::URC_SYSTEM_WSCLIENTCONN_NAMEPREFIX, wsObjs) != Suloki::SUCCESS || wsObjs.size() == 1)
			return Suloki::INVALIDSTATE_ERRCODE;
		Suloki::SmartPtr< Suloki::Base > baseSmart;
		if (Suloki::UrcSingleton::Instance().GetUr(wsObjs[0], baseSmart) != Suloki::SUCCESS)
		{
			std::stringstream strStream;
			strStream << "GetUr error.urname:" << wsObjs[0];
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		Suloki::SmartPtr<ClientsessionSwrap> wsconnSmartPtr = baseSmart.Upcast<ClientsessionSwrap>();
		*/
		//
		std::string strUrName;
		std::stringstream strStream;
		strStream << Suloki::URC_SYSTEM_RESPONSE_NAMEPREFIX << strBusinessId << "_" << strMessageId << "_" << strSequenceId;
		strUrName = strStream.str();
		Suloki::SmartPtr<Suloki::EventManager::ConvarItem> convarSmart = Suloki::EventManagerSingleton::Instance().PopConvar();
		if (convarSmart.IsNULL())
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "Suloki::EventManagerSingleton::Instance().PopConvar() error", Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		if(Suloki::UrcSingleton::Instance().AddUr(strUrName, convarSmart, Suloki::URC_OBJECT_TYPE, 0, false, 0) != Suloki::SUCCESS)
		{
			Suloki::EventManagerSingleton::Instance().PushConvar(convarSmart);
			std::stringstream strStream;
			strStream << "AddUr error.urname:" << strUrName;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		//
		//EventManagerSingleton::Instance().SetTimer(std::string strUrcKey, Int timeout);
		//
		std::string strReq = reqSmart->Get();
		//wsconnSmartPtr->GetSession()->WriteAsync(strReq.c_str(), strReq.length());
		WriteAsync(strReq.c_str(), strReq.length());
		//
		Suloki::Ret ret = Suloki::EventManagerSingleton::Instance().Wait(strUrName, timeout, resSmart);
		//Suloki::EventManagerSingleton::Instance().PushConvar(convarSmart);
		//Suloki::UrcSingleton::Instance().DelUr(strUrName, baseSmart);
		//
		return ret;
	}
	Suloki::Ret WriteAsync(const char* pData, int dataLen)
	{
		if (!(pData != NULL && dataLen > 0 && dataLen < SULOKI_MAXLEN_SENDED_NET_SULOKIDEF))
			return Suloki::INVALIDPARA_ERRCODE;
		if(!m_bReady)
			return Suloki::INVALIDSTATE_ERRCODE;
		Suloki::SmartPtr< Suloki::Data > dataSmart = Suloki::DataStaticCreater::Create((Suloki::Int8*)pData, dataLen);
		if (dataSmart.IsNULL())
			return Suloki::FAIL;
		m_queueSend.Push(dataSmart);
		if (SULOKI_INCREMENT_SULOKI(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return Suloki::SUCCESS;
		}
		dataSmart = m_queueSend.Pop();
		if (dataSmart.IsNULL())
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return Suloki::SUCCESS;
		}
		Suloki::Int bufLen = 0;
		const char* pBuf = (const char*)dataSmart->Get(&bufLen);
		//text_ = std::string(pData, dataLen);
		ws_.async_write(
			net::buffer(pBuf, bufLen),//text_),
			beast::bind_front_handler(
				&clientsession::on_write,
				shared_from_this()));
		return Suloki::SUCCESS;
	}
	virtual void HandleRead(std::string& strMsg)
	{
		//std::cout << strMsg << std::endl;
	}

	// Start the asynchronous operation
	void run(
			char const* host,
			char const* port,
			char const* text)
	{
		// Save these for later
		host_ = host;
		text_ = text;

		// Look up the domain name
		resolver_.async_resolve(
			host,
			port,
			beast::bind_front_handler(
				&clientsession::on_resolve,
				shared_from_this()));
		//return shared_from_this();
	}

	void
		on_resolve(
			beast::error_code ec,
			tcp::resolver::results_type results)
	{
		if (ec)
			return fail(ec, "resolve");

		// Set the timeout for the operation
		beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

		// Make the connection on the IP address we get from a lookup
		beast::get_lowest_layer(ws_).async_connect(
			results,
			beast::bind_front_handler(
				&clientsession::on_connect,
				shared_from_this()));
	}

	void
		on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
	{
		if (ec)
			return fail(ec, "connect");

		// Turn off the timeout on the tcp_stream, because
		// the websocket stream has its own timeout system.
		beast::get_lowest_layer(ws_).expires_never();

		// Set suggested timeout settings for the websocket
		ws_.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::client));

		// Set a decorator to change the User-Agent of the handshake
		ws_.set_option(websocket::stream_base::decorator(
			[](websocket::request_type& req)
		{
			req.set(http::field::user_agent,
				std::string(BOOST_BEAST_VERSION_STRING) +
				" websocket-client-async");
		}));

		// Update the host_ string. This will provide the value of the
		// Host HTTP header during the WebSocket handshake.
		// See https://tools.ietf.org/html/rfc7230#section-5.4
		host_ += ':' + std::to_string(ep.port());

		// Perform the websocket handshake
		ws_.async_handshake(host_, "/",
			beast::bind_front_handler(
				&clientsession::on_handshake,
				shared_from_this()));
	}

	void
		on_handshake(beast::error_code ec)
	{
		if (ec)
			return fail(ec, "handshake");

		m_bReady = true;

		std::stringstream strStream;
		strStream << Suloki::URC_SYSTEM_WSCLIENTCONN_NAMEPREFIX << Suloki::IdManagerSingleton::Instance().GetId();
		std::shared_ptr<clientsession> wsconnSmartPtr1 = shared_from_this();
		//std::shared_ptr<session> wsconnSmartPtr1(stdSmartPtr.get(), [stdSmartPtr](session*)mutable {stdSmartPtr.reset(); });
		wsconnSmartPtr1->SetUrname(strStream.str());
		Suloki::SmartPtr<ClientsessionSwrap> wsconnSmartPtr(new ClientsessionSwrap());
		wsconnSmartPtr->SetSession(wsconnSmartPtr1);
		Suloki::SmartPtr< Suloki::Base > baseptrSmart = wsconnSmartPtr.Downcast<Suloki::Base>();
		if (Suloki::UrcSingleton::Instance().AddUr(strStream.str(), baseptrSmart, Suloki::URC_OBJECT_TYPE, 0, true, 0) != Suloki::SUCCESS)
		{
			std::stringstream strStream;
			strStream << "AddUr error.ur key:" << strStream.str();
			SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		}
		else
		{
			/*
			boost::property_tree::ptree jsonMsg;
			Suloki::Message::MakeJsonMsg(jsonMsg);
			try {
				jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
				jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_WSCONNECTED_MESSAGEID_VAL_SULOKIDEF);
				std::string serverIpRemoted;
				Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIPREMOTED_KEY, serverIpRemoted);
				std::string serverPortRemoted;
				Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORTREMOTED_KEY, serverPortRemoted);
				Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
				std::vector<std::string> context;
				context.push_back(GetUrname());
				context.push_back(serverIpRemoted);
				context.push_back(serverPortRemoted);
				context.push_back(SULOKI_FALSE_VAL_SULOKIDEF);
				Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
				if (!msgSmart.IsNULL() && !contextSmart.IsNULL())
				{
					Suloki::UrcSingleton::Instance().SendMsgToMainService(msgSmart, contextSmart);
				}
			}
			catch (boost::property_tree::ptree_error pt) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
			}
			catch (...) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
			}
			*/
			/*
			std::string groupname;
			Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, groupname);
			std::string servicename;
			Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICENAME_KEY, servicename);
			if (groupname.length() > 0 && servicename.length() > 0)
			{
				boost::property_tree::ptree jsonMsg;
				Suloki::Message::MakeJsonMsg(jsonMsg);
				try {
					jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
					jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF);
					jsonMsg.put<std::string>(SULOKI_GROUPFROM_MSGFIELD_KEY_SULOKIDEF, groupname);
					jsonMsg.put<std::string>(SULOKI_SERVICEFROM_MSGFIELD_KEY_SULOKIDEF, servicename);
					//boost::property_tree::ptree jsonBody;
					//jsonBody.put<std::string>(SULOKI_GROUPFROM_MSGFIELD_KEY_SULOKIDEF, groupname);
					//jsonBody.put<std::string>(SULOKI_SERVICEFROM_MSGFIELD_KEY_SULOKIDEF, servicename);
					//jsonMsg.put_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, jsonBody);
				}
				catch (boost::property_tree::ptree_error pt) {
					SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
				}
				catch (...) {
					SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
				}
				Suloki::SmartPtr<Suloki::Message> reqSmart(new Suloki::Message(jsonMsg));
				Suloki::SmartPtr<Suloki::Message> resSmart;
				if(!reqSmart.IsNULL() && ReqRes(reqSmart, resSmart, 1000) == Suloki::SUCCESS)
					HandleConnectted();
				else
				{
					std::stringstream strStream;
					strStream << "reqres SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF msg error.groupname:" << groupname << ";servicename:" << servicename;
					SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
					Close();
				}
			}
			else
			*/
				HandleConnectted();
			/*
			std::string groupname;
			Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, groupname);
			std::string servicename;
			Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICENAME_KEY, servicename);
			if (groupname.length() > 0 && servicename.length() > 0)
			{
				boost::property_tree::ptree jsonMsg;
				Suloki::Message::MakeJsonMsg(jsonMsg);
				try {
					jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
					jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF);
					boost::property_tree::ptree jsonBody;
					jsonBody.put<std::string>(SULOKI_GROUP_MSGFIELD_KEY_SULOKIDEF, groupname);
					jsonBody.put<std::string>(SULOKI_SERVICE_MSGFIELD_KEY_SULOKIDEF, servicename);
					jsonMsg.put_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, jsonBody);
				}
				catch (boost::property_tree::ptree_error pt) {
					SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
				}
				catch (...) {
					SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
				}
				Suloki::Message msg(jsonMsg);
				std::string strMsg = msg.Get();
				if (strMsg.length() > 0)
					WriteAsync(strMsg.c_str(), strMsg.length());
			}
			*/
		}

		// Send the message
		//
		//ws_.async_write(
		//	net::buffer(text_),
		//	beast::bind_front_handler(
		//		&clientsession::on_write,
		//		shared_from_this()));
				//
		ws_.async_read(
			buffer_,
			beast::bind_front_handler(
				&clientsession::on_read,
				shared_from_this()));
		//m_bReady = true;
	}

	void
		on_write(
			beast::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			HandleException();
			return fail(ec, "write");
		}

		// Read a message into our buffer
		//
		//ws_.async_read(
		//	buffer_,
		//	beast::bind_front_handler(
		//		&clientsession::on_read,
		//		shared_from_this()));
		//
		SULOKI_DECREMENT_SULOKI(&m_flagWrited);
		if (SULOKI_INCREMENT_SULOKI(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return;
		}
		Suloki::SmartPtr< Suloki::Data > dataSmart = m_queueSend.Pop();
		if (dataSmart.IsNULL())
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return;
		}
		Suloki::Int bufLen = 0;
		const char* pBuf = (const char*)dataSmart->Get(&bufLen);
		//text_ = msg;
		ws_.async_write(
			net::buffer(pBuf, bufLen),//text_),
			beast::bind_front_handler(
				&clientsession::on_write,
				shared_from_this()));
	}

	void
		on_read(
			beast::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			HandleException();
			return fail(ec, "read");
		}

		//
		//std::cout << "recv:" << std::string((char*)buffer_.data().data(), buffer_.size()) << std::endl;

		// Close the WebSocket connection
		//ws_.async_close(websocket::close_code::normal,
		//	beast::bind_front_handler(
		//		&clientsession::on_close,
		//		shared_from_this()));
		//
		std::string strTmp = std::string((char*)buffer_.data().data(), buffer_.size());
		HandleRead(strTmp);
		buffer_.consume(buffer_.size());
		ws_.async_read(
			buffer_,
			beast::bind_front_handler(
				&clientsession::on_read,
				shared_from_this()));
	}

	void
		on_close(beast::error_code ec)
	{
		HandleException();
		//
		if (ec)
			return fail(ec, "close");

		// If we get here then the connection is closed gracefully

		// The make_printable() function helps print a ConstBufferSequence
		//std::cout << beast::make_printable(buffer_.data()) << std::endl;
		//std::cout << "clientsession have closed" << std::endl;
	}
	bool IsReady(void)
	{
		return m_bReady;
	}
	std::string GetUrname(void)
	{
		return m_urName;
	}
	void SetUrname(std::string strUrname)
	{
		m_urName = strUrname;
	}
protected:
	std::string m_urName;
	volatile bool m_bReady;
	Suloki::Int m_flagWrited;
	Suloki::Queue<Suloki::Data> m_queueSend;
};

class session;
class SessionSwrap : public Suloki::Service//Base
{
public:
	SessionSwrap()
	{}
	virtual ~SessionSwrap()
	{}
	//
	virtual Suloki::Ret Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
	{
		Suloki::Service::Init(pUrcInterface, groupName, serviceName);
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Start(void) { return Suloki::SUCCESS; }
	virtual Suloki::Ret Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart);
	virtual Suloki::Ret Stop(void) { return Suloki::SUCCESS; }
	virtual Suloki::Ret Clear(void) { return Suloki::SUCCESS; }
	//
	void SetSession(std::shared_ptr<session> sessionSharedptr)
	{
		m_sessionSharedptr = sessionSharedptr;
	}
	std::shared_ptr<session> GetSession(void)
	{
		return m_sessionSharedptr;
	}
private:
	SessionSwrap(SessionSwrap& ref) {}
	SessionSwrap& operator=(SessionSwrap& ref) { return *this; }
private:
	std::shared_ptr<session> m_sessionSharedptr;
};
// Echoes back all received WebSocket messages
class session : public std::enable_shared_from_this<session>
{
	typedef boost::unique_lock<Suloki::RwLockPolicy>   WriteLock;
	typedef boost::shared_lock<Suloki::RwLockPolicy>   ReadLock;
	//
	websocket::stream<beast::tcp_stream> ws_;
	beast::flat_buffer buffer_;

public:
	// Take ownership of the socket
	explicit
		session(tcp::socket&& socket)
		: ws_(std::move(socket)), m_flagWrited(0), m_bReady(false), m_bLogined(false)
	{
		//?????
		//s_threadPoolClosed.Init(2);
	}
	virtual ~session()
	{
		HandleException();
	}

	void SetLogined(bool bLogined)
	{
		m_bLogined = bLogined;
	}
	bool GetLogined(void)
	{
		return m_bLogined;
	}
protected:
	virtual void HandleException(void)
	{
		//ClearGroupService();
		//
		if (m_bReady)
		{
			m_bReady = false;
			Suloki::SmartPtr< Suloki::Base > baseptrSmart;
			Suloki::UrcSingleton::Instance().DelUr(m_urName, baseptrSmart);
		}
	}
	virtual void HandleConnectted(void)
	{
	}
	virtual Suloki::Ret Handle(std::string strReq, std::string& strRes)
	{
		strRes = strReq;
		return Suloki::SUCCESS;
	}
public:
	static void CloseHandler(beast::error_code ec)
	{}
	void Close(void)
	{
		if (!m_bReady)
			return;
		try {
			boost::beast::websocket::close_reason reason;
			reason.reason = "none";
			ws_.close(reason);
		}catch (...) {
		}
	}
	virtual Suloki::Ret WriteAsync(const char* pData, int dataLen)
	{
		if (!(pData != NULL && dataLen > 0 && dataLen < SULOKI_MAXLEN_SENDED_NET_SULOKIDEF))
			return Suloki::INVALIDPARA_ERRCODE;
		if (!m_bReady)
			return Suloki::INVALIDSTATE_ERRCODE;
		Suloki::SmartPtr< Suloki::Data > dataSmart = Suloki::DataStaticCreater::Create((Suloki::Int8*)pData, dataLen);
		if (dataSmart.IsNULL())
			return Suloki::FAIL;
		m_queueSend.Push(dataSmart);
		if (SULOKI_INCREMENT_SULOKI(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return Suloki::SUCCESS;
		}
		dataSmart = m_queueSend.Pop();
		if (dataSmart.IsNULL())
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return Suloki::SUCCESS;
		}
		Suloki::Int bufLen = 0;
		const char* pBuf = (const char*)dataSmart->Get(&bufLen);
		ws_.text(ws_.got_text());
		ws_.async_write(
			net::buffer(pBuf, bufLen),//buffer_.data(),
			beast::bind_front_handler(
				&session::on_write,
				shared_from_this()));
		//std::stringstream strStream;
		//strStream << "send:" << strProtocol;
		//SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_INFO_LEVEL);
		return Suloki::SUCCESS;
	}
	// Get on the correct executor
	void
		run()
	{
		// We need to be executing within a strand to perform async operations
		// on the I/O objects in this session. Although not strictly necessary
		// for single-threaded contexts, this example code is written to be
		// thread-safe by default.
		net::dispatch(ws_.get_executor(),
			beast::bind_front_handler(
				&session::on_run,
				shared_from_this()));
	}

	// Start the asynchronous operation
	void
		on_run()
	{
		// Set suggested timeout settings for the websocket
		ws_.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::server));

		// Set a decorator to change the Server of the handshake
		ws_.set_option(websocket::stream_base::decorator(
			[](websocket::response_type& res)
		{
			res.set(http::field::server,
				std::string(BOOST_BEAST_VERSION_STRING) +
				" websocket-server-async");
		}));
		// Accept the websocket handshake
		ws_.async_accept(
			beast::bind_front_handler(
				&session::on_accept,
				shared_from_this()));
	}

	void
		on_accept(beast::error_code ec)
	{
		if (ec)
			return fail(ec, "accept");

		std::stringstream strStream;
		strStream << Suloki::URC_SYSTEM_WSCONN_NAMEPREFIX << Suloki::IdManagerSingleton::Instance().GetId();
		std::shared_ptr<session> wsconnSmartPtr1 = shared_from_this();
		//std::shared_ptr<session> wsconnSmartPtr1(stdSmartPtr.get(), [stdSmartPtr](session*)mutable {stdSmartPtr.reset(); });
		wsconnSmartPtr1->SetUrname(strStream.str());
		Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr(new SessionSwrap());
		wsconnSmartPtr->SetSession(wsconnSmartPtr1);
		Suloki::SmartPtr< Suloki::Base > baseptrSmart = wsconnSmartPtr.Downcast<Suloki::Base>();
		if (Suloki::UrcSingleton::Instance().AddUr(strStream.str(), baseptrSmart, Suloki::URC_OBJECT_TYPE, 0, true, 0) != Suloki::SUCCESS)
		{
			std::stringstream strStream;
			strStream << "AddUr error.ur key:" << strStream.str();
			SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		}
		else
		{
			/*
			boost::property_tree::ptree jsonMsg;
			Suloki::Message::MakeJsonMsg(jsonMsg);
			try {
				jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
				jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_WSCONNECTED_MESSAGEID_VAL_SULOKIDEF);
				std::string serverIpRemoted;
				Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIPREMOTED_KEY, serverIpRemoted);
				std::string serverPortRemoted;
				Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORTREMOTED_KEY, serverPortRemoted);
				Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
				std::vector<std::string> context;
				context.push_back(GetUrname());
				context.push_back(GetRemoteIp());
				context.push_back(GetRemotePort());
				context.push_back(SULOKI_TRUE_VAL_SULOKIDEF);
				Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
				if (!msgSmart.IsNULL() && !contextSmart.IsNULL())
				{
					if (Suloki::UrcSingleton::Instance().SendMsgToMainService(msgSmart, contextSmart) != Suloki::SUCCESS)
					{
						//????? modify to async function asyncclose
						//Close();
						std::shared_ptr<session> wsconnSmartPtr = shared_from_this();
						s_threadPoolClosed.Post(boost::bind(&session::CloseWs, wsconnSmartPtr));
					}
				}
			}
			catch (boost::property_tree::ptree_error pt) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
			}
			catch (...) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
			}
			*/
			HandleConnectted();
		}
		m_bReady = true;
		// Read a message
		do_read();
	}

	void
		do_read()
	{
		// Read a message into our buffer
		ws_.async_read(
			buffer_,
			beast::bind_front_handler(
				&session::on_read,
				shared_from_this()));
		if (!ws_.got_text())
		{
			ws_.async_close(websocket::close_code::normal,
				beast::bind_front_handler(
					&session::on_close,
					shared_from_this()));
		}
	}
	void on_close(beast::error_code ec)
	{
		HandleException();
		if (ec)
		{
			return fail(ec, "close");
		}
		// If we get here then the connection is closed gracefully

		// The make_printable() function helps print a ConstBufferSequence
		//std::cout << beast::make_printable(buffer_.data()) << std::endl;
		//std::cout << "clientsession have closed" << std::endl;
	}
	void
		on_read(
			beast::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		// This indicates that the session was closed
		if (ec == websocket::error::closed)
		{
			HandleException();
			return;
		}

		if (ec)
		{
			HandleException();
			fail(ec, "read");
			return;
		}

		//std::stringstream strStream;
		//strStream << "recv:" << std::string((char*)buffer_.data().data(), buffer_.size());
		//SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_INFO_LEVEL);
		
		std::string strRes;
		if (Handle(std::string((char*)buffer_.data().data(), buffer_.size()), strRes) == Suloki::SUCCESS)
		{
			/*
			// Echo the message
			//bool bText = ws_.got_text();
			ws_.text(ws_.got_text());
			ws_.async_write(
				net::buffer(strRes),//buffer_.data(),
				beast::bind_front_handler(
					&session::on_write,
					shared_from_this()));
			{SULOKI_INFO_LOG_SULOKI << "send:" << strRes;}
			*/
			WriteAsync(strRes.c_str(), strRes.length());
		}
		buffer_.consume(buffer_.size());
		ws_.async_read(
			buffer_,
			beast::bind_front_handler(
				&session::on_read,
				shared_from_this()));
	}

	void
		on_write(
			beast::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			HandleException();
			return fail(ec, "write");
		}

		SULOKI_DECREMENT_SULOKI(&m_flagWrited);
		// Clear the buffer
		//buffer_.consume(buffer_.size());

		// Do another read
		//do_read();
		if (SULOKI_INCREMENT_SULOKI(&m_flagWrited) >= 2)
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return;
		}
		Suloki::SmartPtr< Suloki::Data > dataSmart = m_queueSend.Pop();
		if (dataSmart.IsNULL())
		{
			SULOKI_DECREMENT_SULOKI(&m_flagWrited);
			return;
		}
		Suloki::Int bufLen = 0;
		const char* pBuf = (const char*)dataSmart->Get(&bufLen);
		ws_.text(ws_.got_text());
		ws_.async_write(
			net::buffer(pBuf, bufLen),//buffer_.data(),
			beast::bind_front_handler(
				&session::on_write,
				shared_from_this()));
		//std::stringstream strStream;
		//strStream << "send:" << strProtocol;
		//SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_INFO_LEVEL);
	}
	std::string GetUrname(void)
	{
		return m_urName;
	}
	void SetUrname(std::string strUrname)
	{
		m_urName = strUrname;
	}
	std::string GetRemoteIp(void)
	{
		return m_ipRemoted;
	}
	void SetRemoteIp(std::string ip)
	{
		m_ipRemoted = ip;
	}
	std::string GetRemotePort(void)
	{
		return m_portRemoted;
	}
	void SetRemotePort(std::string port)
	{
		m_portRemoted = port;
	}
	/*
protected:
	static void HandleMqueue(Suloki::SmartPtr<Suloki::Message> msgOriSmart, Suloki::SmartPtr<Suloki::Context> contestOriSmart, Suloki::SmartPtr<Suloki::Message>& msgSmart, Suloki::SmartPtr<Suloki::Context>& contestSmart)
	{
		std::vector<std::string> contexts = contestOriSmart->Get();
		if (contexts.size() == 0)
			return;
		Suloki::SmartPtr< Suloki::Base > baseptrSmart;
		if (Suloki::UrcSingleton::Instance().GetUr(contexts[0], baseptrSmart) != Suloki::SUCCESS)
			return;
		Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseptrSmart.Upcast<SessionSwrap>();
		std::string strMsg = msgSmart->Get();
		wsconnSmartPtr->GetSession()->WriteAsync(strMsg.c_str(), strMsg.length());
	}
	*/
public:
	/*
	Suloki::Ret SetupGroupService(std::string group, std::string service)
	{
		if (!(group.length() > 0 && service.length() > 0))
			return Suloki::FAIL;
		{
			WriteLock lock(m_rwLock);
			if (m_group.length() == 0)
				return Suloki::FAIL;
			m_group = group;
			m_service = service;
		}
		//add and reg servide queue
		std::stringstream strStreamQueueKey;
		strStreamQueueKey << Suloki::URC_URC_MQUEUE_NAMEPREFIX << group << "_" << service;
		Suloki::Queue<Suloki::Message> queue;
		if (Suloki::UrcSingleton::Instance().AddUr(strStreamQueueKey.str(), queue, Suloki::URC_MSGQUEUE_TYPE, Suloki::URC_ZERO_ATTR | Suloki::URC_EXCLUSIVE_ATTR, false, 0) != Suloki::SUCCESS)
			return Suloki::FAIL;
		{
			const std::string strUcKeySub = group + "_" + service;
			Suloki::AsyncFunc func(HandleMqueue);
			Suloki::SmartPtr<Suloki::Message> msgOriSmart(new Suloki::Message(""));
			std::vector<std::string> context;
			context.push_back(m_urName);
			Suloki::SmartPtr<Suloki::Context> contextOriSmart(new Suloki::Context(context));
			if (Suloki::UrcSingleton::Instance().SubscribeUr(strStreamQueueKey.str(), strUcKeySub, func, msgOriSmart, contextOriSmart, 1) != Suloki::SUCCESS)
			{
				//{SULOKI_ERROR_LOG_SULOKI << "SubscribeUr error.urname:" << strStreamQueueKey.str(); }
				Suloki::Queue<Suloki::Message> queue;
				Suloki::UrcSingleton::Instance().DelUr(strStreamQueueKey.str(), queue);
				return Suloki::FAIL;
			}
		}
		//add and reg group queue
		//
		return Suloki::SUCCESS;
	}
	void GetGroupService(std::string& group, std::string& service)
	{
		ReadLock lock(m_rwLock);
		group = m_group;
		service = m_service;
	}
	void ClearGroupService(void)
	{
		std::string group;
		std::string service;
		GetGroupService(group, service);
		if (group.length() == 0)
			return;
		//unreg and del(if no reg) group queue
		//????? what time to remove group queue
		//unreg and del servide queue
		std::stringstream strStreamQueueKey;
		strStreamQueueKey << Suloki::URC_URC_MQUEUE_NAMEPREFIX << group << "_" << service;
		{
			const std::string strUcKeySub = group + "_" + service;
			Suloki::AsyncFunc func;
			Suloki::SmartPtr<Suloki::Message> msgOriSmart;
			Suloki::SmartPtr<Suloki::Context> contextOriSmart;
			Suloki::UrcSingleton::Instance().UnsubscribeUr(strStreamQueueKey.str(), strUcKeySub, func, msgOriSmart, contextOriSmart, 1);
		}
		Suloki::Queue<Suloki::Message> queue;
		Suloki::UrcSingleton::Instance().DelUr(strStreamQueueKey.str(), queue);
		//
		WriteLock lock(m_rwLock);
		m_group.clear();
		m_service.clear();
	}
	*/
protected:
	//static void CloseWs(std::shared_ptr<session> wsconnSmartPtr)
	//{
	//	wsconnSmartPtr->Close();
	//}
protected:
	//static Suloki::ThreadPool s_threadPoolClosed;
	//
	std::string m_urName;
	volatile bool m_bReady;
	Suloki::Int m_flagWrited;
	Suloki::Queue<Suloki::Data> m_queueSend;
	std::string m_ipRemoted;
	std::string m_portRemoted;
	//
	volatile bool m_bLogined;
	/*
	//for SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF msg
	Suloki::RwLockPolicy m_rwLock;
	std::string m_group;
	std::string m_service;
	*/
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
template <typename T>
class listener : public std::enable_shared_from_this< listener<T> >
{
	net::io_context& ioc_;
	tcp::acceptor acceptor_;

public:
	listener(
		net::io_context& ioc,
		tcp::endpoint endpoint)
		: ioc_(ioc)
		, acceptor_(ioc)
	{
		beast::error_code ec;

		// Open the acceptor
		acceptor_.open(endpoint.protocol(), ec);
		if (ec)
		{
			fail(ec, "open");
			return;
		}

		// Allow address reuse
		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
		if (ec)
		{
			fail(ec, "set_option");
			return;
		}

		// Bind to the server address
		acceptor_.bind(endpoint, ec);
		if (ec)
		{
			fail(ec, "bind");
			return;
		}

		// Start listening for connections
		acceptor_.listen(
			net::socket_base::max_listen_connections, ec);
		if (ec)
		{
			fail(ec, "listen");
			return;
		}
	}

	// Start accepting incoming connections
	void
		run()
	{
		do_accept();
	}

private:
	void
		do_accept()
	{
		// The new connection gets its own strand
		acceptor_.async_accept(
			net::make_strand(ioc_),
			beast::bind_front_handler(
				&listener::on_accept,
				shared_from_this()));
	}

	void
		on_accept(beast::error_code ec, tcp::socket socket)
	{
		if (ec)
		{
			fail(ec, "accept");
		}
		else
		{
			std::string ip = socket.remote_endpoint().address().to_string();
			unsigned short port = socket.remote_endpoint().port();
			// Create the session and run it
			//std::make_shared<session>(std::move(socket))->run();
			//std::make_shared<T>(std::move(socket))->run();
			std::shared_ptr<T> sessionSmart = std::make_shared<T>(std::move(socket));
			sessionSmart->run();
			sessionSmart->SetRemoteIp(ip);
			std::stringstream strStream;
			strStream << port;
			sessionSmart->SetRemotePort(strStream.str());
		}

		// Accept another connection
		do_accept();
	}
};

template <typename T>
class WsServer : public Suloki::Base, public Suloki::StateMachine
{
public:
	WsServer(){}
	virtual ~WsServer() {}
	//StateMachine interface
	virtual Suloki::Ret Init(void)
	{
		try {
			;
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsServer::Init exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Start(void)
	{
		try {
			m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&WsServer::RunFunction, this)));
			if (m_threadRunnedSmart.get() == NULL)
			{
				//{SULOKI_ERROR_LOG_SULOKI << "no memory"; }
				return Suloki::FAIL;
			}
			//
			//m_contextSmart = std::auto_ptr<net::io_context>(new net::io_context());
			//if (m_contextSmart.get() == NULL)
			//{
			//	SULOKI_ERROR_LOG_SULOKI << "no memory";
			//	return Suloki::FAIL;
			//}
			//m_threadClientRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&WsServer::RunClientFunction, this)));
			//if (m_threadClientRunnedSmart.get() == NULL)
			//{
			//	SULOKI_ERROR_LOG_SULOKI << "no memory";
			//	return Suloki::FAIL;
			//}
			//
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsServer::Start exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Run(void)
	{
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Stop(void)
	{
		try {
			std::vector<std::string> nameVector;
			if (Suloki::UrcSingleton::Instance().GetUr(Suloki::URC_SYSTEM_WSCONN_NAMEPREFIX, nameVector) == Suloki::SUCCESS)
			{
				for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
				{
					Suloki::SmartPtr<Suloki::Base> baseSmartPtr;
					Suloki::UrcSingleton::Instance().DelUr(*iter, baseSmartPtr);
					if (baseSmartPtr.IsNULL())
						continue;
					Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmartPtr.Upcast<SessionSwrap>();
					wsconnSmartPtr->GetSession()->Close();
					//boost::shared_ptr< session > connSmartPtr = boost::dynamic_pointer_cast<session>(baseSmartPtr);
					//connSmartPtr->Close();
				}
			}
			//if (m_threadRunnedSmart.get() != NULL)
			//	m_threadRunnedSmart->join();
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsServer::Stop exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Clear(void)
	{
		try {
			m_threadRunnedSmart.reset(NULL);
			//m_threadClientRunnedSmart.reset(NULL);
			//m_contextSmart.reset(NULL);
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsServer::Clear exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	//
	//std::shared_ptr<Tclient> CreateClientSession(void)
	//{
	//	if (m_piocontext != NULL)
	//	{
	//		std::shared_ptr<Tclient> clientSmartPtr = std::make_shared<Tclient>(*m_piocontext);
	//		clientSmartPtr->run("127.0.0.1", "8081", "wsasd");
	//		return clientSmartPtr;
	//	}
	//	return std::shared_ptr<Tclient>(NULL);
	//}
	/*
	Suloki::Ret WriteAsync(std::string strUrname, const char* pData, int dataLen)
	{
		boost::shared_ptr<Suloki::BaseRoot> baseSmartPtr;
		Suloki::UrcSingleton::Instance().GetUr(strUrname, baseSmartPtr);
		if (baseSmartPtr.get() == NULL)
			return Suloki::NOMEMORY_ERRORCODE;
		boost::shared_ptr< session > connSmartPtr = boost::dynamic_pointer_cast<session>(baseSmartPtr);
		connSmartPtr->WriteAsync(pData, dataLen);
		return Suloki::SUCCESS;
	}
	Suloki::Ret WriteAsyncToAllSession(const char* pData, int dataLen)
	{
		std::vector<std::string> nameVector;
		if (Suloki::UrcSingleton::Instance().GetUrDir(Suloki::URC_SYSTEM_WSCONN_NAMEPREFIX, nameVector) == Suloki::SUCCESS)
		{
			for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
			{
				boost::shared_ptr<Suloki::BaseRoot> baseSmartPtr;
				Suloki::UrcSingleton::Instance().GetUr(*iter, baseSmartPtr);
				if (baseSmartPtr.get() == NULL)
					continue;
				boost::shared_ptr< session > connSmartPtr = boost::dynamic_pointer_cast<session>(baseSmartPtr);
				connSmartPtr->WriteAsync(pData, dataLen);
			}
		}
		return Suloki::SUCCESS;
	}
	*/
#ifdef SULOKI_DIPPORT_DEF
	void ReconnectPtop(void)
	{
		Stop();
		Clear();
		if (Init() != Suloki::SUCCESS)
		{
			{SULOKI_ERROR_LOG_SULOKI << "Init error"; }
			return;
		}
		if (Start() != Suloki::SUCCESS)
		{
			{SULOKI_ERROR_LOG_SULOKI << "Start error"; }
			return;
		}
	}
#endif
protected:
	void RunFunction(void)
	{
		std::string serverIp;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIP_KEY, serverIp);
		std::string serverPort;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORT_KEY, serverPort);
		if (serverPort.compare("0") == 0)
			return;
		//
		while (Suloki::GlobalStateMachineSingleton::Instance().GetState() < Suloki::StateMachine::STOPPED)
		{
			try {
				auto const address = net::ip::make_address(serverIp);
				auto const port = static_cast<unsigned short>(std::atoi(serverPort.c_str()));
				auto const threads = std::max<int>(1, std::atoi("3"));

				// The io_context is required for all I/O
				net::io_context ioc{ threads };

				// Create and launch a listening port
				std::make_shared< listener<T> >(ioc, tcp::endpoint{ address, port })->run();
				//m_piocontext = &ioc;

				std::stringstream strStream;
				strStream << "ws server have accepted.ip:" << serverIp << ";serverPort:" << serverPort;
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_INFO_LEVEL);

				// Run the I/O service on the requested number of threads
				std::vector<std::thread> v;
				v.reserve(threads - 1);
				for (auto i = threads - 1; i > 0; --i)
					v.emplace_back(
						[&ioc]
				{
					ioc.run();
				});
				ioc.run();
			}
			catch (...) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "WsServer::RunFunction exception", Suloki::LOG_ERROR_LEVEL);
			}
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "WsServer::RunFunction will restart by exception", Suloki::LOG_ERROR_LEVEL);
			Suloki::Time::Sleep(256);
		}
		return;
	}
	//
	//void RunClientFunction(void)
	//{
	//	auto const host = "127.0.0.1";
	//	auto const port = "8081";
	//	auto const text = "wsasd";
//
	//	// The io_context is required for all I/O
//
	//	// Launch the asynchronous operation
	//	std::make_shared<clientsession>(*m_contextSmart)->run(host, port, text);
	//
	//	// Run the I/O service. The call will return when
	///	// the socket is closed.
	//	m_contextSmart->run();
	//	m_contextSmart.reset(NULL);
	//}
	//
private:
	WsServer(WsServer& ref) {}
	WsServer& operator=(WsServer& ref) { return *this; }
protected:
	std::auto_ptr<boost::thread> m_threadRunnedSmart;
	//std::auto_ptr<boost::thread> m_threadClientRunnedSmart;
	//std::auto_ptr<net::io_context> m_contextSmart;
};
//typedef Suloki::Singleton<WsServer> WsServerSingleton;
template <typename T>
class WsClient : public Suloki::Base, public Suloki::StateMachine
{
	typedef Suloki::RwLockPolicy LockPolicy;
	typedef boost::unique_lock<LockPolicy>   WriteLock;
	typedef boost::shared_lock<LockPolicy>   ReadLock;
public:
	WsClient():m_bInited(false){}
	virtual ~WsClient() {}
	//StateMachine interface
	virtual Suloki::Ret Init(void)
	{
		try {
			;
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsClient::Init exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Start(void)
	{
		try {
			m_contextSmart = std::auto_ptr<net::io_context>(new net::io_context());
			if (m_contextSmart.get() == NULL)
			{
				//{SULOKI_ERROR_LOG_SULOKI << "no memory"; }
				return Suloki::FAIL;
			}
			m_threadClientRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&WsClient::RunClientFunction, this)));
			if (m_threadClientRunnedSmart.get() == NULL)
			{
				//{SULOKI_ERROR_LOG_SULOKI << "no memory"; }
				return Suloki::FAIL;
			}
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsClient::Start exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Run(void)
	{
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Stop(void)
	{
		{
			WriteLock lock(m_rwLock);
			if(m_clientSmartPtr.get() != NULL)
				m_clientSmartPtr->Close();
			m_clientSmartPtr.reset();
		}
		m_contextSmart->stop();
		try {
			if (m_threadClientRunnedSmart.get() != NULL)
				m_threadClientRunnedSmart->join();
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsClient::Stop exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Clear(void)
	{
		try {
			m_threadClientRunnedSmart.reset();
			m_contextSmart.reset();
		}
		catch (...) {
			//{SULOKI_ERROR_LOG_SULOKI << "WsClient::Clear exception"; }
			return Suloki::FAIL;
		}
		return Suloki::SUCCESS;
	}
#ifdef SULOKI_URC_SERVICE_SULOKIDEF
#else
	Suloki::Ret WriteAsync(const char* pData, int dataLen)
	{
		std::shared_ptr<T> clientSmartPtr;
		{
			ReadLock lock(m_rwLock);
			//if (m_clientSmartPtr.get() == NULL)
			//	return Suloki::FAIL;
			clientSmartPtr = m_clientSmartPtr;
			if (clientSmartPtr.get() == NULL)
				return Suloki::FAIL;
		}
		return clientSmartPtr->WriteAsync(pData, dataLen);
	}
	Suloki::Ret ReqRes(Suloki::SmartPtr<Suloki::Message> reqSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::Int timeout = 0)
	{
		std::shared_ptr<T> clientSmartPtr;
		{
			ReadLock lock(m_rwLock);
			//if (m_clientSmartPtr.get() == NULL)
			//	return Suloki::FAIL;
			clientSmartPtr = m_clientSmartPtr;
			if (clientSmartPtr.get() == NULL)
				return Suloki::FAIL;
		}
		return clientSmartPtr->ReqRes(reqSmart, resSmart, timeout);
	}
#endif
	/*
	std::shared_ptr<T> CreateClientSession(std::string ip, std::string port)
	{
		if (!m_bInited)
			std::shared_ptr<T>(NULL);
		auto const text = "wsasd";
		std::shared_ptr<T> clientSmartPtr = std::make_shared<T>(*m_contextSmart);
		clientSmartPtr->run(ip.c_str(), port.c_str(), text);
		return clientSmartPtr;
	}
	*/
protected:
	void RunClientFunction(void)
	{
#ifdef SULOKI_URC_SERVICE_SULOKIDEF
		std::string serverIpRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIP_KEY, serverIpRemoted);
		std::string serverPortRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORT_KEY, serverPortRemoted);
		if (serverPortRemoted.compare("0") == 0)
			return;
#else
		std::string serverIpRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIPREMOTED_KEY, serverIpRemoted);
		std::string serverPortRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORTREMOTED_KEY, serverPortRemoted);
		if (serverPortRemoted.compare("0") == 0)
			return;
#endif
		//
		//auto const host = "127.0.0.1";
		//auto const port = "8081";
		auto const text = "wsasd";

		while (Suloki::GlobalStateMachineSingleton::Instance().GetState() < Suloki::StateMachine::STOPPED)
		{
			m_contextSmart->restart();
			m_bInited = true;
			{
				WriteLock lock(m_rwLock);
				m_clientSmartPtr = std::make_shared<T>(*m_contextSmart);
				m_clientSmartPtr->run(serverIpRemoted.c_str(), serverPortRemoted.c_str(), text);
			}
			try {
				m_contextSmart->run();
			}
			catch (...) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "WsClient::RunClientFunction exception", Suloki::LOG_ERROR_LEVEL);
			}
			{
				WriteLock lock(m_rwLock);
				if (m_clientSmartPtr.get() != NULL)
					m_clientSmartPtr->Close();
				m_clientSmartPtr.reset();
			}
			m_contextSmart->stop();
			//{
			//	WriteLock lock(m_rwLock);
			//	m_clientSmartPtr.reset();
			//}
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "WsClient::RunClientFunction will restart", Suloki::LOG_ERROR_LEVEL);
			Suloki::Time::Sleep(256);
			m_bInited = false;
		}
	}
private:
	WsClient(WsClient& ref) {}
	WsClient& operator=(WsClient& ref) { return *this; }
protected:
	volatile bool m_bInited;
	LockPolicy m_rwLock;
	std::shared_ptr<T> m_clientSmartPtr;
	//
	std::auto_ptr<net::io_context> m_contextSmart;
	//
	std::auto_ptr<boost::thread> m_threadClientRunnedSmart;
};
//typedef Suloki::Singleton<WsClient> WsClientSingleton;

#endif

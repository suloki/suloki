#include "suloki.h"
#include "ws.h"

namespace Suloki
{
	std::string GetErrStr(Ret ret)
	{
		std::string str[] = { "success","unkoown fail","para invalid","state inside invalid","alloc mem or obj error","timeout","parse json error","invalid msg,maybe some field is not existed" };
		std::string strErrstr;
		if (ret >= SUCCESS && ret <= INVALIDMSG_ERRCODE)
		{
			strErrstr = str[ret];
		}
		else
		{
			strErrstr = "no error string mached";
		}
		/*switch (ret)
		{
		case SUCCESS:
		case FAIL:
		case INVALIDPARA_ERRCODE:
		case INVALIDSTATE_ERRCODE:
		case NOMEMORY_ERRCODE:
		case PARSEJSON_ERRCODE:
		case INVALIDMSG_ERRCODE:
		strErrstr = str[ret];
		break;
		default:
		strErrstr = "no error string mached";
		break;
		}*/
		return strErrstr;
	}
	//thread pool
	ThreadPool::ThreadPool(): m_worker(m_ioService), m_threadNum(MAXNUM_THREAD)
	{}
	ThreadPool::ThreadPool(Int threadNum) : m_worker(m_ioService), m_threadNum(MAXNUM_THREAD)
	{
		if (threadNum > 0 && threadNum <= MAXNUM_THREAD)
		{
			m_threadNum = threadNum;
		}
	}
	ThreadPool::~ThreadPool()
	{
		Clear();
	}
	Ret ThreadPool::Init(void)
	{
		if (GetState() != INITED)
		{
			SetState(INITED);
			for (Int i = 0; i < MAXNUM_THREAD; ++i)
			{
				m_threads.create_thread(boost::bind(&boost::asio::io_service::run, &m_ioService));
			}
		}
		return SUCCESS;
	}
	Ret ThreadPool::Start(void)
	{
		return StateMachine::Start();
	}
	Ret ThreadPool::Run(void)
	{
		return StateMachine::Run();
	}
	Ret ThreadPool::Stop(void)
	{
		return StateMachine::Stop();
	}
	Ret ThreadPool::Clear(void)
	{
		if (GetState() != CLEARED)
		{
			SetState(CLEARED);
			m_ioService.stop();
			m_threads.join_all();
		}
		return SUCCESS;
	}
	Ret ThreadPool::Init(Int threadNum)
	{
		if (GetState() != INITED)
		{
			SetState(INITED);
			for (Int i = 0; i < threadNum; ++i)
			{
				m_threads.create_thread(boost::bind(&boost::asio::io_service::run, &m_ioService));
			}
		}
		return SUCCESS;
	}
	//time management
	void Time::Sleep(Int mSec)
	{
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		::Sleep(mSec);
#else
		usleep(mSec * 1000);
#endif
	}
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
	//up use SULOKI_STDCOUT_SULOKI,down use log
	//log
	Log::Log()
	{
		m_bStop = false;
		m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&Log::RunFunction, this)));
		if (m_threadRunnedSmart.get() == NULL)
		{
			SULOKI_STDCOUT_SULOKI("new boost::thread in Log::Log() error");
		}
		m_threadDeledSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&Log::DelFunction, this)));
		if (m_threadDeledSmart.get() == NULL)
		{
			SULOKI_STDCOUT_SULOKI("new boost::thread in Log::Log() error1");
		}
		//m_file.open("log.txt", std::ios_base::out);
		m_iLevel = LOG_DEBUG_LEVEL;
	}
	Log::~Log()
	{
		m_bStop = true;
		if (m_threadRunnedSmart.get() != NULL)
		{
			m_threadRunnedSmart->join();
		}
		m_threadRunnedSmart.reset(NULL);
		if (m_threadDeledSmart.get() != NULL)
		{
			m_threadDeledSmart->join();
		}
		m_threadDeledSmart.reset(NULL);
		//if (m_file.is_open())
		//	m_file.close();
	}
	void Log::SetLogLevel(std::string logLevel)
	{
		std::map<std::string, Int> kvMap = { { LOG_STRTRACE_LEVEL,LOG_TRACE_LEVEL },{ LOG_STRDEBUG_LEVEL,LOG_DEBUG_LEVEL },{ LOG_STRINFO_LEVEL,LOG_INFO_LEVEL },{ LOG_STRWARN_LEVEL,LOG_WARN_LEVEL },{ LOG_STRERROR_LEVEL,LOG_ERROR_LEVEL },{ LOG_STRFATAL_LEVEL,LOG_FATAL_LEVEL } };
		std::map<std::string, Int>::iterator iter = kvMap.find(logLevel);
		if (iter != kvMap.end())
		{
			m_iLevel = iter->second;
		}
	}
	void Log::SetLogLevel(Int logLevel)
	{
		if (!(logLevel >= LOG_TRACE_LEVEL && logLevel <= LOG_FATAL_LEVEL))
		{
			return;
		}
		m_iLevel = logLevel;
	}
	Ret Log::WriteLog(OutputStream streamType, std::string strLog, Int iLevel, std::string file, Int line, std::string function)
	{
		if (streamType != STDCOUTTYPE && iLevel < m_iLevel)
		{
			return FAIL;
		}
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		std::stringstream strStream;
		if (streamType == STDCOUTTYPE)
			strStream << boost::posix_time::to_simple_string(now) << " " << strLog << std::endl;
		else
			strStream << boost::posix_time::to_simple_string(now) << " " << TurnItoS(iLevel) << " " << file << " " << line << " " << function << " : " << strLog << std::endl;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
		std::auto_ptr<LogItem> logItemSmart(new LogItem(strStream.str(), streamType));
		if (logItemSmart.get() == NULL)
		{
			//std::cout << "new std::string in Log::WriteLog error" << std::endl;
			SULOKI_STDCOUT_SULOKI("new std::string in Log::WriteLog error");
			return FAIL;
		}
		bool bRes = m_queue.push(logItemSmart.get());
		if (bRes)
		{
			logItemSmart.release();
			return SUCCESS;
		}
#else
#endif
		SULOKI_STDCOUT_SULOKI("m_queue.push in Log::WriteLog error");
		return FAIL;
	}
	std::string Log::TurnItoS(Int iLevel)
	{
		std::map<Int, std::string> kvMap = { { LOG_TRACE_LEVEL,LOG_STRTRACE_LEVEL },{ LOG_DEBUG_LEVEL,LOG_STRDEBUG_LEVEL },{ LOG_INFO_LEVEL,LOG_STRINFO_LEVEL },{ LOG_WARN_LEVEL,LOG_STRWARN_LEVEL },{ LOG_ERROR_LEVEL,LOG_STRERROR_LEVEL },{ LOG_FATAL_LEVEL,LOG_STRFATAL_LEVEL } };
		std::map<Int, std::string>::iterator iter = kvMap.find(iLevel);
		std::string strLevel = LOG_STRINFO_LEVEL;
		if (iter != kvMap.end())
		{
			strLevel = iter->second;
		}
		else
		{
			SULOKI_STDCOUT_SULOKI("Log::TurnItoS error");
		}
		return strLevel;
	}
	void Log::RunFunction(void)
	{
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		//std::string path("./log/");
		{
			//::CreateDirectory("./log", NULL);
			/*char cName[256 + 2];
			::GetModuleFileName(NULL, cName, 256);
			std::string strName(cName);
			size_t pos = strName.rfind("\\");
			path = strName.substr(0, pos+1);
			path += "log/";
			if(_access(path.c_str()) != 0)
			::CreateDirectory(path.c_str(), NULL);*/
		}
#else
		{
			/*char cName[256 + 2];
			const char* pSysFile = "/proc/self/exe";
			memset(cName, 0, 256 + 2);
			readlink(pSysFile, cName, 256);
			std::string strName(cName);
			size_t pos = strName.rfind("\\");
			path = strName.substr(0, pos+1);
			path += "log/";
			if(_access(path.c_str()) != 0)
			mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH);*/
		}
#endif
		std::string strPath = "./";// GlobalStateMachineSingleton::Instance().GetModulePath();
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		{
			std::string strPathTmp = strPath + "log/";
			::CreateDirectoryA(strPathTmp.c_str(), NULL);
		}
#else
		{
			std::string strPathTmp = strPath + "log/";
			mkdir(strPathTmp.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
		}
#endif
		//
		//#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		std::ofstream fileStream;
		//#else
		//			OutputStream fileStream;
		//#endif
		while (GlobalStateMachineSingleton::Instance().GetState() < StateMachine::STOPPED)
		{
			Int length = 0;
			std::string strLogName = "./log/log";
			if (strPath.length() > 0)
				strLogName = strPath + "log/log";
			std::string strDt = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
			strLogName += strDt;
			strLogName += ".txt";
			fileStream.open(strLogName, std::ios_base::out);
			if (!fileStream.is_open())
			{
				SULOKI_STDCOUT_SULOKI("fileStream.open() in Log::RunFunction() error");
				Time::Sleep(Time::TIMEOUT);
				continue;
			}
			while (!m_bStop || GlobalStateMachineSingleton::Instance().GetState() < StateMachine::STOPPED)
			{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
				LogItem* pLogstr = NULL;
				bool bRes = m_queue.pop(pLogstr);
				if (bRes)
				{
					std::auto_ptr<LogItem> logstrSmart(pLogstr);
					if (logstrSmart->m_stream == STDCOUTTYPE)
					{
						//SULOKI_STDCOUT_SULOKI(logstrSmart->m_logStr);
						std::cout << logstrSmart->m_logStr << std::endl;
					}
					else
						if (fileStream.is_open())
						{
							fileStream << logstrSmart->m_logStr;
							length += logstrSmart->m_logStr.length();
							if (length >= 1024 * 1024 * 16)
							{
								break;
							}
							fileStream.flush();
						}
					continue;
				}
#else
#endif
				Time::Sleep(Time::HALF_TIMEOUT);
			}
			if (fileStream.is_open())
			{
				fileStream.close();
			}
		}
	}
	void Log::DelFunction(void)
	{
		boost::posix_time::ptime startDt = boost::posix_time::second_clock::universal_time();
		while (GlobalStateMachineSingleton::Instance().GetState() < StateMachine::STOPPED)
		{
			boost::posix_time::ptime nowDt = boost::posix_time::second_clock::universal_time();
			boost::posix_time::time_duration timespan = nowDt - startDt;
			if (timespan.total_milliseconds() > 1000 * 60 * 60 * 24)
			{
				startDt = nowDt;
				std::string targetPath = "./log";
				boost::filesystem::path myPath(targetPath);
				boost::filesystem::directory_iterator endIter;
				for (boost::filesystem::directory_iterator iter(myPath); iter != endIter; iter++)
				{
					if (!boost::filesystem::is_directory(*iter))
					{
						std::time_t last = boost::filesystem::last_write_time(*iter);
						boost::posix_time::ptime lastDt = boost::posix_time::from_time_t(last);
						boost::posix_time::ptime nowDt = boost::posix_time::second_clock::universal_time();
						//std::cout << iter->path().string() << std::endl;
						boost::posix_time::time_duration timespan = nowDt - lastDt;
						if (timespan.total_milliseconds() > 1000 * 60 * 60 * 5)
						{
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
							//::DeleteFile(iter->path().string().c_str());
#else
							//assert()
#endif
						}
					}
				}
			}
			Time::Sleep(Time::HALF_TIMEOUT);
		}
	}
	//config
	Config::Config()
	{
		SetDefault();
	}
	Ret Config::ReadConfig(void)
	{
		SetDefault();
		//
		std::string strPath = "./";// GlobalStateMachineSingleton::Instance().GetModulePath();
		std::string strName = CONFIGNAME;
		if (strPath.length() > 0)
			strName = strPath + CONFIGNAME;
		//
		try {
			boost::property_tree::ptree root;
			boost::property_tree::read_json<boost::property_tree::ptree>(strName, root);
			std::string logLevel = root.get<std::string>(CONFIG_LOGLEVEL_KEY);
			if (!(logLevel == LOG_STRTRACE_LEVEL || logLevel == LOG_STRDEBUG_LEVEL || logLevel == LOG_STRINFO_LEVEL || logLevel == LOG_STRWARN_LEVEL || logLevel == LOG_STRERROR_LEVEL || logLevel == LOG_STRFATAL_LEVEL))
			{
				SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "read config CONFIG_LOGLEVEL_KEY error", LOG_ERROR_LEVEL);
				return FAIL;
			}
			SetConfig(CONFIG_LOGLEVEL_KEY, logLevel);
			//
			std::string restart = root.get<std::string>(CONFIG_RESTART_KEY);
			if (restart.compare(CONFIG_TRUE_RESTART_VAL) != 0)
				restart = CONFIG_FALSE_RESTART_VAL;
			SetConfig(CONFIG_RESTART_KEY, restart);
			//
			std::string pstorage = root.get<std::string>(CONFIG_PSTORAGE_KEY);
			if (pstorage.compare(CONFIG_TRUE_RESTART_VAL) != 0)
				pstorage = CONFIG_FALSE_RESTART_VAL;
			SetConfig(CONFIG_PSTORAGE_KEY, pstorage);
			//
			std::string groupname = root.get<std::string>(CONFIG_GROUPNAME_KEY);
			SetConfig(CONFIG_GROUPNAME_KEY, groupname);
			std::string servicename = root.get<std::string>(CONFIG_SERVICENAME_KEY);
			SetConfig(CONFIG_SERVICENAME_KEY, servicename);
			//
			std::string serviceTypr = root.get<std::string>(CONFIG_SERVICETYPE_KEY);
			if (serviceTypr.compare(CONFIG_CPLUSPLUS_SERVICETYPE_VAL) == 0 || serviceTypr.compare(CONFIG_JAVASCRIPT_SERVICETYPE_VAL) == 0)
				SetConfig(CONFIG_SERVICETYPE_KEY, serviceTypr);
			std::string serviceThreads = root.get<std::string>(CONFIG_SERVICETHREADS_KEY);
			if (atoi(serviceThreads.c_str()) <= 0)
				serviceThreads = "1";
			{
				//Int num = atoi(serviceThreads.c_str()) + 1;
				//std::stringstream strStream;
				//strStream << num;
				SetConfig(CONFIG_SERVICETHREADS_KEY, serviceThreads);
			}
			//
			std::string serverIp = root.get<std::string>(CONFIG_UDPSERVERIP_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_UDPSERVERIP_KEY, serverIp);
			std::string serverPort = root.get<std::string>(CONFIG_UDPSERVERPORT_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_UDPSERVERPORT_KEY, serverPort);
			//
			std::string remoteserverIp = root.get<std::string>(CONFIG_REMOTEUDPSERVERIP_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_REMOTEUDPSERVERIP_KEY, remoteserverIp);
			std::string remoteserverPort = root.get<std::string>(CONFIG_REMOTEUDPSERVERPORT_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_REMOTEUDPSERVERPORT_KEY, remoteserverPort);
			//
			std::string wsserverIp = root.get<std::string>(CONFIG_WSSERVERIP_KEY);
			SetConfig(CONFIG_WSSERVERIP_KEY, wsserverIp);
			std::string wsserverPort = root.get<std::string>(CONFIG_WSSERVERPORT_KEY);
			SetConfig(CONFIG_WSSERVERPORT_KEY, wsserverPort);
			//
			std::string wsserverIpRemoted = root.get<std::string>(CONFIG_WSSERVERIPREMOTED_KEY);
			SetConfig(CONFIG_WSSERVERIPREMOTED_KEY, wsserverIpRemoted);
			std::string wsserverPortRemoted = root.get<std::string>(CONFIG_WSSERVERPORTREMOTED_KEY);
			SetConfig(CONFIG_WSSERVERPORTREMOTED_KEY, wsserverPortRemoted);
			//
			/*
			std::string serverIp = root.get<std::string>(CONFIG_TCPSERVERIP_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_TCPSERVERIP_KEY, serverIp);
			std::string serverPort = root.get<std::string>(CONFIG_TCPSERVERPORT_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_TCPSERVERPORT_KEY, serverPort);
			//
			std::string remoteserverIp = root.get<std::string>(CONFIG_REMOTETCPSERVERIP_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_REMOTETCPSERVERIP_KEY, remoteserverIp);
			std::string remoteserverPort = root.get<std::string>(CONFIG_REMOTETCPSERVERPORT_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_REMOTETCPSERVERPORT_KEY, remoteserverPort);
			//
			std::string wsserverIp = root.get<std::string>(CONFIG_WSSERVERIP_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_WSSERVERIP_KEY, wsserverIp);
			std::string wsserverPort = root.get<std::string>(CONFIG_WSSERVERPORT_KEY);
			ConfigSingleton::Instance().SetConfig(CONFIG_WSSERVERPORT_KEY, wsserverPort);
			//
			//boost::property_tree::ptree itemModuleinfo = root.get_child(SULOKI_MODULEINFO_KEY_CONFIG_BASE);
			//boost::property_tree::ptree::iterator iterModuleinfo = itemModuleinfo.begin();
			//while (iterModuleinfo != itemModuleinfo.end())
			//{
			//	Module module;
			//	module.m_name = iterModuleinfo->second.get<std::string>("name");
			//	module.m_path = iterModuleinfo->second.get<std::string>("path");
			//	module.m_config = iterModuleinfo->second.get<std::string>("config");
			//	{
			//		WriteLock lock(m_rwLock);
			//		m_doduleVector.push_back(module);
			//	}
			//	iterModuleinfo++;
			//}
			//
			//std::string moduleName = root.get<std::string>(SULOKI_MODULENAME_KEY_CONFIG_BASE);
			//ConfigSingleton::Instance().SetConfig(SULOKI_MODULENAME_KEY_CONFIG_BASE, moduleName);
			//std::string moduleScript = root.get<std::string>(SULOKI_MODULESCRIPT_KEY_CONFIG_BASE);
			//ConfigSingleton::Instance().SetConfig(SULOKI_MODULESCRIPT_KEY_CONFIG_BASE, moduleScript);
			//boost::property_tree::ptree item = root.get_child(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE);
			//boost::property_tree::ptree::iterator iter = item.begin();
			//if(iter != item.end())
			//{
			//	std::string address = iter->second.get<std::string>(SULOKI_ADDRESS_KEY_CONFIG_BASE);
			//	ConfigSingleton::Instance().SetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, address);
			//	iter++;
			//}
			//if(iter != item.end())
			//{
			//	std::string address = iter->second.get<std::string>(SULOKI_ADDRESS_KEY_CONFIG_BASE);
			//	std::cout << address << std::endl;
			//	iter++;
			//}
			//if(iter != item.end())
			//	throw Exception("much address");
			*/
		}
		catch (boost::property_tree::ptree_error pt) {
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "read config excepion", LOG_ERROR_LEVEL);
			return FAIL;
		}
		catch (...) {
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "read config excepion1", LOG_ERROR_LEVEL);
			return FAIL;
		}
		return SUCCESS;
	}
	Ret Config::SetConfig(std::string key, std::string val)
	{
		if (!(key.length() > 0 && val.length() > 0))
			return INVALIDPARA_ERRCODE;
		//WriteLock lock(m_rwLock);
		m_configMap[key] = val;
		return SUCCESS;
	}
	Ret Config::GetConfig(std::string key, std::string& val)
	{
		if (!(key.length() > 0))
			return INVALIDPARA_ERRCODE;
		//ReadLock lock(m_rwLock);
		std::map<std::string, std::string>::iterator iter = m_configMap.find(key);
		if (iter == m_configMap.end())
		{
			return FAIL;
		}
		val = iter->second;
		return SUCCESS;
	}
	void Config::SetDefault(void)
	{
		{
			//WriteLock lock(m_rwLock);
			m_configMap.clear();
		}
		SetConfig(CONFIG_LOGLEVEL_KEY, LOG_STRDEBUG_LEVEL);
		SetConfig(CONFIG_RESTART_KEY, CONFIG_FALSE_RESTART_VAL);
		SetConfig(CONFIG_PSTORAGE_KEY, CONFIG_FALSE_RESTART_VAL);
		SetConfig(CONFIG_GROUPNAME_KEY, "");
		SetConfig(CONFIG_SERVICENAME_KEY, "");
		SetConfig(CONFIG_SERVICETYPE_KEY, CONFIG_CPLUSPLUS_SERVICETYPE_VAL);
		SetConfig(CONFIG_SERVICETHREADS_KEY, CONFIG_SERVICETHREADS_VAL);
		//
		SetConfig(CONFIG_UDPSERVERIP_KEY, "127.0.0.1");
		SetConfig(CONFIG_UDPSERVERPORT_KEY, "0");
		SetConfig(CONFIG_REMOTEUDPSERVERIP_KEY, "127.0.0.1");
		SetConfig(CONFIG_REMOTEUDPSERVERPORT_KEY, "0");
		//
		SetConfig(CONFIG_WSSERVERIP_KEY, "127.0.0.1");
		SetConfig(CONFIG_WSSERVERPORT_KEY, "0");
		SetConfig(CONFIG_WSSERVERIPREMOTED_KEY, "127.0.0.1");
		SetConfig(CONFIG_WSSERVERPORTREMOTED_KEY, "0");
	}
	//
	Ret Message::MakeJsonMsg(boost::property_tree::ptree& jsonMsg)
	{
		boost::property_tree::ptree root;
		try {
			root.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
			root.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_TEST_MESSAGEID_VAL_SULOKIDEF);
			root.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_NOTICE_TYPE_VAL_SULOKIDEF);
			std::stringstream strStream;
			strStream << IdManagerSingleton::Instance().GetId();
			root.put<std::string>(SULOKI_SEQUENCEID_MSGFIELD_KEY_SULOKIDEF, strStream.str());
		}
		catch (boost::property_tree::ptree_error pt) {
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "put json field exception", LOG_ERROR_LEVEL);
			return FAIL;
		}
		catch (...) {
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "put json field exception1", LOG_ERROR_LEVEL);
			return FAIL;
		}
		jsonMsg = root;
		return SUCCESS;
	}
	Ret Message::CheckJsonMsg(boost::property_tree::ptree& jsonMsg)
	{
		std::string strVal;
		try {
			strVal = jsonMsg.get<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF);
			strVal = jsonMsg.get<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF);
			strVal = jsonMsg.get<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF);
			strVal = jsonMsg.get<std::string>(SULOKI_SEQUENCEID_MSGFIELD_KEY_SULOKIDEF);
		}
		catch (boost::property_tree::ptree_error pt) {
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "get json field exception", LOG_ERROR_LEVEL);
			return FAIL;
		}
		catch (...) {
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "get json field exception1", LOG_ERROR_LEVEL);
			return FAIL;
		}
		return SUCCESS;
	}
	//
	Ret EventManager::Init(void)
	{
		for (Int i = 0; i <= INITNUM_TIMER; i++)
		{
			SmartPtr<TimerItem> timerSmart(new TimerItem());
			if (timerSmart.get() == NULL || timerSmart->Init(m_ioService) != SUCCESS)
			{
				SULOKI_WRITELOG_SULOKI(Log::FILETYPE, GetErrStr(NOMEMORY_ERRCODE), LOG_ERROR_LEVEL);
				return NOMEMORY_ERRCODE;
			}
			m_timerQueue.Push(timerSmart);
		}
		m_timerForeverSmart = m_timerQueue.Pop();
		for (Int i = 0; i < INITNUM_THREAD; i++)
		{
			SmartPtr<ConvarItem> convarSmart(new ConvarItem());
			if (convarSmart.get() == NULL)
			{
				SULOKI_WRITELOG_SULOKI(Log::FILETYPE, GetErrStr(NOMEMORY_ERRCODE), LOG_ERROR_LEVEL);
				return NOMEMORY_ERRCODE;
			}
			m_convarQueue.Push(convarSmart);
		}
		return SUCCESS;
	}
	Ret EventManager::Start(void)
	{
		m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&EventManager::RunThread, this)));
		if (m_threadRunnedSmart.get() == NULL)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, GetErrStr(NOMEMORY_ERRCODE), LOG_ERROR_LEVEL);
			return NOMEMORY_ERRCODE;
		}
		return SUCCESS;
	}
	Ret EventManager::Run(void)
	{
		return SUCCESS;
	}
	Ret EventManager::Stop(void)
	{
		m_timerQueue.Push(m_timerForeverSmart);
		m_timerQueue.Clear();
		//
		m_ioService.stop();
		//
		if (m_threadRunnedSmart.get() != NULL)
		{
			m_threadRunnedSmart->join();
		}
		return SUCCESS;
	}
	Ret EventManager::Clear(void)
	{
		m_threadRunnedSmart.reset(NULL);
		return SUCCESS;
	}
	SmartPtr<EventManager::ConvarItem> EventManager::PopConvar(void)
	{
		SmartPtr<TimerItem> timerSmart = m_timerQueue.Pop();
		if (timerSmart.IsNULL())
		{
			timerSmart = SmartPtr<TimerItem>(new TimerItem());
			if (timerSmart.get() == NULL || timerSmart->Init(m_ioService) != SUCCESS)
				return SmartPtr<EventManager::ConvarItem>(NULL);
		}
		SmartPtr<EventManager::ConvarItem> convarSmart = m_convarQueue.Pop();
		convarSmart->m_timerSmart = timerSmart;
		return convarSmart;
	}
	Ret EventManager::PushConvar(SmartPtr<ConvarItem> convarSmart)
	{
		if (convarSmart.IsNULL())
			return SUCCESS;
		if (!convarSmart->m_timerSmart.IsNULL())
			m_timerQueue.Push(convarSmart->m_timerSmart);
		convarSmart->m_timerSmart = SmartPtr<TimerItem>(NULL);
		convarSmart->m_msgSmart = SmartPtr<Message>(NULL);
		m_convarQueue.Push(convarSmart);
		return SUCCESS;
	}
	Ret EventManager::Wait(std::string strUrcKey, Int timeout, SmartPtr<Message>& msgSmart)
	{
		SmartPtr<Base> baseSmart;
		if (Suloki::UrcSingleton::Instance().GetUr(strUrcKey, baseSmart) != Suloki::SUCCESS)
		{
			std::stringstream strStream;
			strStream << "AddUr error.urname:" << strUrcKey;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		SmartPtr<ConvarItem> convarSmart = baseSmart.Upcast<ConvarItem>();
		/*
		SmartPtr<TimerItem> timerSmart = m_timerQueue.Pop();
		if (timerSmart.IsNULL())
		{
		timerSmart = SmartPtr<TimerItem>(new TimerItem());
		if (timerSmart.get() == NULL || timerSmart->Init(m_ioService) != SUCCESS)
		{
		m_convarQueue.Push(convarSmart);
		Suloki::UrcSingleton::Instance().DelUr(strUrcKey, baseSmart);
		return FAIL;
		}
		}
		*/
		convarSmart->m_timerSmart->GetTimer().expires_from_now(std::chrono::milliseconds(timeout));
		convarSmart->m_timerSmart->GetTimer().async_wait(std::bind(&EventManager::MyTimeout, this, strUrcKey));// , timerSmart, convarSmart));
																											   //
		bool bTimeout = false;
		{
			boost::unique_lock<boost::mutex> lock(convarSmart->m_mutex);
			convarSmart->m_ready = false;
			convarSmart->m_bTimeout = false;
			convarSmart->m_msgSmart = SmartPtr<Message>(NULL);
			while (!convarSmart->m_ready)
				convarSmart->m_cond.wait(lock);
			bTimeout = convarSmart->m_bTimeout;
			msgSmart = convarSmart->m_msgSmart;
		}
		//
		convarSmart->m_timerSmart->GetTimer().cancel();
		//m_timerQueue.Push(convarSmart->m_timerSmart);
		//convarSmart->m_timerSmart = SmartPtr<TimerItem>(NULL);
		//convarSmart->m_msgSmart = SmartPtr<Message>(NULL);
		//m_convarQueue.Push(convarSmart);
		PushConvar(convarSmart);
		Suloki::UrcSingleton::Instance().DelUr(strUrcKey, baseSmart);
		if (bTimeout)
			return TIMEOUT_ERRCODE;
		//else
		//	timerSmart->GetTimer().cancel();
		return SUCCESS;
	}
	Ret EventManager::Notify(SmartPtr<Message> msgSmart)
	{
		if (msgSmart.IsNULL())
			return INVALIDPARA_ERRCODE;
		std::string strBusinessId = msgSmart->GetField(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF);
		std::string strMessageId = msgSmart->GetField(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF);
		std::string strType = msgSmart->GetField(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF);
		std::string strSequenceId = msgSmart->GetField(SULOKI_SEQUENCEID_MSGFIELD_KEY_SULOKIDEF);
		if (!(strBusinessId.length() > 0 && strMessageId.length() > 0 && strType.compare(SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF) == 0 && strSequenceId.length() > 0))
			return INVALIDPARA_ERRCODE;
		std::string strUrName;
		std::stringstream strStream;
		strStream << Suloki::URC_SYSTEM_RESPONSE_NAMEPREFIX << strBusinessId << "_" << strMessageId << "_" << strSequenceId;
		strUrName = strStream.str();
		SmartPtr<Base> baseSmart;
		if (!(Suloki::UrcSingleton::Instance().DelUr(strUrName, baseSmart) == Suloki::SUCCESS && !baseSmart.IsNULL()))
		{
			std::stringstream strStream;
			strStream << "DelUr error.urname:" << strUrName;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		SmartPtr<ConvarItem> convarSmart = baseSmart.Upcast<ConvarItem>();
		//if (!(id >= 0 && id < MAXNUM_THREAD && msgSmart.get() != NULL && msgSmart->messagetype() == SulokiMessage::response))
		//	return INVALIDPARA_ERRORCODE;
		//
		//std::stringstream strStream;
		//strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msgSmart->businessid << "_" << msgSmart->messageid << "_" << msgSmart->sequencenumber;
		//
		//boost::function<void(Int, std::string, std::auto_ptr<SulokiMessage>, bool)> func;
		//if (UrcSingleton::Instance().DelUr_Urcsys(strStream.str(), func) != SUCCESS)
		//	return URC_UNEXISTED;
		//
		boost::unique_lock<boost::mutex> lock(convarSmart->m_mutex);
		convarSmart->m_ready = true;
		convarSmart->m_msgSmart = msgSmart;
		convarSmart->m_cond.notify_one();
		return SUCCESS;
	}
	/*Ret EventManager::SetTimer(std::string strUrName, Int timeout)
	{
	SmartPtr<TimerItem> timerSmart = m_timerQueue.Pop();
	if (timerSmart.IsNULL())
	{
	timerSmart = SmartPtr<TimerItem>(new TimerItem());
	if (timerSmart.get() == NULL || timerSmart->Init(m_ioService) != SUCCESS)
	return FAIL;
	}
	timerSmart->GetTimer().expires_from_now(std::chrono::milliseconds(timeout));
	timerSmart->GetTimer().async_wait(std::bind(&EventManager::MyAsyncTimeout, this, strUrName, timerSmart));
	return SUCCESS;
	}*/
	Ret EventManager::SetTimerDelObjUr(std::string strUrcKey, Int timeout)
	{
		SmartPtr<TimerItem> timerSmart = m_timerQueue.Pop();
		if (timerSmart.IsNULL())
		{
			timerSmart = SmartPtr<TimerItem>(new TimerItem());
			if (timerSmart.get() == NULL || timerSmart->Init(m_ioService) != SUCCESS)
				return FAIL;
		}
		timerSmart->GetTimer().expires_from_now(std::chrono::milliseconds(timeout));
		timerSmart->GetTimer().async_wait(std::bind(&EventManager::MyTimeoutDelObjUr, this, strUrcKey, timerSmart));
		return SUCCESS;
	}
	void EventManager::MyTimeout(std::string strUrcKey)//, SmartPtr<TimerItem> timerSmart, SmartPtr<ConvarItem> convarSmart)
	{
		//m_timerQueue.Push(timerSmart);
		//
		//boost::function<void(Int, std::string, std::auto_ptr<SulokiMessage>, bool)> func;
		//boost::function<void(Uint, bool)> func;
		//Urc::AsyncNewFunc func;
		//if (UrcSingleton::Instance().DelUr_Urcsys<Urc::AsyncNewFunc>(strUrcKey, func) != SUCCESS)
		//	return;
		SmartPtr<Base> baseSmart;
		if (!(Suloki::UrcSingleton::Instance().DelUr(strUrcKey, baseSmart) == Suloki::SUCCESS && !baseSmart.IsNULL()))
		{
			std::stringstream strStream;
			strStream << "DelUr error.urname:" << strUrcKey;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
			return;
		}
		SmartPtr<ConvarItem> convarSmart = baseSmart.Upcast<ConvarItem>();
		//
		boost::unique_lock<boost::mutex> lock(convarSmart->m_mutex);
		convarSmart->m_ready = true;
		convarSmart->m_bTimeout = true;
		convarSmart->m_cond.notify_one();
	}
	//void EventManager::MyAsyncTimeout(std::string strUrcKey, SmartPtr<TimerItem> timerSmart)
	//{
	//	m_timerQueue.Push(timerSmart);
	//	//
	//	boost::property_tree::ptree root;
	//	try {
	//		root.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
	//		root.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_TIMEOUT_MESSAGEID_VAL_SULOKIDEF);
	//		root.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
	//		std::stringstream strStream;
	//		strStream << IdManagerSingleton::Instance().GetId();
	//		root.put<std::string>(SULOKI_SEQUENCEID_MSGFIELD_KEY_SULOKIDEF, strStream.str());
	//	}
	//	catch (boost::property_tree::ptree_error pt) {
	//		SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "handle json exception", LOG_ERROR_LEVEL);
	//		return;
	//	}
	//	catch (...) {
	//		SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "handle json exception1", LOG_ERROR_LEVEL);
	//		return;
	//	}
	//	SmartPtr<Message> msgSmart(new Message(root));
	//	if (msgSmart.IsNULL())
	//	{
	//		SULOKI_WRITELOG_SULOKI(Log::FILETYPE, GetErrStr(NOMEMORY_ERRCODE), LOG_ERROR_LEVEL);
	//		return;
	//	}
	//	//
	//	//if (UrcSingleton::Instance().Response(strUrcKey, msgSmart, SmartPtr<Context>(NULL)) != SUCCESS)
	//	//{
	//	//	SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "PostMsgSubUr error", LOG_ERROR_LEVEL); 
	//	//	return;
	//	//}
	//	//
	//}
	void EventManager::MyTimeoutDelObjUr(std::string strUrName, SmartPtr<TimerItem> timerSmart)
	{
		m_timerQueue.Push(timerSmart);
		//
		Suloki::SmartPtr< Suloki::Base > baseptrSmart;
		if (Suloki::UrcSingleton::Instance().GetUr(strUrName, baseptrSmart) == SUCCESS)
		{
			Suloki::SmartPtr< SessionSwrap > wssessionSwrap = baseptrSmart.Upcast<SessionSwrap>();
			if (!wssessionSwrap->GetSession()->GetLogined())
			{
				wssessionSwrap->GetSession()->Close();
				Suloki::UrcSingleton::Instance().DelUr(strUrName, baseptrSmart);
			}
		}
	}
	void EventManager::MyTimeoutForever(std::string strUrcKey)
	{
		if (GlobalStateMachineSingleton::Instance().GetState() < StateMachine::STOPPED)
		{
			m_timerForeverSmart->GetTimer().expires_from_now(std::chrono::milliseconds(1000));
			m_timerForeverSmart->GetTimer().async_wait(std::bind(&EventManager::MyTimeoutForever, this, strUrcKey));
		}
	}
	void EventManager::RunThread(void)
	{
		m_timerForeverSmart->GetTimer().expires_from_now(std::chrono::milliseconds(1000));
		m_timerForeverSmart->GetTimer().async_wait(std::bind(&EventManager::MyTimeoutForever, this, ""));
		m_ioService.run();
		return;
	}
	//
	Ret ServiceManager::Init(void)
	{
		std::vector<std::string> nameVector;
		if (UrcSingleton::Instance().GetUr(URC_SYSTEM_NAMEPREFIX, nameVector) == SUCCESS)
		{
			for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
			{
				SmartPtr<Base> baseSmartPtr;
				UrcSingleton::Instance().GetUr(*iter, baseSmartPtr);
				if (baseSmartPtr.IsNULL())
					continue;
				SmartPtr< Service > serviceSmartPtr = baseSmartPtr.Upcast<Service>();
				if (serviceSmartPtr->Init(serviceSmartPtr->m_pUrcInterface, serviceSmartPtr->m_groupName, serviceSmartPtr->m_serviceName) != SUCCESS)
				{
					std::stringstream strSTream;
					strSTream << "service Init error.m_groupName:" << serviceSmartPtr->m_groupName << ";m_serviceName:" << serviceSmartPtr->m_serviceName;
					SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strSTream.str(), Suloki::LOG_ERROR_LEVEL);
					return FAIL;
				}
			}
		}
		return SUCCESS;
	}
	Ret ServiceManager::Start(void)
	{
		std::vector<std::string> nameVector;
		if (UrcSingleton::Instance().GetUr(URC_SYSTEM_NAMEPREFIX, nameVector) == SUCCESS)
		{
			for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
			{
				SmartPtr<Base> baseSmartPtr;
				UrcSingleton::Instance().GetUr(*iter, baseSmartPtr);
				if (baseSmartPtr.IsNULL())
					continue;
				SmartPtr< Service > serviceSmartPtr = baseSmartPtr.Upcast<Service>();
				if (serviceSmartPtr->Start() != SUCCESS)
				{
					std::stringstream strSTream;
					strSTream << "service Start error.m_groupName:" << serviceSmartPtr->m_groupName << ";m_serviceName:" << serviceSmartPtr->m_serviceName;
					SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strSTream.str(), Suloki::LOG_ERROR_LEVEL);
					return FAIL;
				}
			}
		}
		return SUCCESS;
	}
	Ret ServiceManager::Run(void)
	{
		return SUCCESS;
	}
	Ret ServiceManager::Stop(void)
	{
		std::vector<std::string> nameVector;
		if (UrcSingleton::Instance().GetUr(URC_SYSTEM_NAMEPREFIX, nameVector) == SUCCESS)
		{
			for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
			{
				SmartPtr<Base> baseSmartPtr;
				UrcSingleton::Instance().GetUr(*iter, baseSmartPtr);
				if (baseSmartPtr.IsNULL())
					continue;
				SmartPtr< Service > serviceSmartPtr = baseSmartPtr.Upcast<Service>();
				serviceSmartPtr->Stop();
			}
		}
		return SUCCESS;
	}
	Ret ServiceManager::Clear(void)
	{
		std::vector<std::string> nameVector;
		if (UrcSingleton::Instance().GetUr(URC_SYSTEM_NAMEPREFIX, nameVector) == SUCCESS)
		{
			for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
			{
				SmartPtr<Base> baseSmartPtr;
				UrcSingleton::Instance().GetUr(*iter, baseSmartPtr);
				if (baseSmartPtr.IsNULL())
					continue;
				SmartPtr< Service > serviceSmartPtr = baseSmartPtr.Upcast<Service>();
				serviceSmartPtr->Clear();
			}
		}
		return SUCCESS;
	}
	Ret ServiceManager::AddServide(std::string groupName, std::string serviceName, SmartPtr< Service > serviceSmart)
	{
		//
		SmartPtr< Base > baseSmart = serviceSmart.Downcast<Base>();
		std::stringstream strStream;
		strStream << URC_SYSTEM_NAMEPREFIX << groupName << "_" << serviceName;
		if (UrcSingleton::Instance().AddUr(strStream.str(), baseSmart, URC_OBJECT_TYPE, 0, true, 0) != SUCCESS)
		{
			return FAIL;
		}
		std::stringstream strStreamQueueKey;
		strStreamQueueKey << strStream.str() << URC_PARAQUEUE_NAMEKEY;
		Queue<Message> queue;
		if (UrcSingleton::Instance().AddUr(strStreamQueueKey.str(), queue, URC_MSGQUEUE_TYPE, URC_ZERO_ATTR | URC_EXCLUSIVE_ATTR, false, 0) != SUCCESS)
		{
			UrcSingleton::Instance().DelUr(strStream.str(), baseSmart);
			return FAIL;
		}
		{
			std::string strThreadsVal;
			ConfigSingleton::Instance().GetConfig(CONFIG_SERVICETHREADS_KEY, strThreadsVal);
			//????? del for mul thread from config
			//if (groupName.compare(URC_MAINGROUP_SERVICE) == 0 && serviceName.compare(URC_MAINSERVICE_SERVICE) == 0)
			//	strThreadsVal = "1";
			//
			const std::string strUcKeySub = groupName + "_" + serviceName;
			AsyncFunc func(Handle);
			SmartPtr<Message> msgOriSmart(new Message("{}"));
			std::vector<std::string> context;
			context.push_back(strStream.str());
			SmartPtr<Context> contextOriSmart(new Context(context));
			if (UrcSingleton::Instance().SubscribeUr(strStreamQueueKey.str(), strUcKeySub, func, msgOriSmart, contextOriSmart, atoi(strThreadsVal.c_str()) + 1) != SUCCESS)
			{
				//{SULOKI_ERROR_LOG_SULOKI << "SubscribeUr error.urname:" << strStreamQueueKey.str(); }
				Queue<Message> queue;
				UrcSingleton::Instance().DelUr(strStreamQueueKey.str(), queue);
				UrcSingleton::Instance().DelUr(strStream.str(), baseSmart);
				return FAIL;
			}
		}
		//
		return SUCCESS;
	}
	Ret ServiceManager::RemoveServide(std::string groupName, std::string serviceName, SmartPtr< Service >& serviceSmart)
	{
		std::stringstream strStream;
		strStream << URC_SYSTEM_NAMEPREFIX << groupName << "_" << serviceName;
		std::stringstream strStreamQueueKey;
		strStreamQueueKey << strStream.str() << URC_PARAQUEUE_NAMEKEY;
		{
			std::string strThreadsVal;
			ConfigSingleton::Instance().GetConfig(CONFIG_SERVICETHREADS_KEY, strThreadsVal);
			if (groupName.compare(URC_MAINGROUP_SERVICE) == 0 && serviceName.compare(URC_MAINSERVICE_SERVICE) == 0)
				strThreadsVal = "1";
			//
			const std::string strUcKeySub = groupName + "_" + serviceName;
			AsyncFunc func;
			SmartPtr<Message> msgOriSmart;
			SmartPtr<Context> contextOriSmart;
			UrcSingleton::Instance().UnsubscribeUr(strStreamQueueKey.str(), strUcKeySub, func, msgOriSmart, contextOriSmart, atoi(strThreadsVal.c_str()) + 1);
		}
		Queue<Message> queue;
		UrcSingleton::Instance().DelUr(strStreamQueueKey.str(), queue);
		{
			SmartPtr< Base > baseSmart;
			std::stringstream strStream;
			strStream << URC_SYSTEM_NAMEPREFIX << groupName << "_" << serviceName;
			if (UrcSingleton::Instance().DelUr(strStream.str(), baseSmart) == SUCCESS)
			{
				serviceSmart = baseSmart.Upcast<Service>();
			}
		}
		return SUCCESS;
	}
	Ret ServiceManager::GetServide(std::string groupName, std::string serviceName, SmartPtr< Service >& serviceSmart)
	{
		SmartPtr< Base > baseSmart;
		std::stringstream strStream;
		strStream << URC_SYSTEM_NAMEPREFIX << groupName << "_" << serviceName;
		if (UrcSingleton::Instance().GetUr(strStream.str(), baseSmart) == SUCCESS)
		{
			serviceSmart = baseSmart.Upcast<Service>();
			return SUCCESS;
		}
		return FAIL;
	}
	Ret ServiceManager::SubUr(std::string groupName, std::string serviceName, std::string strUrName)
	{
		std::string strVal;
		SmartPtr< Base > baseSmart;
		if (!(UrcSingleton::Instance().GetUr(strUrName, strVal) == SUCCESS || UrcSingleton::Instance().GetUr(strUrName, baseSmart) == SUCCESS))
		{
			return FAIL;
		}
		std::stringstream strStream;
		strStream << URC_SYSTEM_NAMEPREFIX << groupName << "_" << serviceName;
		{
			const std::string strUcKeySub = groupName + "_" + serviceName;
			AsyncFunc func(Handle);
			SmartPtr<Message> msgOriSmart(new Message("{}"));
			std::vector<std::string> context;
			context.push_back(strStream.str());
			SmartPtr<Context> contextOriSmart(new Context(context));
			if (UrcSingleton::Instance().SubscribeUr(strUrName, strUcKeySub, func, msgOriSmart, contextOriSmart) != SUCCESS)
			{
				return FAIL;
			}
		}
		return SUCCESS;
	}
	Ret ServiceManager::UnsubUr(std::string groupName, std::string serviceName, std::string strUrName)
	{
		std::string strVal;
		SmartPtr< Base > baseSmart;
		if (!(UrcSingleton::Instance().GetUr(strUrName, strVal) == SUCCESS || UrcSingleton::Instance().GetUr(strUrName, baseSmart) == SUCCESS))
		{
			return FAIL;
		}
		std::stringstream strStream;
		strStream << URC_SYSTEM_NAMEPREFIX << groupName << "_" << serviceName;
		{
			const std::string strUcKeySub = groupName + "_" + serviceName;
			AsyncFunc func;
			SmartPtr<Message> msgOriSmart;
			SmartPtr<Context> contextOriSmart;
			UrcSingleton::Instance().UnsubscribeUr(strUrName, strUcKeySub, func, msgOriSmart, contextOriSmart);
		}
		return SUCCESS;
	}
	void ServiceManager::Handle(SmartPtr<Message> msgOriSmart, SmartPtr<Context> contestOriSmart, SmartPtr<Message>& msgSmart, SmartPtr<Context>& contestSmart)
	{
		//????? if contestOriSmart is empty,handle to main service
		std::vector<std::string> constents = contestOriSmart->Get();
		std::stringstream strStream;
		if (constents.size() > 0)
			strStream << constents[0];
		else
			strStream << URC_SYSTEM_NAMEPREFIX << URC_MAINGROUP_SERVICE << "_" << URC_MAINSERVICE_SERVICE;
		SmartPtr< Base > baseSmart;
		if (UrcSingleton::Instance().GetUr(strStream.str(), baseSmart) != SUCCESS)
		{
			return;
		}
		SmartPtr< Service > mainserviceSmart = baseSmart.Upcast<Service>();
		SmartPtr<Message> resSmart;
		SmartPtr<Context> paraOutSmart;
		mainserviceSmart->Handle(msgSmart, contestSmart, resSmart, paraOutSmart);
	}
#ifdef SULOKI_DUMP_SULOKIDEF
	//about dump
	std::string g_dumpName = "suloki_dump";
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
	//code when unhandle exception
	LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo)
	{
		std::string strDumpName = g_dumpName + std::string("_") + boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) + std::string(".dmp");
		//std::cout << strDumpName << std::endl;
		{
			std::string exeFullPath;
			char aFilePath[MAX_PATH + 1] = { 0 };
			::GetModuleFileNameA(NULL, aFilePath, MAX_PATH);
			std::size_t nFindLast = std::string(aFilePath).find_last_of("\\");
			if (nFindLast == std::string::npos)
				nFindLast = std::string(aFilePath).find_last_of("/");
			if (nFindLast != std::string::npos)
				exeFullPath = std::string(aFilePath).substr(0, nFindLast + 1);
			else
				exeFullPath.clear();
			strDumpName = exeFullPath + strDumpName;
		}
		HANDLE hFile = CreateFileA(strDumpName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION exInfo;
			exInfo.ThreadId = ::GetCurrentThreadId();
			exInfo.ExceptionPointers = pExceptionInfo;
			exInfo.ClientPointers = NULL;
			BOOL bOk = ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &exInfo, NULL, NULL);
			CloseHandle(hFile);
		}
		//restart
		//if (Global::GetRestart())
		std::string restart;
		ConfigSingleton::Instance().GetConfig(CONFIG_RESTART_KEY, restart);
		if (restart.compare(CONFIG_TRUE_RESTART_VAL) == 0)
		{
			char aFilePath[MAX_PATH + 1] = { 0 };
			::GetModuleFileNameA(NULL, aFilePath, MAX_PATH);
			std::string strCommandLine = "\"";
			strCommandLine += aFilePath;
			strCommandLine += "\"";
			//
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));
			if (CreateProcessA(NULL,   // No module name (use command line)
				(LPSTR)strCommandLine.c_str(),        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,//0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				(LPSTARTUPINFOA)&si,            // Pointer to STARTUPINFO structure
				&pi)           // Pointer to PROCESS_INFORMATION structure
				)
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		}
		//
		exit(-1);
		return 0;
	}
	//crt check invalid para handler function
	void MyInvalidParameterHandler(const wchar_t* pExpression, const wchar_t* pFunction, const wchar_t* pFile, unsigned int line, uintptr_t pReserved)
	{
		//std::cout << "MyInvalidParameterHandler" << std::endl;
		throw std::exception("MyInvalidParameterHandler exception");
		return;
	}
	//pure virtual function call
	void MyPurecallHandle(void)
	{
		//std::cout << "MyPurecallHandle" << std::endl;
		throw std::exception("MyPurecallHandle exception");
		return;
	}
#else
	//void sig_func(int signo)
	//{
	//	std::string strDumpName = g_dumpName + Time::GetCurDatetimeStr() + std::string(".dmp");
	//	WriteCoreDump(strDumpName.c_str());
	//	exit(-1);
	//}
#endif
	Dump::Dump()
	{
		//std::cout << g_dumpName << std::endl;
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		m_oldFilter = SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		m_oldHandler = _set_invalid_parameter_handler(MyInvalidParameterHandler);
		m_oldPurecallHandler = _set_purecall_handler(MyPurecallHandle);
#else
		//signal(SIGSEGV, &sig_func);
		//signal(SIGFPE, &sig_func);
		struct rlimit rlmt;
		if (getrlimit(RLIMIT_CORE, &rlmt) == -1)
		{
			//std::cout << "getrlimit error" << std::endl;
			SULOKI_STDCOUT_SULOKI("getrlimit error");
		}
		rlmt.rlim_cur = (rlim_t)1024 * 1024 * 500;
		rlmt.rlim_max = (rlim_t)1024 * 1024 * 500;
		if (setrlimit(RLIMIT_CORE, &rlmt) == -1)
		{
			//std::cout << "setrlimit error" << std::endl;
			SULOKI_STDCOUT_SULOKI("setrlimit error");
		}
#endif
	}
	Dump::Dump(std::string strDumpName)
	{
		//g_dumpName = std::string("dump_") + strDumpName;
		g_dumpName = strDumpName;
		//std::cout << g_dumpName << std::endl;
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		m_oldFilter = SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		m_oldHandler = _set_invalid_parameter_handler(MyInvalidParameterHandler);
		m_oldPurecallHandler = _set_purecall_handler(MyPurecallHandle);
#else
		//signal(SIGSEGV, &sig_func);
		//signal(SIGFPE, &sig_func);
		struct rlimit rlmt;
		if (getrlimit(RLIMIT_CORE, &rlmt) == -1)
		{
			//std::cout << "getrlimit error" << std::endl;
			SULOKI_STDCOUT_SULOKI("getrlimit error");
		}
		rlmt.rlim_cur = (rlim_t)1024 * 1024 * 500;
		rlmt.rlim_max = (rlim_t)1024 * 1024 * 500;
		if (setrlimit(RLIMIT_CORE, &rlmt) == -1)
		{
			//std::cout << "setrlimit error" << std::endl;
			SULOKI_STDCOUT_SULOKI("setrlimit error");
		}
#endif
	}
	Dump::~Dump()
	{
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		SetUnhandledExceptionFilter(m_oldFilter);
		_set_invalid_parameter_handler(m_oldHandler);
		_set_purecall_handler(m_oldPurecallHandler);
#else
		//assert(false);
#endif
	}
	Ret Dump::TestDump(void)
	{
		std::string strIn;
		std::cin >> strIn;
		int i = 1 / atoi(strIn.c_str());
		return i;
	}
#endif

	//app statemachine
	AppStateMachine::AppStateMachine() :StateMachine()
	{}
	AppStateMachine::~AppStateMachine()
	{}
	Ret AppStateMachine::Init(void)
	{
#ifdef SULOKI_DUMP_SULOKIDEF
		DumpSingleton::Instance();
#endif
		GlobalStateMachineSingleton::Instance().Init();
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
		//
		LogSingleton::Instance().SetLogLevel(LOG_DEBUG_LEVEL);
		if (ConfigSingleton::Instance().ReadConfig() != SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "ReadConfig error", LOG_ERROR_LEVEL);
			return FAIL;
		}
		std::string strLoglevel;
		ConfigSingleton::Instance().GetConfig(CONFIG_LOGLEVEL_KEY, strLoglevel);
		LogSingleton::Instance().SetLogLevel(strLoglevel);
		if (IdManagerSingleton::Instance().Init() != SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "IdManagerSingleton::Instance().Init() error", LOG_ERROR_LEVEL);
			return FAIL;
		}
		if (EventManagerSingleton::Instance().Init() != SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "EventManagerSingleton::Instance().Init() error", LOG_ERROR_LEVEL);
			return FAIL;
		}
		if (UrcSingleton::Instance().Init() != SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "UrcSingleton::Instance().Init() error", LOG_ERROR_LEVEL);
			return FAIL;
		}
		;
		//
		//SetState(StateMachine::INITED);
		return SUCCESS;
	}
	Ret AppStateMachine::Start(void)
	{
		GlobalStateMachineSingleton::Instance().Start();
		if (IdManagerSingleton::Instance().Start() != SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "IdManagerSingleton::Instance().Start() error", LOG_ERROR_LEVEL);
			return FAIL;
		}
		if (EventManagerSingleton::Instance().Start() != SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "EventManagerSingleton::Instance().Start() error", LOG_ERROR_LEVEL);
			return FAIL;
		}
		if (UrcSingleton::Instance().Start() != SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "UrcSingleton::Instance().Start() error", LOG_ERROR_LEVEL);
			return FAIL;
		}
		;
		//
		//SetState(StateMachine::STARTED);
		return SUCCESS;
	}
	Ret AppStateMachine::Run(void)
	{
		SetState(StateMachine::RUNNING);
		//
		const std::string strMsg = "input quit to exit";
		SULOKI_WRITELOG_SULOKI(Log::STDCOUTTYPE, strMsg, LOG_DEBUG_LEVEL);
		std::string str;
		while (GetState() < StateMachine::STOPPED)
		{
			std::cin >> str;
			if (str.length() > 0 && str.compare("quit") == 0)
			{
				SULOKI_WRITELOG_SULOKI(Log::STDCOUTTYPE, "quit input, i will exit", LOG_FATAL_LEVEL);
				SULOKI_WRITELOG_SULOKI(Log::FILETYPE, "quit input, i will exit", LOG_FATAL_LEVEL);
				break;
			}
		}
		return SUCCESS;
	}
	Ret AppStateMachine::Stop(void)
	{
		SetState(StateMachine::STOPPED);
		//
		;
		UrcSingleton::Instance().Stop();
		EventManagerSingleton::Instance().Stop();
		IdManagerSingleton::Instance().Stop();
		GlobalStateMachineSingleton::Instance().Stop();
		return SUCCESS;
	}
	Ret AppStateMachine::Clear(void)
	{
		SetState(StateMachine::CLEARED);
		//
		;
		UrcSingleton::Instance().Clear();
		UrcSingleton::Deinstance();
		EventManagerSingleton::Instance().Clear();
		EventManagerSingleton::Deinstance();
		IdManagerSingleton::Instance().Clear();
		IdManagerSingleton::Deinstance();
		ConfigSingleton::Deinstance();
		Time::Sleep(Time::TIMEOUT);
		LogSingleton::Deinstance();
		//
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
		//
		GlobalStateMachineSingleton::Instance().Clear();
		GlobalStateMachineSingleton::Deinstance();
#ifdef SULOKI_DUMP_SULOKIDEF
		DumpSingleton::Deinstance();
#endif
		return SUCCESS;
	}
}//end namespace Suloki

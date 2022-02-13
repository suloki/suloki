// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../suloki.h"

//global statemachine
class MyAppStateMachine : public Suloki::AppStateMachine
{
public:
	MyAppStateMachine():Suloki::AppStateMachine()
	{}
	virtual ~MyAppStateMachine()
	{}
	virtual Suloki::Ret Init(void)
	{
		if(Suloki::AppStateMachine::Init() != Suloki::SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "Suloki::AppStateMachine::Init() error", Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		;
		//
		SetState(Suloki::StateMachine::INITED);
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Start(void)
	{
		if (Suloki::AppStateMachine::Start() != Suloki::SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "Suloki::AppStateMachine::Start() error", Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		;
		//
		SetState(Suloki::StateMachine::STARTED);
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Run(void)
	{
		return Suloki::AppStateMachine::Run();
	}
	virtual Suloki::Ret Stop(void)
	{
		SetState(Suloki::StateMachine::STOPPED);
		//
		;
		//
		Suloki::AppStateMachine::Stop();
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Clear(void)
	{
		SetState(Suloki::StateMachine::CLEARED);
		//
		;
		//
		Suloki::AppStateMachine::Clear();
		return Suloki::SUCCESS;
	}
private:
	MyAppStateMachine(MyAppStateMachine& ref) {}
	MyAppStateMachine& operator=(MyAppStateMachine& ref) { return *this; }
};
typedef Suloki::Singleton<MyAppStateMachine> MyAppStateMachineSingleton;
//
class MyClass : public Suloki::Base
{
public:
	MyClass()
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "MyClass()", Suloki::LOG_FATAL_LEVEL);
	}
	virtual ~MyClass() 
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "~MyClass()", Suloki::LOG_FATAL_LEVEL);
	}
	Suloki::Ret Test(void)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "Test()", Suloki::LOG_FATAL_LEVEL);
		return Suloki::SUCCESS;
	}
private:
	MyClass(MyClass& ref) {}
	MyClass& operator=(MyClass& ref) { return *this; }
private:
	volatile Suloki::Int m_state;
};
void Test(void)
{
	SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "test start", Suloki::LOG_FATAL_LEVEL);
	//
	{
		Suloki::DataWrap< Suloki::Int> intWrap(0);
		Suloki::Int t;
		intWrap.Increase(t);
		{
			std::stringstream strStream;
			strStream << t;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_FATAL_LEVEL);
		}
		intWrap.Decrease(t);
		{
			std::stringstream strStream;
			strStream << t;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_FATAL_LEVEL);
		}
		intWrap.Set(100);
		intWrap.Get(t);
		{
			std::stringstream strStream;
			strStream << t;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_FATAL_LEVEL);
		}
	}
	{
		Suloki::SmartPtr<MyClass> myclassSmart(new MyClass());
		myclassSmart->Test();
	}
	{
		Suloki::SmartPtr<Suloki::Data> dataSmart = Suloki::DataStaticCreater::Create("asd", strlen("asd"));
		Suloki::Queue<Suloki::Data> queue;
		queue.Push(dataSmart);
		dataSmart = queue.Pop();
		std::stringstream strStream;
		strStream << "data pop from queue:" << dataSmart->Get();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_FATAL_LEVEL);
	}
	{
		Suloki::SmartPtr<Suloki::Data> keySmart = Suloki::DataStaticCreater::Create("key", strlen("key"));
		Suloki::SmartPtr<Suloki::Data> valSmart = Suloki::DataStaticCreater::Create("val", strlen("val"));
		Suloki::SmartPtr<Suloki::Data> valNewSmart = Suloki::DataStaticCreater::Create("valNew", strlen("valNew"));
		Suloki::Map<64> myMap;
		myMap.Add(keySmart, valSmart);
		keySmart = Suloki::DataStaticCreater::Create("key", strlen("key"));
		myMap.Get(keySmart, valSmart);
		valSmart = Suloki::SmartPtr<Suloki::Data>(NULL);
		myMap.Update(keySmart, valNewSmart, valSmart);
		valSmart = Suloki::SmartPtr<Suloki::Data>(NULL);
		myMap.Del(keySmart, valSmart);
		valSmart = Suloki::SmartPtr<Suloki::Data>(NULL);
		Suloki::Ret ret = myMap.Get(keySmart, valSmart);
		std::stringstream strStream;
		strStream << "get result:" << ret;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_FATAL_LEVEL);
	}
	//
	SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "test end", Suloki::LOG_FATAL_LEVEL);
	/*
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "test exception", Suloki::LOG_DEBUG_LEVEL);
	try {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "throw exception", Suloki::LOG_DEBUG_LEVEL);
		SULOKI_THROW_EXCEPTION_SULOKI("");
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "catch exception", Suloki::LOG_DEBUG_LEVEL);
	}
	//
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "test SmartPtr", Suloki::LOG_DEBUG_LEVEL);
	{
		Suloki::SmartPtr<MyClass<>> myclassSmart(new MyClass<>());
		myclassSmart->Test();
		myclassSmart->SetContext("asd");
		myclassSmart->GetContext();
	}
	{
		std::vector<Suloki::SmartPtr<MyClass<>>> vec1;
		{
			std::vector<Suloki::SmartPtr<MyClass<>>> vec;
			Suloki::SmartPtr<MyClass<>> myclassSmart(new MyClass<>());
			vec.push_back(myclassSmart);
			vec1 = vec;
		}
		Suloki::SmartPtr<MyClass<>> myclassSmart(new MyClass<>());
		vec1.push_back(myclassSmart);
	}
	{
		Suloki::SmartPtr<MyClass<>> myclassSmartOri(new MyClass<>());
		myclassSmartOri.IsNULL();
		//Suloki::SmartPtr<Suloki::Base<>> mybaseSmartOri(myclassSmartOri->Clone());
		//Suloki::SmartPtr<MyClass> myclassSmart = mybaseSmartOri.Upcast<MyClass>();
		//Suloki::SmartPtr<Suloki::Base<>> mybaseSmart = myclassSmart.Downcast();
		Suloki::SmartPtr<Suloki::Base<>> mybaseSmartOri(myclassSmartOri->Clone());
		Suloki::SmartPtr<MyClass<>> myclassSmart = mybaseSmartOri.Upcast<MyClass<>>();
		Suloki::SmartPtr<Suloki::Base<>> mybaseSmart = myclassSmart.Downcast<Suloki::Base<>>();
	}
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "test queue start", Suloki::LOG_DEBUG_LEVEL);
		{
			Suloki::Queue<MyClass<>> queue;
			{
				Suloki::SmartPtr<MyClass<>> myclassSmart(new MyClass<>());
				queue.Push(myclassSmart);
			}
			{
				Suloki::SmartPtr<MyClass<>> myclassSmart(new MyClass<>());
				queue.Push(myclassSmart);
			}
			Suloki::SmartPtr<MyClass<>> myclassSmart = queue.Pop();
			if (!myclassSmart.IsNULL())
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "queue have poped one", Suloki::LOG_DEBUG_LEVEL);
			myclassSmart = queue.Pop();
			if (!myclassSmart.IsNULL())
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "queue have poped one", Suloki::LOG_DEBUG_LEVEL);
			myclassSmart = queue.Pop();
			if(myclassSmart.IsNULL())
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "queue is empty", Suloki::LOG_DEBUG_LEVEL);
		}
		SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, "test queue end", Suloki::LOG_DEBUG_LEVEL);
	}
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "test ThreadPool", Suloki::LOG_DEBUG_LEVEL);
	MyThreadPoolSingleton::Instance().Post(boost::bind(&TestAsync, Suloki::SmartPtr<MyClass<>>(new MyClass<>())));
	*/
	//
	{
		//MyDispatcher dispatcher;
		//boost::property_tree::ptree jsonMsg;
		//Suloki::Message::MakeJsonMsg(jsonMsg);
		//Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
		//std::vector<std::string> context;
		//Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
		//dispatcher.Handle(msgSmart, contextSmart);
		/*
		boost::property_tree::ptree jsonMsg;
		Suloki::Message::MakeJsonMsg(jsonMsg);
		try {
			jsonMsg.put<std::string>("testcontext", "test post message to main service async");
		}
		catch (boost::property_tree::ptree_error pt) {
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json exception", Suloki::LOG_ERROR_LEVEL);
			return;
		}
		catch (...) {
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json exception1", Suloki::LOG_ERROR_LEVEL);
			return;
		}
		Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
		std::vector<std::string> context;
		Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
		Suloki::UrcSingleton::Instance().PostMsgToMainService(msgSmart, contextSmart);
		//
		{
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key";
				Suloki::UrcSingleton::Instance().AddData(strStream.str(), "test val", 0, true);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key1";
				Suloki::UrcSingleton::Instance().AddData(strStream.str(), "test val1", 0, true);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key";
				std::string strVal;
				Suloki::UrcSingleton::Instance().GetData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key1";
				std::string strVal;
				Suloki::UrcSingleton::Instance().GetData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX;
				std::vector<std::string> strValVec;
				Suloki::UrcSingleton::Instance().GetUr(strStream.str(), strValVec);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strValVec[0], Suloki::LOG_DEBUG_LEVEL);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strValVec[1], Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key";
				MyServiceManagerSingleton::Instance().SubUr(Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE, strStream.str());
				std::string strValOri;
				Suloki::UrcSingleton::Instance().UpdateData(strStream.str(), "test val2", strValOri);
				Suloki::Time::Sleep(1000*1);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key";
				std::string strVal;
				Suloki::UrcSingleton::Instance().DelData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key";
				std::string strVal;
				Suloki::UrcSingleton::Instance().GetData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key1";
				std::string strVal;
				Suloki::UrcSingleton::Instance().GetData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX;
				std::vector<std::string> strValVec;
				Suloki::UrcSingleton::Instance().GetUr(strStream.str(), strValVec);
				strValVec.size();
			}
			//
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key1";
				std::string strVal;
				Suloki::UrcSingleton::Instance().DelData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key";
				std::string strVal;
				Suloki::UrcSingleton::Instance().GetData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test key1";
				std::string strVal;
				Suloki::UrcSingleton::Instance().GetData(strStream.str(), strVal);
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strVal, Suloki::LOG_DEBUG_LEVEL);
			}
			{
				std::stringstream strStream;
				strStream << Suloki::URC_BUSINESS_NAMEPREFIX;
				std::vector<std::string> strValVec;
				Suloki::UrcSingleton::Instance().GetUr(strStream.str(), strValVec);
				strValVec.size();
			}
		}
		{
			std::stringstream strStream;
			strStream << Suloki::URC_BUSINESS_NAMEPREFIX << "test obj key";
			Suloki::SmartPtr<MyClass<>> myclassSmart(new MyClass<>());
			Suloki::SmartPtr<Suloki::Base<>> baseSmart = myclassSmart.Downcast<Suloki::Base<>>();
			Suloki::UrcSingleton::Instance().AddSmartPtr(strStream.str(), baseSmart);
			{
				Suloki::SmartPtr<Suloki::Base<>> baseSmart;
				Suloki::UrcSingleton::Instance().GetSmartPtr(strStream.str(), baseSmart);
				myclassSmart.IsNULL();
			}
			{
				MyServiceManagerSingleton::Instance().SubUr(Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE, strStream.str());
				Suloki::SmartPtr<MyClass<>> myclassSmart(new MyClass<>());
				Suloki::SmartPtr<Suloki::Base<>> baseSmart = myclassSmart.Downcast<Suloki::Base<>>();
				Suloki::SmartPtr<Suloki::Base<>> baseSmartOri;
				Suloki::UrcSingleton::Instance().UpdateSmart(strStream.str(), baseSmart, baseSmartOri);
				Suloki::Time::Sleep(1000 * 1);
			}
			{
				Suloki::SmartPtr<Suloki::Base<>> baseSmart;
				Suloki::UrcSingleton::Instance().DelSmartPtr(strStream.str(), baseSmart);
				myclassSmart.IsNULL();
			}
			{
				Suloki::SmartPtr<Suloki::Base<>> baseSmart;
				Suloki::UrcSingleton::Instance().GetSmartPtr(strStream.str(), baseSmart);
				myclassSmart.IsNULL();
			}
		}
		*/
	}
}
//
int main()
{
	if (MyAppStateMachineSingleton::Instance().Init() != Suloki::SUCCESS)
	{
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyAppStateMachineSingleton::Instance().Init() error", Suloki::LOG_ERROR_LEVEL);
		return -1;
	}
	if(MyAppStateMachineSingleton::Instance().Start() != Suloki::SUCCESS)
	{
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyAppStateMachineSingleton::Instance().Start() error", Suloki::LOG_ERROR_LEVEL);
		return -1;
	}
	//
	Test();
	//
	MyAppStateMachineSingleton::Instance().Run();
	//
	MyAppStateMachineSingleton::Instance().Stop();
	MyAppStateMachineSingleton::Instance().Clear();
	//
	MyAppStateMachineSingleton::Deinstance();
	//
    return 0;
}


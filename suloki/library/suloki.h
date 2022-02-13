#ifndef _SULOKI_H_
#define _SULOKI_H_

//std
#include <stdio.h>
#include <iostream>
#include <setjmp.h>
#include <list>
#include <memory> 
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <math.h>
#include <sstream>
#include <time.h>
#include <exception>
#include <ctime>

//loki
#define LOKI_USE_REFERENCE
#include "TypeManip.h"
#include "./Reference/functor.h"

//boost
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/thread/mutex.hpp>
//#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
//#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <boost/asio/steady_timer.hpp>
#include <boost/filesystem.hpp>
//#include <boost/uuid/uuid.hpp>
//#include <boost/uuid/uuid_generators.hpp>
//#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "sulokidef.h"

#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <intrin.h>
#include  <io.h>
//about dump
#ifdef SULOKI_DUMP_SULOKIDEF
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <setjmp.h>
#include <sys/resource.h>
#include <sys/time.h>
#endif

//about global define used with c and c++
#ifdef __cplusplus
extern "C" {
#endif
#include "sulokidef.h"
#ifdef __cplusplus
}
#endif

#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
#include "leveldb/db.h"
#endif

namespace Suloki
{
	//base simple type
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
#ifdef SULOKI_64_PLATFORM_SULOKIDEF
	typedef long long  Int;
	typedef unsigned long long Uint;
	typedef double Float;
#else
	typedef long  Int;
	typedef unsigned long Uint;
	typedef float  Float;
#endif
	typedef long long  Int64;
	typedef long  Int32;
	typedef short  Int16;
	typedef char   Int8;
	typedef unsigned long long Uint64;
	typedef unsigned long Uint32;
	typedef unsigned short Uint16;
	typedef unsigned char  Uint8;
	typedef double Float64;
	typedef float  Float32;
#else
#ifdef SULOKI_64_PLATFORM_SULOKIDEF
	typedef int64_t  Int;
	typedef uint64_t Uint;
	typedef double   Float;
#else
	typedef int32_t  Int;
	typedef uint32_t Uint;
	typedef float    Float;
#endif
	typedef int64_t  Int64;
	typedef int32_t  Int32;
	typedef int16_t  Int16;
	typedef int8_t   Int8;
	typedef uint64_t Uint64;
	typedef uint32_t Uint32;
	typedef uint16_t Uint16;
	typedef uint8_t  Uint8;
	typedef double   Float64;
	typedef float    Float32;
#endif
	//return code
	typedef Int Ret;
	//error code
	const Ret SUCCESS = 0;
	const Ret FAIL = 1;
	const Ret INVALIDPARA_ERRCODE = 2;
	const Ret INVALIDSTATE_ERRCODE = 3;
	const Ret NOMEMORY_ERRCODE = 4;
	const Ret TIMEOUT_ERRCODE = 5;
	//
	const Ret PARSEJSON_ERRCODE = 6;
	const Ret INVALIDMSG_ERRCODE = 7;
	//
	inline std::string GetErrStr(Ret ret);
	inline std::string GetCodeErr(Ret ret)
	{
		std::stringstream strStream;
		strStream << ret;
		return strStream.str();
	}
	//std::cout
#define SULOKI_STDCOUT_SULOKI(str) std::cout << str << ". code info:file=" << __FILE__ << " line=" << __LINE__ << " function=" << __FUNCTION__ << std::endl;
	//exception
#define SULOKI_THROW_EXCEPTION_SULOKI(strError) {std::stringstream strStream;strStream << strError << ".code info:file=" << __FILE__ << " line=" << __LINE__ << " function=" << __FUNCTION__ << std::endl;throw Suloki::Exception(strStream.str());}
	struct Exception : public std::exception
	{
		enum { LENGTH = 62 };
	public:
		explicit Exception(std::string strError)
		{
			Int length = strError.length();
			if (length > LENGTH)
			{
				length = LENGTH;
			}
			memcpy(m_strErr, strError.c_str(), length);
			m_strErr[length] = '\0';
		}
		~Exception() {}
		const char* what() const throw()
		{
			return m_strErr;
		}
	private:
		char m_strErr[LENGTH + 2];
	};
	//interface layer
	//StateMachine inetrface, no lock in one thread 
	class StateMachine
	{
	public:
		enum { ZERO = 0, INITED = 1, STARTED = 2, RUNNING = 3, STOPPED = 4, CLEARED = 5 };
		StateMachine():m_state(ZERO){}
		virtual ~StateMachine() {}
		virtual Ret Init(void) { SetState(INITED); return SUCCESS; }
		virtual Ret Start(void) { SetState(STARTED); return SUCCESS; }
		virtual Ret Run(void) { SetState(RUNNING); return SUCCESS; }
		virtual Ret Stop(void) { SetState(STOPPED); return SUCCESS; }
		virtual Ret Clear(void) { SetState(CLEARED); return SUCCESS; }
	public:
		virtual Int GetState(void)
		{
			return m_state;
		}
	protected:
		virtual void SetState(Int state)
		{
			m_state = state;
		}
	private:
		StateMachine(StateMachine& ref) {}
		StateMachine& operator=(StateMachine& ref) { return *this; }
	private:
		volatile Int m_state;
	};
	//end interface layer
	//os layer
	//atom
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
#define SULOKI_CAS_SULOKI( destination, exchange, comparand ) ( InterlockedCompareExchangePointer( (PVOID volatile *)destination, (PVOID)exchange, (PVOID)comparand ) != comparand ? true : false )
#else
//#define CAS( destination, exchange, comparand ) ( __sync_val_compare_and_swap( destination, comparand, exchange ) != comparand ? true : false )
#define SULOKI_CAS_SULOKI( destination, exchange, comparand ) ( !__sync_bool_compare_and_swap( destination, comparand, exchange ) )
#endif
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
#ifdef SULOKI_64_PLATFORM_SULOKIDEF
#define SULOKI_INCREMENT_SULOKI(pInt) InterlockedIncrement64(pInt)
#define SULOKI_DECREMENT_SULOKI(pInt) InterlockedDecrement64(pInt)
#endif
#else
#define SULOKI_INCREMENT_SULOKI(pInt) __sync_fetch_and_add(pInt, 1)+1
#define SULOKI_DECREMENT_SULOKI(pInt) __sync_fetch_and_sub(pInt, 1)-1
#endif
	//end os layer
	//boost layer
	//read/write lock
	typedef boost::shared_mutex RwLockPolicy;
	typedef boost::unique_lock<RwLockPolicy>   WriteLock;
	typedef boost::shared_lock<RwLockPolicy>   ReadLock;
	//thread pool
	class ThreadPool : public StateMachine
	{
		enum { MAXNUM_THREAD = 64 };
	public:
		ThreadPool();
		explicit ThreadPool(Int threadNum);
		virtual ~ThreadPool();
		//StateMachine inetrface
		virtual Ret Init(void);
		virtual Ret Start(void);
		virtual Ret Run(void);
		virtual Ret Stop(void);
		virtual Ret Clear(void);
		//end StateMachine inetrface
		Ret Init(Int threadNum);
		//
		template<typename F>
		Ret Post(F func)
		{
			//if (!m_bInited)
			//{
			//	return FAIL;
			//}
			m_ioService.post(func);
			return SUCCESS;
		}
		template<typename F, typename...Args>
		Ret Post(F &&f, Args&&...args)
		{
			Post(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
			return SUCCESS;
		}
	private:
		ThreadPool(ThreadPool& ref) :m_worker(m_ioService) {}
		ThreadPool& operator=(ThreadPool& ref) { return *this; }
	private:
		boost::asio::io_service m_ioService;
		boost::asio::io_service::work m_worker;
		boost::thread_group m_threads;
		//
		volatile Int m_threadNum;
	};
	//end boost layer
	//singleton
	template < typename T, typename LockPolicy = RwLockPolicy>
	class Singleton
	{
		//typedef boost::unique_lock<LockPolicy>   WriteLock;
		//typedef boost::shared_lock<LockPolicy>   ReadLock;
	public:
		inline static T& Instance(void) throw()
		{
			if (m_pInstance == NULL)
			{
				WriteLock lock(m_rwLock);
				if (m_pInstance == NULL)
				{
					m_pInstance = new T();
					if (m_pInstance == NULL)
					{
						SULOKI_THROW_EXCEPTION_SULOKI("new exception");
					}
				}
			}
			//ReadLock lock(m_rwLock);
			return *m_pInstance;
		}
		inline static void Deinstance(void)
		{
			if (m_pInstance != NULL)
			{
				WriteLock lock(m_rwLock);
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}
	private:
		Singleton() {}
		~Singleton() {}
		Singleton(Singleton& ref) {}
		Singleton& operator=(Singleton& ref) { return *this; }
	private:
		static LockPolicy m_rwLock;
		static T* m_pInstance;
	};
	template < typename T, typename LockPolicy >
	LockPolicy Singleton< T, LockPolicy >::m_rwLock;
	template < typename T, typename LockPolicy >
	T* Singleton< T, LockPolicy >::m_pInstance = NULL;
	//global statemachine, used inside
	class AppStateMachine;
	class GlobalStateMachine : public StateMachine
	{
	public:
		GlobalStateMachine():StateMachine() {}
		virtual ~GlobalStateMachine() {}
		virtual Ret Init(void) { return SUCCESS; }
		virtual Ret Start(void) { return SUCCESS; }
		virtual Ret Run(void) { return SUCCESS; }
		virtual Ret Stop(void) { return SUCCESS; }
		virtual Ret Clear(void) { return SUCCESS; }
		//
	private:
		GlobalStateMachine(GlobalStateMachine& ref) {}
		GlobalStateMachine& operator=(GlobalStateMachine& ref) { return *this; }
	private:
		friend class AppStateMachine;
	};
	typedef Singleton<GlobalStateMachine> GlobalStateMachineSingleton;
	//time management
	class Time
	{
	public:
		enum { TIMEOUT = 256, HALF_TIMEOUT = 128};
		static void Sleep(Int mSec);
	public:
		Time()
		{}
		virtual ~Time()
		{}
	private:
		Time(Time& ref) {}
		Time& operator=(Time& ref) { return *this; }
	private:
	};
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#define SULOKI_MAX_THREADNUM_ALLOCATER_SULOKI 128//256//64//128//256
#else
#endif
	//atom data wrap
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
	//end atom data wrap
	//SmartPtr, all ptr must in SmartPtr
	//T must a child of Base
	class Base;
	template <typename T> class Queue;
	template <Int NUMSLOT, Int MAXKEYLEN, Int MAXVALLEN> class Map;
	class EventManager;
	class Urc;
	template < typename T>
	class SmartPtr
	{
	public:
		inline SmartPtr() :m_ptr(NULL)
		{}
		inline explicit SmartPtr(T* ptr) :m_ptr(NULL)
		{
			m_ptr = ptr;
			if (m_ptr != NULL)
			{
				static_cast<Base*>(m_ptr)->IncreaseCountPolicy();
			}
		}
		inline SmartPtr(const SmartPtr& ref) :m_ptr(NULL)
		{
			m_ptr = ref.m_ptr;
			if (m_ptr != NULL)
			{
				static_cast<Base*>(m_ptr)->IncreaseCountPolicy();
			}
		}
		inline SmartPtr& operator=(const SmartPtr& ref)
		{
			if (this != &ref && m_ptr != ref.m_ptr)
			{
				if (m_ptr != NULL)
				{
					if (static_cast<Base*>(m_ptr)->DecreaseCountPolicy() == 0)
					{
						delete m_ptr;
						m_ptr = NULL;
					}
				}
				m_ptr = ref.m_ptr;
				if (m_ptr != NULL)
				{
					static_cast<Base*>(m_ptr)->IncreaseCountPolicy();
				}
			}
			return *this;
		}
		virtual ~SmartPtr()
		{
			if (m_ptr != NULL)
			{
				if (static_cast<Base*>(m_ptr)->DecreaseCountPolicy() == 0)
				{
					delete m_ptr;
					m_ptr = NULL;
				}
			}
		}
		inline T* operator->() const
		{
			if (m_ptr == NULL)
			{
				SULOKI_THROW_EXCEPTION_SULOKI("ptr is NULL exception");
			}
			return m_ptr;
		}
		inline bool IsNULL(void)
		{
			if (m_ptr == NULL)
			{
				return true;
			}
			return false;
		}
		template < typename V>
		SmartPtr<V> Downcast(void)
		{
			V* pTr = static_cast<V*>(m_ptr);
			return SmartPtr<V>(pTr);
		}
		template < typename V>
		SmartPtr<V> Upcast(void)
		{
			V* pTr = dynamic_cast<V*>(m_ptr);
			return SmartPtr<V>(pTr);
		}
		//????? temp,must del future
		bool IsOne(SmartPtr<T> tSmart)
		{
			if (m_ptr == tSmart.m_ptr)
				return true;
			return false;
		}
	protected:
	//public:
		//????? temp,must del future
		void Capture(T* pTr)
		{
			if (m_ptr != NULL)
			{
				if (static_cast<Base*>(m_ptr)->DecreaseCountPolicy() == 0)
				{
					delete m_ptr;
					m_ptr = NULL;
				}
			}
			m_ptr = pTr;
		}
		//????? temp,must del future
		T* Release(void)
		{
			T* pTr = m_ptr;
			m_ptr = NULL;
			return pTr;
		}
		//????? temp,must del future
		T* get(void)
		{
			return m_ptr;
		}
	private:
		T* m_ptr;
		//
		template < typename T>
		friend class Queue;
		template <Int NUMSLOT, Int MAXKEYLEN, Int MAXVALLEN> 
		friend class Map;
		friend class EventManager;
		friend class Urc;
	};
	//base class of all class
//#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
//#else
	typedef struct _MultyThreadReferenceCountPolicy
	{
		inline void InitCountPolicy(void) { m_referenceCount = 0; }
		inline void IncreaseCountPolicy(void) { SULOKI_INCREMENT_SULOKI(&m_referenceCount); }
		inline Int DecreaseCountPolicy(void) { return SULOKI_DECREMENT_SULOKI(&m_referenceCount); }
		volatile Int m_referenceCount;
	}MultyThreadReferenceCountPolicy;
	class Base : public MultyThreadReferenceCountPolicy
	{
	public:
		Base()
		{
			InitCountPolicy();
		}
		virtual ~Base()
		{
		}
	public:
		virtual SmartPtr< Base > Clone(void)
		{
			return SmartPtr< Base >(NULL);
		}
	protected:
		inline void IncreaseCountPolicy(void)
		{
			MultyThreadReferenceCountPolicy::IncreaseCountPolicy();
		}
		inline Int DecreaseCountPolicy(void)
		{
			return MultyThreadReferenceCountPolicy::DecreaseCountPolicy();
		}
	private:
		inline void InitCountPolicy(void)
		{
			InitCountPolicyIn(Loki::Type2Type< MultyThreadReferenceCountPolicy >());
		}
		inline void InitCountPolicyIn(Loki::Type2Type< MultyThreadReferenceCountPolicy >)
		{
			MultyThreadReferenceCountPolicy::InitCountPolicy();
		}
	private:
		Base(Base& ref) {}
		Base& operator=(Base& ref) { return *this; }
	private:
		template < typename T>
		friend class SmartPtr;
	};
//#endif	//end base
	//data structure
	//data
	template <Int NUMSLOT, Int MAXKEYLEN, Int MAXVALLEN> //, typename TdbPolicy = NoDbPolicy>
	class Map;
	class Data : public Base
	{
	public:
		explicit Data(Int lDataLen) {}
		Data(Int8* pData, Int lDataLen) {}
		virtual ~Data() {}
		virtual const Int8* Get(Int* pSize = NULL) { return NULL; }
	protected:
		virtual void ResetSize(void) { return; }
		virtual Ret Append(Int8* pData, Int lDataLen) { return FAIL; }
	private:
		Data(Data& ref) {}
		Data& operator=(Data& ref) { return *this; }

		template <Int NUMSLOT, Int MAXKEYLEN, Int MAXVALLEN> //, typename TdbPolicy = NoDbPolicy>
		friend class Map;
	};
	const Int SULOKI_MAXLEN_DATASTATICCREATER_SULOKI = 1024 * 1024 * 1024;
	class Urc;
	class DataStaticCreater
	{
		template <Int SIZE>
		class DataStatic : public Data
		{
		protected:
			static SmartPtr< Data > Create(Int8* pData, Int lDataLen)
			{
				if (!(pData != NULL && lDataLen > 0 && lDataLen <= SIZE))
				{
					return SmartPtr< Data >(NULL);
				}
				SmartPtr< Data > dataSmart(new DataStatic<SIZE>(pData, lDataLen));
				return dataSmart;
			}
			static SmartPtr< Data > Create(Int lDataLen)
			{
				if (!(lDataLen > 0 && lDataLen <= SIZE))
				{
					return SmartPtr< Data >(NULL);
				}
				SmartPtr< Data > dataSmart(new DataStatic<SIZE>(lDataLen));
				return dataSmart;
			}
			explicit DataStatic(Int lDataLen) :Data(lDataLen), m_size(lDataLen)//m_size(0)
			{
				memset(m_data, 0, lDataLen);
				m_data[m_size] = '\0';
			}
			DataStatic(Int8* pData, Int lDataLen) :Data(pData, lDataLen), m_size(lDataLen)
			{
				memcpy(m_data, pData, lDataLen);
				m_data[lDataLen] = '\0';
			}
		public:
			virtual ~DataStatic()
			{
			}
			virtual const Int8* Get(Int* pSize = NULL)
			{
				if (pSize != NULL)
				{
					*pSize = m_size;
				}
				return &m_data[0];
			}
		protected:
			virtual void ResetSize(void)
			{ 
				m_size = 0;
			}
			virtual Ret Append(Int8* pData, Int lDataLen) 
			{
				if (!(pData != NULL && lDataLen >= 0 && m_size + lDataLen <= SIZE))
					return FAIL;
				memcpy(m_data+ m_size, pData, lDataLen);
				m_size += lDataLen;
				return SUCCESS; 
			}
		private:
			DataStatic(DataStatic& ref) {}
			DataStatic& operator=(DataStatic& ref) { return *this; }
		private:
			Int m_size;
			Int8 m_data[SIZE + 2];
			//
			friend class DataStaticCreater;
		};
	public:
		static SmartPtr< Data > Create(Int8* pData, Int lDataLen)
		{
			if (!(pData != NULL && lDataLen > 0 && lDataLen <= SULOKI_MAXLEN_DATASTATICCREATER_SULOKI))
			{
				return  SmartPtr< Data >(NULL);
			}
			const Int MAXNUM = 31;
			static SmartPtr< Data >(*func[])(Int8* pData, Int lDataLen) = {
				DataStatic<1>::Create, DataStatic<2>::Create, DataStatic<4>::Create, DataStatic<8>::Create,
				DataStatic<16>::Create, DataStatic<32>::Create, DataStatic<64>::Create, DataStatic<128>::Create,
				DataStatic<256>::Create, DataStatic<512>::Create, DataStatic<1024>::Create, DataStatic<2048>::Create,
				DataStatic<4096>::Create, DataStatic<8192>::Create, DataStatic<16384>::Create, DataStatic<32768>::Create,
				DataStatic<32768 * 2>::Create, DataStatic<32768 * 4>::Create, DataStatic<32768 * 8>::Create, DataStatic<32768 * 16>::Create,
				DataStatic<32768 * 32>::Create, DataStatic<32768 * 64>::Create, DataStatic<32768 * 128>::Create, DataStatic<32768 * 256>::Create,//1024*1024*8
				DataStatic<1024 * 1024 * 16>::Create, DataStatic<1024 * 1024 * 32>::Create, DataStatic<1024 * 1024 * 64>::Create, DataStatic<1024 * 1024 * 128>::Create,//128M
				DataStatic<1024 * 1024 * 256>::Create, DataStatic<1024 * 1024 * 512>::Create, DataStatic<1024 * 1024 * 1024>::Create,//1G
			};
			//if (lDataLen <= 0)
			//	lDataLen = 1;
			double grade = log((double)lDataLen) / log((double)2);
			Int pos = ceil(grade);
			if (pos >= MAXNUM)
			{
				return SmartPtr< Data >(NULL);
			}
			return func[pos](pData, lDataLen);
		}
	protected:
		static SmartPtr< Data > Create(Int lDataLen)
		{
			if (!(lDataLen > 0))
			{
				return  SmartPtr< Data >(NULL);
			}
			const Int MAXNUM = 31;
			static SmartPtr< Data >(*func[])(Int lDataLen) = {
				DataStatic<1>::Create, DataStatic<2>::Create, DataStatic<4>::Create, DataStatic<8>::Create,
				DataStatic<16>::Create, DataStatic<32>::Create, DataStatic<64>::Create, DataStatic<128>::Create,
				DataStatic<256>::Create, DataStatic<512>::Create, DataStatic<1024>::Create, DataStatic<2048>::Create,
				DataStatic<4096>::Create, DataStatic<8192>::Create, DataStatic<16384>::Create, DataStatic<32768>::Create,
				DataStatic<32768 * 2>::Create, DataStatic<32768 * 4>::Create, DataStatic<32768 * 8>::Create, DataStatic<32768 * 16>::Create,
				DataStatic<32768 * 32>::Create, DataStatic<32768 * 64>::Create, DataStatic<32768 * 128>::Create, DataStatic<32768 * 256>::Create,//1024*1024*8
				DataStatic<1024 * 1024 * 16>::Create, DataStatic<1024 * 1024 * 32>::Create, DataStatic<1024 * 1024 * 64>::Create, DataStatic<1024 * 1024 * 128>::Create,//128M
				DataStatic<1024 * 1024 * 256>::Create, DataStatic<1024 * 1024 * 512>::Create, DataStatic<1024 * 1024 * 1024>::Create,	//1G
			};
			//if (lDataLen <= 0)
			//	lDataLen = 1;
			double grade = log((double)lDataLen) / log((double)2);
			Int pos = ceil(grade);
			if (pos >= MAXNUM)
			{
				return SmartPtr< Data >(NULL);
			}
			return func[pos](lDataLen);
		}
	public:
		static SmartPtr< Data > Append(SmartPtr< Data > dataOriSmart, Int8* pData, Int lDataLen)
		{
			if (!(pData != NULL && lDataLen > 0))
			{
				return  SmartPtr< Data >(NULL);
			}
			SmartPtr< Data > dataNewSmart;
			if (dataOriSmart.IsNULL())
			{
				dataNewSmart = Create(pData, lDataLen);
			}
			else
			{
				Int dataOriLen = 0;
				const Int8* pDataOri = dataOriSmart->Get(&dataOriLen);
				dataNewSmart = Create(dataOriLen + lDataLen);
				if (dataNewSmart.IsNULL())
				{
					return dataNewSmart;
				}
				Int8* pDataNew = (Int8*)dataNewSmart->Get();
				memcpy(pDataNew, pDataOri, dataOriLen);
				memcpy(pDataNew + dataOriLen, pData, lDataLen);
			}
			return dataNewSmart;
		}
		static Ret Read(SmartPtr< Data > dataOriSmart, Int& position, void* pBuffer, Int bufLength, bool bFirst = false)
		{
			if (!(!dataOriSmart.IsNULL() && pBuffer != NULL && bufLength > 0))
			{
				return  FAIL;
			}
			if (bFirst)
			{
				position = 0;
			}
			Int dataOriLen = 0;
			const Int8* pDataOri = dataOriSmart->Get(&dataOriLen);
			if (position + bufLength > dataOriLen)
			{
				return  FAIL;
			}
			memcpy(pBuffer, pDataOri + position, bufLength);
			position += bufLength;
			return SUCCESS;
		}
		//
		template <Int NUMSLOT, Int MAXKEYLEN, Int MAXVALLEN> //, typename TdbPolicy = NoDbPolicy>
		friend class Map;
		friend class Urc;
	};
	//queue,T must a child of Base
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
	template <typename T>
	class Queue : public Base
	{
	public:
		Queue() throw()
		{
			m_pQueue = new boost::lockfree::queue<T*, boost::lockfree::fixed_sized<false> >(0);
			if (m_pQueue == NULL)
			{
				SULOKI_THROW_EXCEPTION_SULOKI("new boost::lockfree::queue exception");
			}
			//if (m_pQueue->is_lock_free())
			//{
			//	std::cout << "" << std::endl;
			//}
			m_size = 0;
		}
		virtual ~Queue()
		{
			Clear();
			if (m_pQueue != NULL)
			{
				delete m_pQueue;
				m_pQueue = NULL;
			}
		}
		inline Ret Push(SmartPtr<T> tSmart)// throw()
		{
			if (tSmart.get() == NULL)
			{
				return INVALIDPARA_ERRCODE;
			}
			T* pItem = tSmart.get();
			SULOKI_INCREMENT_SULOKI(&m_size);
			bool bRes = m_pQueue->push(pItem);
			if (bRes)
			{
				tSmart.Release();
				return SUCCESS;
			}
			SULOKI_DECREMENT_SULOKI(&m_size);
			return FAIL;
		}
		inline SmartPtr<T> Pop(void)// throw()
		{
			T* pItem = NULL;
			bool bRes = m_pQueue->pop(pItem);
			if (bRes && pItem != NULL)
			{
				SULOKI_DECREMENT_SULOKI(&m_size);
				SmartPtr<T> tSmart;
				tSmart.Capture(pItem);
				return tSmart;
			}
			return SmartPtr<T>(NULL);
		}
		inline void Clear(void)
		{
			while (true)
			{
				SmartPtr<T> tSmart = Pop();
				if (tSmart.get() == NULL)
				{
					break;
				}
			}
		}
		inline Int GetSize(void)
		{
			return m_size;
		}
	private:
		Queue(Queue& ref) {}
		Queue& operator=(Queue& ref) { return *this; }
	private:
		boost::lockfree::queue<T*, boost::lockfree::fixed_sized<false> > *m_pQueue;
		volatile Int m_size;
	};
#else
#endif
	//up use SULOKI_STDCOUT_SULOKI,down use log
	//log
	//????? log is old code,check code carefully
#define SULOKI_WRITELOG_SULOKI(streamType, strLog, logLevel) Suloki::LogSingleton::Instance().WriteLog(streamType, strLog, logLevel, __FILE__, __LINE__, __FUNCTION__);
	const std::string LOG_STRTRACE_LEVEL = "trace";
	const std::string LOG_STRDEBUG_LEVEL = "debug";
	const std::string LOG_STRINFO_LEVEL = "info";
	const std::string LOG_STRWARN_LEVEL = "warning";
	const std::string LOG_STRERROR_LEVEL = "error";
	const std::string LOG_STRFATAL_LEVEL = "fatal";
	const Int LOG_TRACE_LEVEL = 0;
	const Int LOG_DEBUG_LEVEL = 1;
	const Int LOG_INFO_LEVEL = 2;
	const Int LOG_WARN_LEVEL = 3;
	const Int LOG_ERROR_LEVEL = 4;
	const Int LOG_FATAL_LEVEL = 5;
	class Log : public Base
	{
	public:
		enum OutputStream { STDCOUTTYPE = 0, FILETYPE = 1 };
	public:
		Log();
		virtual ~Log();
		void SetLogLevel(std::string logLevel);
		void SetLogLevel(Int logLevel);
		Ret WriteLog(OutputStream streamType, std::string strLog, Int iLevel, std::string file, Int line, std::string function);
	protected:
		std::string TurnItoS(Int iLevel);
	protected:
		void RunFunction(void);
		void DelFunction(void);
	private:
		Log(Log& ref) {}
		Log& operator=(Log& ref) { return *this; }
	private:
		class LogItem : public Base
		{
		public:
			LogItem(std::string logStr, OutputStream stream)
			{
				m_logStr = logStr;
				m_stream = stream;
			}
			std::string m_logStr;
			OutputStream m_stream;
		};
		std::auto_ptr<boost::thread> m_threadRunnedSmart;
		std::auto_ptr<boost::thread> m_threadDeledSmart;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
		boost::lockfree::queue<LogItem*, boost::lockfree::fixed_sized<false> > m_queue;
#else
#endif
		//std::ofstream m_file;
		volatile Int m_iLevel;
		volatile bool m_bStop;

	};
	typedef Singleton<Log> LogSingleton;
	//config
	//????? config is old code,check code carefully
	const std::string CONFIGNAME = "sulokiconfig.txt";
	//key
	const std::string CONFIG_LOGLEVEL_KEY = "loglevel";//must exist
	const std::string CONFIG_RESTART_KEY = "restart";//must exist
	const std::string CONFIG_PSTORAGE_KEY = "pstorage";//must exist
	const std::string CONFIG_GROUPNAME_KEY = "groupname";//must exist
	const std::string CONFIG_SERVICENAME_KEY = "servicename";//must exist
	const std::string CONFIG_SERVICETYPE_KEY = "servicetype";//must exist
	const std::string CONFIG_SERVICETHREADS_KEY = "servicethreads";//must exist
	const std::string CONFIG_UDPSERVERIP_KEY = "udpserverip";//must exist
	const std::string CONFIG_UDPSERVERPORT_KEY = "udpserverport";//must exist
	const std::string CONFIG_REMOTEUDPSERVERIP_KEY = "remoteudpserverip";//must exist
	const std::string CONFIG_REMOTEUDPSERVERPORT_KEY = "remoteudpserverport";//must exist
	const std::string CONFIG_WSSERVERIP_KEY = "wsserverip";//must exist
	const std::string CONFIG_WSSERVERPORT_KEY = "wsserverport";//must exist
	const std::string CONFIG_WSSERVERIPREMOTED_KEY = "wsserveripremoted";//must exist
	const std::string CONFIG_WSSERVERPORTREMOTED_KEY = "wsserverportremoted";//must exist
																			 //val
	const std::string CONFIG_CPLUSPLUS_SERVICETYPE_VAL = "c++";
	const std::string CONFIG_JAVASCRIPT_SERVICETYPE_VAL = "js";
	const std::string CONFIG_SERVICETHREADS_VAL = "0";
	const std::string CONFIG_TRUE_RESTART_VAL = "true";
	const std::string CONFIG_FALSE_RESTART_VAL = "false";
	//const std::string CONFIG_USER_KEY = "user";
	//const std::string CONFIG_PASS_KEY = "pass";
	class Config : public Base
	{
		typedef RwLockPolicy LockPolicy;
		typedef boost::unique_lock<LockPolicy>   WriteLock;
		typedef boost::shared_lock<LockPolicy>   ReadLock;
	public:
		Config();
		virtual ~Config()
		{}
		virtual Ret ReadConfig(void);
		Ret SetConfig(std::string key, std::string val);
		Ret GetConfig(std::string key, std::string& val);
		//template< typename T >
		//Ret GetConfig(std::string key, std::vector<T>& tVector)
		//{
		//	tVector.clear();
		//	ReadLock lock(m_rwLock);
		//	tVector = m_doduleVector;
		//	return SUCCESS;
		//}
	protected:
		void SetDefault(void);
	private:
		Config(Config& ref) {}
		Config& operator=(Config& ref) { return *this; }
	protected:
		//?????
		//LockPolicy m_rwLock;
		std::map<std::string, std::string> m_configMap;
	};
	typedef Singleton<Config> ConfigSingleton;
	//message and context
	//????? is old code,check code carefully
	class Context : public Base
	{
	public:
		explicit Context(std::vector<std::string> context)
		{
			m_context = context;
		}
		virtual ~Context()
		{}
		virtual SmartPtr< Base > Clone(void)
		{
			return SmartPtr< Context >(new Context(m_context)).Downcast<Base>();
		}
		std::vector<std::string> Get(void)
		{
			return m_context;
		}
	private:
		Context(Context& ref) {}
		Context& operator=(Context& ref)
		{
			if (this != &ref)
			{
			}
			return *this;
		}
	protected:
		std::vector<std::string> m_context;
	};
	//????? is old code,check code carefully
	class Message : public Base
	{
	public:
		Message(std::string strMsg, SmartPtr<Context> contextSmart = SmartPtr<Context>(NULL))
		{
			m_strMsg = strMsg;
			m_jsonMsg.clear();
			try {
				std::stringstream strStream;
				strStream << strMsg;
				boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, m_jsonMsg);
			}
			catch (boost::property_tree::ptree_error pt) {
			}
			catch (...) {
			}
		}
		virtual ~Message()
		{}
		virtual SmartPtr< Base > Clone(void)
		{
			if (m_strMsg.length() == 0)
			{
				return SmartPtr< Message >(new Message(m_strMsg, m_contextSmart)).Downcast<Base>();
			}
			return SmartPtr< Message >(new Message(m_jsonMsg, m_contextSmart)).Downcast<Base>();
		}
		std::string Get(void)
		{
			return m_strMsg;
		}
		void SetContext(SmartPtr<Context> contextSmart)
		{
			m_contextSmart = contextSmart;
		}
		SmartPtr<Context> GetContext(void)
		{
			return m_contextSmart;
		}
	public:
		Message(boost::property_tree::ptree jsonMsg, SmartPtr<Context> contextSmart = SmartPtr<Context>(NULL))
		{
			m_jsonMsg = jsonMsg;
			std::string strRes;
			try {
				std::stringstream strStream;
				boost::property_tree::write_json(strStream, jsonMsg);
				m_strMsg = strStream.str();
			}
			catch (boost::property_tree::ptree_error pt) {
			}
			catch (...) {
			}
		}
		std::string GetField(std::string key)
		{
			std::string field;
			try {
				field = m_jsonMsg.get<std::string>(key);
			}
			catch (boost::property_tree::ptree_error pt) {
				//std::stringstream strStream;
				//strStream << "get json field excepion.key:" << key;
				//SULOKI_WRITELOG_SULOKI(Log::FILETYPE, strStream.str(), LOG_ERROR_LEVEL);
				return std::string("");
			}
			catch (Exception e) {
				//std::stringstream strStream;
				//strStream << "get json field excepion1.key:" << key;
				//SULOKI_WRITELOG_SULOKI(Log::FILETYPE, strStream.str(), LOG_ERROR_LEVEL);
				return std::string("");
			}
			return field;
		}
		boost::property_tree::ptree GetJsonobj(void)
		{
			return m_jsonMsg;
		}
		static Ret MakeJsonMsg(boost::property_tree::ptree& jsonMsg);
		static Ret CheckJsonMsg(boost::property_tree::ptree& jsonMsg);
	private:
		Message(Message& ref) {}
		Message& operator=(Message& ref)
		{
			if (this != &ref) {}
			return *this;
		}
	protected:
		std::string m_strMsg;
		boost::property_tree::ptree m_jsonMsg;
		SmartPtr<Context> m_contextSmart;
	};
	//double dispatch
	//????? is old code,check code carefully
	template<typename Tprotocol = Message, typename Tpara = Context, typename LockPolicy = RwLockPolicy>
	class Dispatcher : public Base
	{
		//typedef boost::unique_lock<LockPolicy>   WriteLock;
		//typedef boost::shared_lock<LockPolicy>   ReadLock;
	protected:
		typedef Loki::Functor<Ret, TYPELIST_4(SmartPtr<Tprotocol>, SmartPtr<Tpara>, SmartPtr<Tprotocol>&, SmartPtr<Tpara>&)> HandlerFunctor;
		//typedef Loki::Functor<Ret, TYPELIST_2(SmartPtr<Tprotocol>, SmartPtr<Tpara>)> HandlerFunctorSimple;
	public:
		Dispatcher() {}
		virtual ~Dispatcher()
		{
		}
		Ret Handle(SmartPtr<Tprotocol> reqSmart, SmartPtr<Tpara> paraInSmart, SmartPtr<Tprotocol>& resSmart, SmartPtr<Tpara>& paraOutSmart)
		{
			std::string strKey = CalKey(reqSmart);
			//ReadLock lock(m_rwLock);
			typename std::map< std::string, HandlerFunctor >::iterator iter = m_syncMsgHandlerMap.find(strKey);
			if (iter != m_syncMsgHandlerMap.end())
			{
				return iter->second(reqSmart, paraInSmart, resSmart, paraOutSmart);
			}
			std::stringstream strStream;
			strStream << "can't find handler, dispatch key:" << strKey;
			SULOKI_WRITELOG_SULOKI(Log::FILETYPE, strStream.str(), LOG_WARN_LEVEL);
			return HandleUnmatched(reqSmart, paraInSmart);
		}
		/*Ret Handle(SmartPtr<Tprotocol> msgSmart, SmartPtr<Tpara> paraSmart)
		{
		std::string strKey = CalKey(msgSmart);
		ReadLock lock(m_rwLock);
		typename std::map< std::string, HandlerFunctorSimple >::iterator iter = m_syncMsgHandlerMapSimple.find(strKey);
		if (iter != m_syncMsgHandlerMapSimple.end())
		{
		return iter->second(msgSmart, paraSmart);
		}
		std::stringstream strStream;
		strStream << "can't find handler, dispatch key:" << strKey;
		SULOKI_WRITELOG_SULOKI(Log::FILETYPE, strStream.str(), LOG_WARN_LEVEL);
		return HandleUnmatched(msgSmart, paraSmart);
		}*/
	protected:
		virtual std::string CalKey(SmartPtr<Tprotocol> protocolReqSmart)
		{
			return "";
		}
		virtual Ret HandleUnmatched(SmartPtr<Tprotocol> protocolReqSmart, SmartPtr<Tpara> paraInSmart)
		{
			return FAIL;
		}
	private:
		Dispatcher(Dispatcher& ref) {}
		Dispatcher& operator=(Dispatcher& ref) { return *this; }
	protected:
		//?????
		//LockPolicy m_rwLock;
		std::map< std::string, HandlerFunctor > m_syncMsgHandlerMap;
		//std::map< std::string, HandlerFunctorSimple > m_syncMsgHandlerMapSimple;
	};
	//urc
	//????? is old code,check code carefully
	typedef Loki::Functor<void, TYPELIST_4(SmartPtr<Message>, SmartPtr<Context>, SmartPtr<Message>&, SmartPtr<Context>&)> AsyncFunc;
	class UrcInterface
	{
	public:
		virtual ~UrcInterface() {}
		virtual Ret AddData(std::string strUrName, std::string data, Int attrib = 0, bool bDir = false) = 0;
		virtual Ret AddSmartPtr(std::string strUrName, SmartPtr< Base > ptrSmart, bool bDir = false) = 0;
		virtual Ret AddQueue(std::string strUrName, Int attrib) = 0;
		virtual Ret DelData(std::string strUrName, std::string& data) = 0;
		virtual Ret DelSmartPtr(std::string strUrName, SmartPtr< Base >& ptrSmart) = 0;
		virtual Ret DelQueue(std::string strUrName) = 0;
		virtual Ret UpdateData(std::string strUrName, std::string data, std::string& dataOri) = 0;
		virtual Ret UpdateSmart(std::string strUrName, SmartPtr< Base > ptrSmart, SmartPtr< Base >& ptrSmartOri) = 0;
		virtual Ret GetData(std::string strUrName, std::string& data) = 0;
		virtual Ret GetSmartPtr(std::string strUrName, SmartPtr< Base >& ptrSmart) = 0;
		virtual Ret Subscribe(std::string strUrName, std::string strSubName, AsyncFunc func, SmartPtr<Message> msgOriSmart, SmartPtr<Context> contextOriSmart) = 0;
		virtual Ret Unsubscribe(std::string strUrName, std::string strSubName) = 0;
		virtual Ret PostMsg(std::string strUrName, SmartPtr<Message> msgSmart, SmartPtr<Context> contextSmart) = 0;
	protected:
		UrcInterface() {}
	private:
		UrcInterface(UrcInterface& ref) {}
		UrcInterface& operator=(UrcInterface& ref) { return *this; }
	protected:
	};
	//????? is old code,check code carefully
	class IdManager : public Base, public StateMachine
	{
	public:
		IdManager() :m_connId(0) {}
		virtual ~IdManager() {}
		//StateMachine interface
		virtual Ret Init(void)
		{
			return SUCCESS;
		}
		virtual Ret Start(void)
		{
			return SUCCESS;
		}
		virtual Ret Run(void)
		{
			return SUCCESS;
		}
		virtual Ret Stop(void)
		{
			return SUCCESS;
		}
		virtual Ret Clear(void)
		{
			return SUCCESS;
		}
		//????? m_connId is too big must handle
		Int GetId(void)
		{
			return SULOKI_INCREMENT_SULOKI(&m_connId);
		}
	private:
		IdManager(IdManager& ref) {}
		IdManager& operator=(IdManager& ref) { return *this; }
	protected:
		volatile Int m_connId;
	};
	typedef Singleton< IdManager > IdManagerSingleton;
	//????? is old code,check code carefully
	//const std::string URC_MAINGROUP_SERVICE = "maingroup";
	//const std::string URC_MAINSERVICE_SERVICE = "mainservice";
	class ServiceManager;
	class Service : public Base
	{
		typedef boost::unique_lock<Suloki::RwLockPolicy>   WriteLock;
		typedef boost::shared_lock<Suloki::RwLockPolicy>   ReadLock;
	public:
		Service() :m_pUrcInterface(NULL) {}
		virtual ~Service() {}
		virtual Ret Init(UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
		{
			//WriteLock lock(m_rwLock);
			m_pUrcInterface = pUrcInterface;
			m_groupName = groupName;
			m_serviceName = serviceName;
			return SUCCESS;
		}
		virtual Ret Start(void) { return FAIL; }
		virtual Ret Handle(SmartPtr<Message> msgSmart, SmartPtr<Context> paraSmart, SmartPtr<Message>& resSmart, SmartPtr<Context>& paraOutSmart) { return FAIL; }
		virtual Ret Stop(void) { return FAIL; }
		virtual Ret Clear(void) { return FAIL; }
		//
		inline Ret GetUrcInterface(UrcInterface** ppUrcInterface)
		{
			//ReadLock lock(m_rwLock);
			*ppUrcInterface = m_pUrcInterface;
			return SUCCESS;
		}
		void GetGroupService(std::string& group, std::string& service)
		{
			//ReadLock lock(m_rwLock);
			group = m_groupName;
			service = m_serviceName;
		}
	private:
		Service(Service& ref) {}
		Service& operator=(Service& ref) { return *this; }
	protected:
		//RwLockPolicy m_rwLock;
		UrcInterface* m_pUrcInterface;
		std::string m_groupName;
		std::string m_serviceName;
		//
		friend class ServiceManager;
	};
	typedef Ret(SULOKI_STDCALL_SULOKIDEF *ServiceInit)(SULOKI_IN_SULOKIDEF UrcInterface* pUrcInterface, SULOKI_IN_SULOKIDEF std::string groupName, SULOKI_IN_SULOKIDEF std::string  serviceName);
	typedef Ret(SULOKI_STDCALL_SULOKIDEF *ServiceStart)(void);
	typedef Ret(SULOKI_STDCALL_SULOKIDEF *ServiceHandle)(SULOKI_IN_SULOKIDEF SmartPtr<Message> msgSmart, SULOKI_IN_SULOKIDEF SmartPtr<Context> paraSmart, SULOKI_IN_SULOKIDEF SmartPtr<Message>& resSmart, SULOKI_IN_SULOKIDEF SmartPtr<Context>& paraOutSmart);
	typedef Ret(SULOKI_STDCALL_SULOKIDEF *ServiceStop)(void);
	typedef Ret(SULOKI_STDCALL_SULOKIDEF *ServiceClear)(void);
	//????? is old code,check code carefully
	//Use inside by urc
	//????? EventManager is old code,check code carefully
	const Int SULOKI_MAX_TIMEOUT_EVENTMANAGER = 1000 * 60;
	class EventManager : public Base, public StateMachine
	{
		typedef boost::unique_lock<RwLockPolicy>   WriteLock;
		typedef boost::shared_lock<RwLockPolicy>   ReadLock;
		enum { INITNUM_TIMER = 1024, INITNUM_THREAD = SULOKI_MAX_THREADNUM_ALLOCATER_SULOKI / 2 };
	public:
		class TimerItem : public Base
		{
		public:
			TimerItem() {}
			virtual ~TimerItem()
			{
				if (m_timerSmart.get() != NULL)
					m_timerSmart->cancel();
			}
			Ret Init(boost::asio::io_service& ioService)
			{
				m_timerSmart = std::auto_ptr<boost::asio::steady_timer>(new boost::asio::steady_timer(ioService));
				if (m_timerSmart.get() == NULL)
					return FAIL;
				return SUCCESS;
			}
			boost::asio::steady_timer& GetTimer(void)
			{
				return *m_timerSmart;
			}
		protected:
			std::auto_ptr<boost::asio::steady_timer> m_timerSmart;
		};
		class ConvarItem : public Base
		{
		public:
			ConvarItem() :m_ready(false), m_bTimeout(false)
			{}
			virtual ~ConvarItem()
			{}
		public:
			boost::condition_variable_any m_cond;
			boost::mutex m_mutex;
			volatile bool m_ready;
			volatile bool m_bTimeout;
			SmartPtr<Message> m_msgSmart;
			SmartPtr<TimerItem> m_timerSmart;
		};
	public:
		EventManager() {}
		virtual ~EventManager() {}
		//StateMachine interface
		virtual Ret Init(void);
		virtual Ret Start(void);
		virtual Ret Run(void);
		virtual Ret Stop(void);
		virtual Ret Clear(void);
		//
		SmartPtr<ConvarItem> PopConvar(void);
		Ret PushConvar(SmartPtr<ConvarItem> convarSmart);
		Ret Wait(std::string strUrcKey, Int timeout, SmartPtr<Message>& msgSmart);
		Ret Notify(SmartPtr<Message> msgSmart);
		//Ret SetTimer(std::string strUrcKey, Int timeout);
		Ret SetTimerDelObjUr(std::string strUrcKey, Int timeout);
	protected:
		void MyTimeout(std::string strUrcKey);// , SmartPtr<TimerItem> timerSmart, SmartPtr<ConvarItem> convarSmart);
											  //void MyAsyncTimeout(std::string strUrName, SmartPtr<TimerItem> timerSmart);
		void MyTimeoutDelObjUr(std::string strUrName, SmartPtr<TimerItem> timerSmart);
		void MyTimeoutForever(std::string strUrcKey);
		void RunThread(void);
	private:
		EventManager(EventManager& ref) {}
		EventManager& operator=(EventManager& ref) { return *this; }
	protected:
		boost::asio::io_service m_ioService;
		std::auto_ptr<boost::thread> m_threadRunnedSmart;
		//
		SmartPtr<TimerItem> m_timerForeverSmart;
		Queue<TimerItem> m_timerQueue;
		//
		Queue<ConvarItem> m_convarQueue;
		//RwLockPolicy m_rwLock;
		//std::vector<boost::asio::steady_timer*> m_timerVector;
	};
	typedef Singleton<EventManager> EventManagerSingleton;
	//????? is old code,check code carefully
	class ServiceManager : public Base, public StateMachine
	{
	public:
		ServiceManager() {}
		virtual ~ServiceManager() {}
		//StateMachine interface
		virtual Ret Init(void);
		virtual Ret Start(void);
		virtual Ret Run(void);
		virtual Ret Stop(void);
		virtual Ret Clear(void);
		//
		Ret AddServide(std::string groupName, std::string serviceName, SmartPtr< Service > serviceSmart);
		Ret RemoveServide(std::string groupName, std::string serviceName, SmartPtr< Service >& serviceSmart);
		Ret GetServide(std::string groupName, std::string serviceName, SmartPtr< Service >& serviceSmart);
		Ret SubUr(std::string groupName, std::string serviceName, std::string strUrName);
		Ret UnsubUr(std::string groupName, std::string serviceName, std::string strUrName);
	protected:
		static void Handle(SmartPtr<Message> msgOriSmart, SmartPtr<Context> contestOriSmart, SmartPtr<Message>& msgSmart, SmartPtr<Context>& contestSmart);
	private:
		ServiceManager(ServiceManager& ref) {}
		ServiceManager& operator=(ServiceManager& ref) { return *this; }
	protected:
	};
	//typedef Singleton< ServiceManager > ServiceManagerSingleton;
	//????? is old code,check code carefully
	class NetSession : public Base
	{
	public:
		NetSession() {}
		virtual ~NetSession() {}
		//

	private:
		NetSession(NetSession& ref) {}
		NetSession& operator=(NetSession& ref) { return *this; }
	protected:
	};
	//????? is old code,check code carefully
	//????? log is old code,check code carefully
	//urc path start flag about auth
	const std::string URC_MAINGROUP_SERVICE = "maingroup";
	const std::string URC_MAINSERVICE_SERVICE = "mainservice";
	const std::string URC_URCGROUP_SERVICE = "urcgroup";
	const std::string URC_URCSERVICE_SERVICE = "urcservice";
	const std::string URC_MINID_URCSERVICE_SERVICE = "0";
	//urc inside
	const std::string URC_URC_ROOT = "/";
	const std::string URC_URC_NAMEPREFIX = "/urcsys/";//this is urc reserved type, used by urc server only
	const std::string URC_URC_MQUEUE_NAMEPREFIX = URC_URC_NAMEPREFIX + "mqueue/";//for dispatch msg to servide on urcservice
																				 //const std::string URC_URC_URCSERVICES_NAMEPREFIX = URC_URC_NAMEPREFIX + "urcservices/";//include all urc service info,formed a circle
																				 //const std::string URC_URC_TCPCONN_NAMEPREFIX = URC_URC_NAMEPREFIX + "tcpconn/";
																				 //const std::string URC_URC_SERVICES_NAMEPREFIX = URC_URC_NAMEPREFIX + "services/";//include all system service info
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
	const std::string URC_URC_PSTORAGE_NAMEPREFIX = URC_URC_NAMEPREFIX + "pstorage/";//urc pstorage
#endif
																					 //system,service relevant
	const std::string URC_SYSTEM_NAMEPREFIX = "/service/";//service used
	const std::string URC_SYSTEM_WSCONN_NAMEPREFIX = URC_SYSTEM_NAMEPREFIX + "wsconn/";
	const std::string URC_SYSTEM_WSCLIENTCONN_NAMEPREFIX = URC_SYSTEM_NAMEPREFIX + "wsclientconn/";
	const std::string URC_SYSTEM_PACKER_NAMEPREFIX = URC_SYSTEM_NAMEPREFIX + "packer/";
	const std::string URC_SYSTEM_RESPONSE_NAMEPREFIX = URC_SYSTEM_NAMEPREFIX + "response/";//for response of net reqres api
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
	const std::string URC_SYSTEM_PSTORAGE_NAMEPREFIX = URC_SYSTEM_NAMEPREFIX + "pstorage/";//system(service) pstorage
	const std::string URC_URCSERVICE_PSTORAGE_NAMEPREFIX = URC_SYSTEM_NAMEPREFIX + "pstorage/" + URC_URCGROUP_SERVICE + "/";//urc service pstorage
#endif
																															//business,user relevant
	const std::string URC_BUSINESS_NAMEPREFIX = "/business/";//user used
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
	const std::string URC_BUSINESS_PSTORAGE_NAMEPREFIX = URC_BUSINESS_NAMEPREFIX + "pstorage/";//user used for persistent storage
	const std::string URC_SERVICES_PSTORAGE_NAMEPREFIX = URC_BUSINESS_PSTORAGE_NAMEPREFIX + "services/";//service persistent storage
	const std::string URC_USERS_PSTORAGE_NAMEPREFIX = URC_BUSINESS_PSTORAGE_NAMEPREFIX + "users/";//user persistent storage
	const std::string URC_DBNAME_PSTORAGE = "sulokidb";
#endif
	//const std::string URC_BUSINESS_VIDEDRTSP_NAMEPREFIX = URC_BUSINESS_NAMEPREFIX + "videortsp/";
	//normal key define
	const std::string URC_PARAQUEUE_NAMEKEY = "/paraqueue";
	const std::string URC_SERIALQUEUE_NAMEKEY = "/serialqueue";
	//urc type
	const Int URC_DATA_TYPE = 1;
	//const Int URC_SQLDATA_TYPE = 1;
	const Int URC_MSGQUEUE_TYPE = 3;
	const Int URC_DIR_TYPE = 4;
	const Int URC_OBJECT_TYPE = 5;
	//const Int URC_EVENT_TYPE = 6;
	//urc attrib
	const Int URC_ZERO_ATTR = 0;
	const Int URC_PROXY_ATTR = 1;//this bit is 0 mean not cache
	const Int URC_EXCLUSIVE_ATTR = 2;//this bit is 0 mean shared
	const Int URC_SERIAL_ATTR = 4;//this bit is 0 mean one callback function parallel,call continue
	const Int URC_BROADCAST_ATTR = 8;//this bit is valid when URC_EXCLUSIVE_ATTR have not setted
	const Int URC_ONLYONE_ATTR = 16;//only one msg
									//const Int URC_BUSYDEGREE_ATTR = 16;//this bit is 0 mean one bu one when URC_BROADCAST_ATTR have not setted
									//invalid char _
	const std::string URC_INVALIDCHAR_URNAME = "_";
	//min max urname
	const std::string URC_MIN_URNAME = "\0";
	//const std::string URC_USERMIN_URNAME = "A";
	//const std::string URC_USERMAX_URNAME = "ZZZZZZZZ";
	const std::string URC_MAX_URNAME = "zzzzzzzz";
	//????? used free version temp
	//template < typename LockPolicy = RwLockPolicy>
	class Urc : public Base, public StateMachine, public UrcInterface
	{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
		enum { MAXNUM_SLOT = 128 };
		typedef RwLockPolicy LockPolicy;
		typedef boost::unique_lock<LockPolicy>   WriteLock;
		typedef boost::shared_lock<LockPolicy>   ReadLock;
#else
#endif
		//public:
		//	typedef Loki::Functor<void, TYPELIST_4(SmartPtr<Message>, SmartPtr<Context>, SmartPtr<Message>, SmartPtr<Context>)> AsyncFunc;
	private:
		enum { MAXNUM_THREAD = SULOKI_MAX_THREADNUM_ALLOCATER_SULOKI / 2 };
		enum { MAXNUM_QUEUESERI_THREAD = 2, MAXNUM_QUEUEPARA_THREAD = 4 };
		class InfoSubscribed : public Base
		{
		public:
			InfoSubscribed()//:m_ref(0)
			{
				m_queueSmart = SmartPtr<Queue<Message>>(new Queue<Message>());
				//m_refSmart = SmartPtr<Int>(new Int(0));
				m_ref = 0;
				m_maxNumQueuePara = MAXNUM_QUEUEPARA_THREAD;
			}
			InfoSubscribed(AsyncFunc cb, SmartPtr<Message> msgOriSmart, SmartPtr<Context> contextOriSmart, std::string strSubName, Int maxNumQueuePara = MAXNUM_QUEUEPARA_THREAD)//:m_ref(0)
			{
				m_cb = cb;
				m_msgOriSmart = msgOriSmart;
				m_contextOriSmart = contextOriSmart;
				m_strSubName = strSubName;
				m_queueSmart = SmartPtr<Queue<Message>>(new Queue<Message>());
				//m_refSmart = SmartPtr<Int>(new Int(0));
				m_ref = 0;
				m_maxNumQueuePara = maxNumQueuePara;// MAXNUM_QUEUEPARA_THREAD;
			}
			virtual ~InfoSubscribed()
			{}
			/*
			InfoSubscribed(InfoSubscribed& ref)
			{
			m_cb = ref.m_cb;
			m_msgOriSmart = ref.m_msgOriSmart;
			m_contextOriSmart = ref.m_contextOriSmart;
			m_strSubName = ref.m_strSubName;
			m_queueSmart = ref.m_queueSmart;
			m_ref = ref.m_ref;
			}
			InfoSubscribed& operator=(InfoSubscribed& ref)
			{
			if (this != &ref)
			{
			m_cb = ref.m_cb;
			m_msgOriSmart = ref.m_msgOriSmart;
			m_contextOriSmart = ref.m_contextOriSmart;
			m_strSubName = ref.m_strSubName;
			m_queueSmart = ref.m_queueSmart;
			m_ref = ref.m_ref;
			}
			return *this;
			}
			*/
			AsyncFunc m_cb;
			SmartPtr<Message> m_msgOriSmart;
			SmartPtr<Context> m_contextOriSmart;
			std::string m_strSubName;
			//Queue<Message > m_queue;
			SmartPtr<Queue<Message>> m_queueSmart;
			volatile Int m_ref;
			//SmartPtr<Int> m_refSmart;
			Int m_maxNumQueuePara;
		};
		class Wraper : public Base
		{
		public:
			explicit Wraper(Int attrib) // :m_currentSubscriber(0)
			{
				m_attrib = attrib;
				//m_currentSubscriberSmart = SmartPtr<Int>(new Int(0));
				m_currentSubscriber = 0;
				//m_refSmart = SmartPtr<Int>(new Int(0));
				m_queueSmart = SmartPtr<Queue<Message>>(new Queue<Message>());
				m_ref = 0;
			}
			Wraper(Int type, Int attrib)// :m_currentSubscriber(0)
			{
				m_type = type; m_attrib = attrib;
				//m_currentSubscriberSmart = SmartPtr<Int>(new Int(0));
				m_currentSubscriber = 0;
				//m_refSmart = SmartPtr<Int>(new Int(0));
				m_queueSmart = SmartPtr<Queue<Message>>(new Queue<Message>());
				m_ref = 0;
			}
			virtual ~Wraper()
			{
				//for (std::map< std::string, InfoSubscribed* >::iterator iter = m_subscriberMap.begin(); iter != m_subscriberMap.end(); iter++)
				//{
				//	InfoSubscribed* pInfoSubscribed = (InfoSubscribed*)(iter->second);
				//	delete pInfoSubscribed;
				//}
				m_subscriberMap.clear();
			}
			/*
			Wraper(Wraper& ref)
			{
			m_type = ref.m_type;
			m_attrib = ref.m_attrib;
			m_currentSubscriber = ref.m_currentSubscriber;
			m_queueSmart = ref.m_queueSmart;
			m_ref = ref.m_ref;
			m_subscriberMap = ref.m_subscriberMap;
			}
			Wraper& operator=(Wraper& ref)
			{
			if (this != &ref)
			{
			m_type = ref.m_type;
			m_attrib = ref.m_attrib;
			m_currentSubscriber = ref.m_currentSubscriber;
			m_queueSmart = ref.m_queueSmart;
			m_ref = ref.m_ref;
			m_subscriberMap = ref.m_subscriberMap;
			}
			return *this;
			}
			*/
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
			Int m_type;
			Int m_attrib;
			volatile Int m_currentSubscriber;
			//SmartPtr<Int> m_currentSubscriberSmart;
			//Queue<Message > m_queue;
			SmartPtr<Queue<Message>> m_queueSmart;
			volatile Int m_ref; //SmartPtr<Int> m_refSmart;
			std::map< std::string, SmartPtr<InfoSubscribed> > m_subscriberMap;
		};
		class WraperDir : public Wraper
		{
		public:
			explicit WraperDir(Int type) :Wraper(type, 0)
			{}
			virtual ~WraperDir()
			{
			}
			/*
			WraperDir(WraperDir& ref):Wraper::Wraper(ref)
			{
			m_strIndex = ref.m_strIndex;
			}
			WraperDir& operator=(WraperDir& ref)
			{
			if (this != &ref)
			{
			Wraper::Wraper(ref);
			m_strIndex = ref.m_strIndex;
			}
			return *this;
			}
			*/
			//????? length is limited
			std::string m_strIndex;
		};
		template <typename T>
		class WraperObject : public Wraper
		{
		public:
			WraperObject(Int type, Int attrib, bool bDir, T val) :Wraper(type, attrib)
			{
				m_val = val; m_bDir = bDir;
			}
			virtual ~WraperObject()
			{
			}
			/*
			WraperObject(WraperObject& ref) :Wraper::Wraper(ref)
			{
			m_bDir = ref.m_bDir;
			m_val = ref.m_val;
			m_strDir = ref.m_strDir;
			}
			WraperObject& operator=(WraperObject& ref)
			{
			if (this != &ref)
			{
			Wraper::Wraper(ref);
			m_bDir = ref.m_bDir;
			m_val = ref.m_val;
			m_strDir = ref.m_strDir;
			}
			return *this;
			}
			*/
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
			bool m_bDir;
			T m_val;
			std::string m_strDir;
		};
	public:
		Urc()
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
		}
		virtual ~Urc()
		{
		}
		//StateMachine interface
		virtual Ret Init(void)
		{
			m_threadPool.Init();
			//?????
			//std::string strServicesList = "{\"group\":\"urcgroup\",\"services\":[{\"name\":\"urcservice0\",\"ip\":\"172.16.1.231\",\"wsport\":\"1008\"}]}";
			//if(AddUr(URC_URCSERVICES_KEY, strServicesList, URC_DATA_TYPE, 0, false) != SUCCESS)
			//	return FAIL;
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
			std::string pstorage;
			ConfigSingleton::Instance().GetConfig(CONFIG_PSTORAGE_KEY, pstorage);
			//
			m_pDb = NULL;
			if (pstorage.compare(CONFIG_TRUE_RESTART_VAL) == 0)
			{
				leveldb::Options options;
				options.create_if_missing = true;
				leveldb::Status status = leveldb::DB::Open(options, URC_DBNAME_PSTORAGE, &m_pDb);
				if (!status.ok())
				{
					return FAIL;
				}
				leveldb::Iterator* iter = m_pDb->NewIterator(leveldb::ReadOptions());
				for (iter->SeekToFirst(); iter->Valid(); iter->Next())
				{
					if (AddData(iter->key().ToString(), iter->value().ToString(), 0, true) != SUCCESS)
					{
						delete iter;
						return FAIL;
					}
				}
				if (!iter->status().ok())
				{
					delete iter;
					return FAIL;
				}
				delete iter;
			}
#endif
			return SUCCESS;
		}
		virtual Ret Start(void)
		{
			return SUCCESS;
		}
		virtual Ret Run(void)
		{
			return SUCCESS;
		}
		virtual Ret Stop(void)
		{
			return SUCCESS;
		}
		virtual Ret Clear(void)
		{
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
			if (m_pDb != NULL)
				delete m_pDb;
#endif
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			for (int i = 0; i < MAXNUM_SLOT; i++)
			{
				WriteLock lock(m_rwLock[i]);
				for (std::map<std::string, void*>::iterator iter = m_urcMap[i].begin(); iter != m_urcMap[i].end(); iter++)
				{
					SmartPtr<Wraper> wraperSmart;
					wraperSmart.Capture((Wraper*)iter->second);
				}
				m_urcMap[i].clear();
			}
#else
#endif
			return SUCCESS;
		}
	public:
		//urc interface
		virtual Ret AddData(std::string strUrName, std::string data, Int attrib = 0, bool bDir = false)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			if (strUrName.find(URC_INVALIDCHAR_URNAME) != std::string::npos)
				return INVALIDPARA_ERRCODE;
			Ret ret = AddUr(strUrName, data, URC_DATA_TYPE, attrib, bDir);
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
			if (ret == SUCCESS && strUrName.find(URC_BUSINESS_PSTORAGE_NAMEPREFIX) == 0 && m_pDb != NULL)
			{
				m_pDb->Put(leveldb::WriteOptions(), strUrName, data);
			}
#endif
			return ret;
		}
		virtual Ret AddSmartPtr(std::string strUrName, SmartPtr< Base > ptrSmart, bool bDir = false)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			if (strUrName.find(URC_INVALIDCHAR_URNAME) != std::string::npos)
				return INVALIDPARA_ERRCODE;
			return AddUr(strUrName, ptrSmart, URC_OBJECT_TYPE, 0, bDir, 0);
		}
		virtual Ret AddQueue(std::string strUrName, Int attrib)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			if (strUrName.find(URC_INVALIDCHAR_URNAME) != std::string::npos)
				return INVALIDPARA_ERRCODE;
			Queue<Message> queue;
			return AddUr(strUrName, queue, URC_MSGQUEUE_TYPE, attrib, false, 0);
		}
		/*
		virtual Ret AddResponseCb(std::string strUrName, AsyncFunc asyncCb, Int timeout, SmartPtr<Message> msgOriSmart, SmartPtr<Context> contextOriSmart)
		{
		if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
		return INVALIDPARA_ERRCODE;
		if (strUrName.find(URC_INVALIDCHAR_URNAME) != std::string::npos)
		return INVALIDPARA_ERRCODE;
		if (!(timeout > 0 && timeout < SULOKI_MAX_TIMEOUT_EVENTMANAGER))
		return INVALIDPARA_ERRCODE;
		Queue<Message> queue;
		if (AddUr(strUrName, queue, URC_MSGQUEUE_TYPE, URC_EXCLUSIVE_ATTR | URC_SERIAL_ATTR | URC_ONLYONE_ATTR, false, 0) != SUCCESS)
		return FAIL;
		if (SubscribeUr(strUrName, "response", asyncCb, msgOriSmart, contextOriSmart) != SUCCESS)
		{
		DelUr(strUrName, queue);
		return FAIL;
		}
		if (EventManagerSingleton::Instance().SetTimer(strUrName, timeout) != SUCCESS)
		{
		AsyncFunc func;
		SmartPtr<Message> msgOriSmart;
		SmartPtr<Context> contextOriSmart;
		UnsubscribeUr(strUrName, "response", func, msgOriSmart, contextOriSmart);
		DelUr(strUrName, queue);
		return FAIL;
		}
		return SUCCESS;
		}
		virtual Ret Response(std::string strUrName, SmartPtr<Message> msgSmart, SmartPtr<Context> contextSmart)
		{
		Ret ret = PostMsgSubUr(strUrName, msgSmart, SmartPtr<Context>(NULL));
		return ret;
		}
		*/
		virtual Ret DelData(std::string strUrName, std::string& data)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
			if (strUrName.find(URC_BUSINESS_PSTORAGE_NAMEPREFIX) == 0 && m_pDb != NULL)
			{
				m_pDb->Delete(leveldb::WriteOptions(), strUrName);
			}
#endif
			return DelUr(strUrName, data);
		}
		virtual Ret DelSmartPtr(std::string strUrName, SmartPtr< Base >& ptrSmart)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			return DelUr(strUrName, ptrSmart);
		}
		virtual Ret DelQueue(std::string strUrName)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			Queue<Message> queue;
			return DelUr(strUrName, queue);
		}
		virtual Ret UpdateData(std::string strUrName, std::string data, std::string& dataOri)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			Ret ret = UpdateUr(strUrName, data, dataOri);
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
			if (ret == SUCCESS && strUrName.find(URC_BUSINESS_PSTORAGE_NAMEPREFIX) == 0 && m_pDb != NULL)
			{
				m_pDb->Put(leveldb::WriteOptions(), strUrName, data);
			}
#endif
			return ret;
		}
		virtual Ret UpdateSmart(std::string strUrName, SmartPtr< Base > ptrSmart, SmartPtr< Base >& ptrSmartOri)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			return UpdateUr(strUrName, ptrSmart, ptrSmartOri);
		}
		virtual Ret GetData(std::string strUrName, std::string& data)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			return GetUr(strUrName, data);
		}
		virtual Ret GetSmartPtr(std::string strUrName, SmartPtr< Base >& ptrSmart)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			return GetUr(strUrName, ptrSmart);
		}
		//can sub any ur
		virtual Ret Subscribe(std::string strUrName, std::string strSubName, AsyncFunc func, SmartPtr<Message> msgOriSmart, SmartPtr<Context> contextOriSmart)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			return SubscribeUr(strUrName, strSubName, func, msgOriSmart, contextOriSmart);
		}
		virtual Ret Unsubscribe(std::string strUrName, std::string strSubName)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			AsyncFunc func;
			SmartPtr<Message> msgOriSmart;
			SmartPtr<Context> contextOriSmart;
			return UnsubscribeUr(strUrName, strSubName, func, msgOriSmart, contextOriSmart);
		}
		//post msg to queue;async,mean dispatch msg with new thread in threadpool  
		virtual Ret PostMsg(std::string strUrName, SmartPtr<Message> msgSmart, SmartPtr<Context> contextSmart)
		{
			if (strUrName.find(URC_BUSINESS_NAMEPREFIX) != 0)
				return INVALIDPARA_ERRCODE;
			return PostMsgSubUr(strUrName, msgSmart, contextSmart);
		}
	public:
		//add
		template <typename T>
		Ret AddUr(std::string strUrName, T& tVal, Int type, Int attrib = 0, bool bDir = false, Int timeout = 0)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			return AddUrIn(strUrName, tVal, type, attrib, bDir, timeout, Loki::Type2Type<T>());
		}
	protected:
		template <typename T>
		Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, Loki::Type2Type< std::string >)
		{
			if (!(type == URC_DATA_TYPE))
				return INVALIDPARA_ERRCODE;
			return AddUrInIn(strUrName, tVal, type, attrib, bDir);
		}
		template <typename T, typename Tptr>
		Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, Loki::Type2Type< SmartPtr<Tptr> >)
		{
			if (!(type == URC_OBJECT_TYPE))
				return INVALIDPARA_ERRCODE;
			return AddUrInIn(strUrName, tVal, type, URC_ZERO_ATTR, bDir);
		}
		//
		//template <typename T>//, typename Tpcb>
		//Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, Loki::Type2Type< AsyncNewFunc >)
		//{
		//if (!(type == SULOKI_EVENT_TYPE_URC_BASE))
		//return INVALIDPARA_ERRCODE;
		//if (!(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0 || strUrName.find(SULOKI_URCSYS_RESOURCE_URC_BASE) == 0))
		//return URC_INVALIDPATH_ERRORCODE;
		//return AddUrInIn(strUrName, tVal, type, attrib, bDir);
		//}
		//
		template <typename T>
		Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, Loki::Type2Type< Queue<Message> >)
		{
			if (!(type == URC_MSGQUEUE_TYPE))
				return INVALIDPARA_ERRCODE;
			//????? queue do't used
			SmartPtr<Queue<Message>> queueSmart(new Queue<Message>());
			if (queueSmart.IsNULL())
				return NOMEMORY_ERRCODE;
			return AddUrInIn(strUrName, queueSmart, type, attrib, bDir);
		}
		template <typename T>
		Ret AddUrInIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir)
		{
			SmartPtr< WraperObject<T> > objSmart(new WraperObject<T>(type, attrib, bDir, tVal));
			if (objSmart.IsNULL())
				return NOMEMORY_ERRCODE;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			SmartPtr< WraperDir > objDirSmart;
			std::string strPath;
			char cName[SULOKI_MAXLEN_URNAME_SULOKIDEF + 2];
			if (bDir)
			{
				size_t pos = strUrName.rfind('/');
				strPath = strUrName.substr(0, pos + 1);
				//std::cout << strPath << std::endl;
				objDirSmart = SmartPtr< WraperDir >(new WraperDir(URC_DIR_TYPE));
				if (objDirSmart.IsNULL())
					return NOMEMORY_ERRCODE;
				memset(cName, 0, SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
				//std::string strLastName = strUrName.substr(pos+1);
				//std::cout << strLastName << std::endl;
				sprintf(cName, "%s", strUrName.c_str());
				objDirSmart->m_strIndex.append(cName, SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
			}
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			{
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
				if (iter != m_urcMap[hashId].end())
					return FAIL;//URC_EXISTED_ERRORCODE;
				m_urcMap[hashId].insert(std::pair<std::string, void*>(strUrName, (void*)objSmart.Release()));
			}
			if (bDir)
			{
				hashId = Hash(strPath.c_str(), strPath.length(), MAXNUM_SLOT);
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strPath);
				if (iter != m_urcMap[hashId].end())
				{
					std::string& strIndex = ((WraperDir*)(iter->second))->m_strIndex;
					////char name[SULOKI_MAXLEN_URNAME_DEF+2];
					//memset(cName, 0, SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
					////std::string strLastName = strUrName.substr(pos+1);
					//sprintf(cName, "%s", strUrName.c_str());
					//strIndex.append(cName, SULOKI_MAXLEN_URNAME_SULOKIDEF+2);
					AddDir(strUrName, strIndex);
				}
				else
				{
					m_urcMap[hashId].insert(std::pair<std::string, void*>(strPath, (void*)objDirSmart.Release()));
				}
			}
			return SUCCESS;
#else
#endif
		}
	public:
		//del
		template <typename T>
		Ret DelUr(std::string strUrName, T& tVal)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			return DelUrIn(strUrName, tVal, Loki::Type2Type<T>());
		}
	protected:
		template <typename T>
		Ret DelUrIn(std::string strUrName, T& tVal, Loki::Type2Type< std::string >)
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			SmartPtr< WraperObject<T> > objSmart;
			{
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
				if (iter == m_urcMap[hashId].end())
					return FAIL;// URC_UNEXISTED_ERRCODE;
				Int type = ((Wraper*)(iter->second))->m_type;
				if (!(type == URC_DATA_TYPE))
					return INVALIDPARA_ERRCODE;
				objSmart.Capture((WraperObject<T>*)(iter->second));
				tVal = objSmart->m_val;
				m_urcMap[hashId].erase(iter);
			}
			if (!objSmart.IsNULL() && objSmart->m_bDir)
			{
				std::string strPath;
				size_t pos = strUrName.rfind('/');
				strPath = strUrName.substr(0, pos + 1);
				hashId = Hash(strPath.c_str(), strPath.length(), MAXNUM_SLOT);
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strPath);
				if (iter != m_urcMap[hashId].end())
				{
					std::string& strIndex = ((WraperDir*)(iter->second))->m_strIndex;
					DelDir(strUrName, strIndex);
					if (strIndex.length() == 0)
					{
						SmartPtr< WraperDir > objDirSmart;
						objDirSmart.Capture((WraperDir*)(iter->second));
						//tVal = objSmart->m_val;
						m_urcMap[hashId].erase(iter);
					}
				}
			}
			return SUCCESS;
#else
#endif
		}
		template <typename T, typename Tptr>
		Ret DelUrIn(std::string strUrName, T& tVal, Loki::Type2Type< SmartPtr<Tptr> >)
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			SmartPtr< WraperObject<T> > objSmart;
			{
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
				if (iter == m_urcMap[hashId].end())
					return FAIL;//URC_UNEXISTED_ERRORCODE;
				Int type = ((Wraper*)(iter->second))->m_type;
				if (!(type == URC_OBJECT_TYPE))
					return INVALIDPARA_ERRCODE;
				objSmart.Capture((WraperObject<T>*)(iter->second));
				tVal = objSmart->m_val;
				m_urcMap[hashId].erase(iter);
			}
			if (!objSmart.IsNULL() && objSmart->m_bDir)
			{
				std::string strPath;
				size_t pos = strUrName.rfind('/');
				strPath = strUrName.substr(0, pos + 1);
				hashId = Hash(strPath.c_str(), strPath.length(), MAXNUM_SLOT);
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strPath);
				if (iter != m_urcMap[hashId].end())
				{
					std::string& strIndex = ((WraperDir*)(iter->second))->m_strIndex;
					DelDir(strUrName, strIndex);
					if (strIndex.length() == 0)
					{
						SmartPtr< WraperDir > objDirSmart;
						objDirSmart.Capture((WraperDir*)(iter->second));
						//tVal = objSmart->m_val;
						m_urcMap[hashId].erase(iter);
					}
				}
			}
			return SUCCESS;
#else
#endif
		}
		//
		template <typename T>
		Ret DelUrIn(std::string strUrName, T& tVal, Loki::Type2Type< Queue<Message> >)
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			SmartPtr< WraperObject<T> > objSmart;
			{
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
				if (iter == m_urcMap[hashId].end())
					return FAIL;//URC_UNEXISTED_ERRCODE;
				Int type = ((Wraper*)(iter->second))->m_type;
				if (!(type == URC_MSGQUEUE_TYPE))
					return INVALIDPARA_ERRCODE;
				objSmart.Capture((WraperObject<T>*)(iter->second));
				//SmartPtr<Queue<Message>> queueSmart = objSmart->m_val;
				m_urcMap[hashId].erase(iter);
			}
			if (!objSmart.IsNULL() && objSmart->m_bDir)
			{
				std::string strPath;
				size_t pos = strUrName.rfind('/');
				strPath = strUrName.substr(0, pos + 1);
				hashId = Hash(strPath.c_str(), strPath.length(), MAXNUM_SLOT);
				WriteLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strPath);
				if (iter != m_urcMap[hashId].end())
				{
					std::string& strIndex = ((WraperDir*)(iter->second))->m_strIndex;
					DelDir(strUrName, strIndex);
					if (strIndex.length() == 0)
					{
						SmartPtr< WraperDir > objDirSmart;
						objDirSmart.Capture((WraperDir*)(iter->second));
						//tVal = objSmart->m_val;
						m_urcMap[hashId].erase(iter);
					}
				}
			}
			return SUCCESS;
#else
#endif
		}
		//
		template <typename T>
		Ret DelUrInIn(std::map<std::string, void*>::iterator iter, std::string strUrName, T& tVal, Int type, std::map<std::string, void*>& urcMap)
		{
			SmartPtr< WraperObject<T> > objSmart;
			objSmart.Capture((WraperObject<T>*)(iter->second));
			tVal = objSmart->m_val;
			urcMap.erase(iter);
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			if (objSmart->m_bDir)
			{
				std::string strPath;
				size_t pos = strUrName.rfind('/');
				strPath = strUrName.substr(0, pos + 1);
				std::map<std::string, void*>::iterator iter = urcMap.find(strPath);
				if (iter != urcMap.end())
				{
					std::string& strIndex = ((WraperDir*)(iter->second))->m_strIndex;
					//size_t pos = strIndex.find(strUrName);
					//if (pos != std::string::npos)
					//{
					//	strIndex.erase(pos, SULOKI_MAXLEN_URNAME_SULOKIDEF+2);
					//	if (strIndex.length() == 0)
					//	{
					//		SmartPtr< WraperDir > objDirSmart;
					//		objDirSmart.Capture((WraperDir*)(iter->second));
					//		//tVal = objSmart->m_val;
					//		urcMap.erase(iter);
					//	}
					//}
					DelDir(strUrName, strIndex);
					if (strIndex.length() == 0)
					{
						SmartPtr< WraperDir > objDirSmart;
						objDirSmart.Capture((WraperDir*)(iter->second));
						//tVal = objSmart->m_val;
						urcMap.erase(iter);
					}
				}
			}
			return SUCCESS;
#else
#endif
		}
	public:
		//update
		template <typename T>
		Ret UpdateUr(std::string strUrName, T& tVal, T& tValOld)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			std::map< std::string, SmartPtr<InfoSubscribed> > subscriberMap;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Ret ret = UpdateUrIn(strUrName, tVal, tValOld, subscriberMap, Loki::Type2Type<T>());
#else
#endif
			for (std::map< std::string, SmartPtr<InfoSubscribed> >::iterator iter = subscriberMap.begin(); iter != subscriberMap.end(); iter++)
			{
				std::vector<std::string> constents = iter->second->m_contextOriSmart->Get();
				if (constents.size() > 0 && constents[0].length() > 0)
				{
					std::stringstream strStream;
					strStream << constents[0] << URC_PARAQUEUE_NAMEKEY;
					boost::property_tree::ptree jsonMsg;
					Suloki::Message::MakeJsonMsg(jsonMsg);
					try {
						jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_URC_BUSINESSID_VAL_SULOKIDEF);
						jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_URC_UPDATE_MESSAGEID_VAL_SULOKIDEF);
						jsonMsg.put<std::string>(SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF, strUrName);
						Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
						std::vector<std::string> context;
						Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
						PostMsgSubUr(strStream.str(), msgSmart, contextSmart);
					}
					catch (boost::property_tree::ptree_error pt) {
						SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json exception", Suloki::LOG_ERROR_LEVEL);
					}
					catch (...) {
						SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json exception1", Suloki::LOG_ERROR_LEVEL);
					}
				}
			}
			return ret;
		}
	protected:
		template <typename T>
		Ret UpdateUrIn(std::string strUrName, T& tVal, T& tValOld, std::map< std::string, SmartPtr<InfoSubscribed> >& subscriberMap, Loki::Type2Type< std::string >)
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			WriteLock lock(m_rwLock[hashId]);
			std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
			if (iter == m_urcMap[hashId].end())
				return FAIL;//URC_UNEXISTED_ERRORCODE;
			Int type = ((Wraper*)(iter->second))->m_type;
			//typeOld = type;
			if (!(type == URC_DATA_TYPE))
				return INVALIDPARA_ERRCODE;
			if (((Wraper*)(iter->second))->m_subscriberMap.begin() != ((Wraper*)(iter->second))->m_subscriberMap.end())
				subscriberMap = ((Wraper*)(iter->second))->m_subscriberMap;
			return UpdateUrInIn(iter, strUrName, tVal, tValOld);// , type);
#else
#endif
		}
		template <typename T, typename Tptr>
		Ret UpdateUrIn(std::string strUrName, T& tVal, T& tValOld, std::map< std::string, SmartPtr<InfoSubscribed> >& subscriberMap, Loki::Type2Type< SmartPtr<Tptr> >)
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			WriteLock lock(m_rwLock[hashId]);
			std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
			if (iter == m_urcMap[hashId].end())
				return FAIL;//URC_UNEXISTED_ERRORCODE;
			Int type = ((Wraper*)(iter->second))->m_type;
			//typeOld = type;
			if (!(type == URC_DATA_TYPE || type == URC_OBJECT_TYPE))
				return INVALIDPARA_ERRCODE;
			if (((Wraper*)(iter->second))->m_subscriberMap.begin() != ((Wraper*)(iter->second))->m_subscriberMap.end())
				subscriberMap = ((Wraper*)(iter->second))->m_subscriberMap;
			return UpdateUrInIn(iter, strUrName, tVal, tValOld);// , type);
#else
#endif
		}
		template <typename T>
		Ret UpdateUrInIn(std::map<std::string, void*>::iterator iter, std::string strUrName, T& tVal, T& tValOld)
		{
			WraperObject<T>* pSwaperObject = ((WraperObject<T>*)(iter->second));
			tValOld = pSwaperObject->m_val;
			pSwaperObject->m_val = tVal;
			return SUCCESS;
		}
	public:
		//get
		template <typename T>
		Ret GetUr(std::string strUrName, T& tVal, Int timeout = 0)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			return GetUrIn(strUrName, tVal, timeout, Loki::Type2Type<T>());
		}
	protected:
		template <typename T>
		Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, Loki::Type2Type< std::string >)
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			ReadLock lock(m_rwLock[hashId]);
			std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
			if (iter == m_urcMap[hashId].end())
				return FAIL;//URC_UNEXISTED_ERRORCODE;
			if (!(((Wraper*)(iter->second))->m_type == URC_DATA_TYPE))
				return INVALIDPARA_ERRCODE;
			tVal = ((WraperObject<T>*)(iter->second))->m_val;
#else
#endif
			return SUCCESS;
		}
		template <typename T>
		Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, Loki::Type2Type< std::vector<std::string> >)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			if (strUrName.at(strUrName.length() - 1) != '/')
				return INVALIDPARA_ERRCODE;
			tVal.clear();
			std::string strIndex;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			{
				Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
				ReadLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
				if (iter == m_urcMap[hashId].end())
					return FAIL;// URC_UNEXISTED_ERRCODE;
				if (((Wraper*)(iter->second))->m_type != URC_DIR_TYPE)
					return FAIL;//URC_NOTDIR_ERRCODE;
				strIndex = ((WraperDir*)(iter->second))->m_strIndex;
			}
			Int numInx = strIndex.length() / (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
			for (Int i = 0; i<numInx; i++)
			{
				tVal.push_back(strIndex.c_str() + (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2)*i);
			}
			return SUCCESS;
#else
#endif
		}
		template <typename T, typename Tptr>
		Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, Loki::Type2Type< SmartPtr<Tptr> >)
		{
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			ReadLock lock(m_rwLock[hashId]);
			std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
			if (iter == m_urcMap[hashId].end())
				return FAIL;//URC_UNEXISTED_ERRORCODE;
			if (!(((Wraper*)(iter->second))->m_type == URC_OBJECT_TYPE))
				return INVALIDPARA_ERRCODE;
			//return GetUrInIn(iter, strUrName, tVal, type);
			tVal = ((WraperObject<T>*)(iter->second))->m_val;
			return SUCCESS;
#else
#endif
		}
	public://sub
		   //static void FuncSubModule(SmartPtr<Context> contextOriSmart, SmartPtr<Message> msgSmart, SmartPtr<Context> contextSmart);
		Ret SubscribeUr(std::string strUrName, std::string strSubName, AsyncFunc func, SmartPtr<Message> msgOriSmart, SmartPtr<Context> contextOriSmart, Int maxNumQueuePara = 0)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			if (!(strSubName.length() > 0 && strSubName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			if (contextOriSmart.IsNULL())
				return INVALIDPARA_ERRCODE;
			if (maxNumQueuePara <= 0)
				maxNumQueuePara = MAXNUM_QUEUEPARA_THREAD;
			Wraper* pSwaper = NULL;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			WriteLock lock(m_rwLock[hashId]);
			std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
			if (iter == m_urcMap[hashId].end())
				return FAIL;//URC_UNEXISTED_ERRCODE;
			pSwaper = (Wraper*)(iter->second);
			//if (!(pSwaper->m_type == SULOKI_SQLDATA_TYPE_URC_BASE || pSwaper->m_type == SULOKI_NOSQLDATA_TYPE_URC_BASE))
			//	return URC_CANNOTSUB_ERRORCODE;
			std::map< std::string, SmartPtr<InfoSubscribed> >::iterator iterSub = pSwaper->m_subscriberMap.find(strSubName);
			if (iterSub != pSwaper->m_subscriberMap.end())
				return FAIL;//URC_SUBEXISTED_ERRORCODE;
			SmartPtr<InfoSubscribed> subSmart(new InfoSubscribed(func, msgOriSmart, contextOriSmart, strSubName, maxNumQueuePara));
			if (subSmart.IsNULL())
				return FAIL;//NOMEMORY_ERRORCODE;
							//InfoSubscribed infoSub(func, msgOriSmart, contextOriSmart, strSubName);
							//pSwaper->m_subscriberMap.insert(std::pair< std::string, InfoSubscribed >(strSubName, infoSub));
			pSwaper->m_subscriberMap[strSubName] = subSmart;// .release();// infoSub;
			return SUCCESS;
#else
#endif
		}
		Ret UnsubscribeUr(std::string strUrName, std::string strSubName, AsyncFunc& func, SmartPtr<Message>& msgOriSmart, SmartPtr<Context>& contextOriSmart, Int maxNumQueuePara = 0)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			if (!(strSubName.length() > 0 && strSubName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			if (maxNumQueuePara <= 0)
				maxNumQueuePara = MAXNUM_QUEUEPARA_THREAD;
			Wraper* pSwaper = NULL;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
			WriteLock lock(m_rwLock[hashId]);
			std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
			if (iter == m_urcMap[hashId].end())
				return FAIL;//URC_UNEXISTED_ERRORCODE;
			pSwaper = (Wraper*)(iter->second);
			//if(!(pSwaper->m_type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
			//	return URC_CANNOTSUB_ERRORCODE;
			std::map< std::string, SmartPtr<InfoSubscribed> >::iterator iterSub = pSwaper->m_subscriberMap.find(strSubName);
			if (iterSub == pSwaper->m_subscriberMap.end())
				return FAIL;//URC_SUBUNEXISTED_ERRORCODE;
			func = iterSub->second->m_cb;
			msgOriSmart = iterSub->second->m_msgOriSmart;
			contextOriSmart = iterSub->second->m_contextOriSmart;
			SmartPtr<InfoSubscribed> subSmart(iterSub->second);
			pSwaper->m_subscriberMap.erase(iterSub);
			return SUCCESS;
#else
#endif
		}
	protected:
		void CallResponseDeled(std::string strUrName, std::string strSubName)
		{
			AsyncFunc func;
			SmartPtr<Message> msgOriSmart;
			SmartPtr<Context> contextOriSmart;
			UnsubscribeUr(strUrName, strSubName, func, msgOriSmart, contextOriSmart);
			Queue<Message> queue;
			DelUr(strUrName, queue);
		}
		void CallSub(std::string strUrName, std::string strSubName)
		{
			//Wraper* pSwaper = NULL;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			SmartPtr<Wraper> wraperSmart;
			SmartPtr<InfoSubscribed> subSmart;
			std::map< std::string, SmartPtr<InfoSubscribed> > subscriberMap;
			{
				Wraper* pSwaper = NULL;
				Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
				ReadLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
				if (iter == m_urcMap[hashId].end())
					return;
				pSwaper = (Wraper*)(iter->second);
				wraperSmart = SmartPtr<Wraper>(pSwaper);
				//
				std::map< std::string, SmartPtr<InfoSubscribed> >::iterator iterSub = wraperSmart->m_subscriberMap.find(strSubName);
				if (iterSub == wraperSmart->m_subscriberMap.end())
					return;
				subSmart = iterSub->second;
			}
			//if(!(pSwaper->m_type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
			//	return URC_CANNOTSUB_ERRORCODE;
			//std::map< std::string, SmartPtr<InfoSubscribed> >::iterator iterSub = wraperSmart->m_subscriberMap.find(strSubName);
			//if (iterSub == wraperSmart->m_subscriberMap.end())
			//	return;
			//SmartPtr<InfoSubscribed> sub = iterSub->second;
			while (true)
			{
				SmartPtr<Message> msgSmart = subSmart->m_queueSmart->Pop();
				if (msgSmart.IsNULL())
					break;
				SmartPtr<Context> contextSmart = msgSmart->GetContext();
				subSmart->m_cb(subSmart->m_msgOriSmart, subSmart->m_contextOriSmart, msgSmart, contextSmart);
				if ((wraperSmart->m_attrib & URC_SERIAL_ATTR) != URC_SERIAL_ATTR)
				{
					break;
				}
			}
			SULOKI_DECREMENT_SULOKI(&subSmart->m_ref);////sub.m_ref--;
														 //????? post to a queue to continue send,forbiden miss
			if (subSmart->m_queueSmart->GetSize() > 0)
			{
				//????? !=URC_SERIAL_ATTR maybe use too much thread
				if ((wraperSmart->m_attrib & URC_BROADCAST_ATTR) == URC_BROADCAST_ATTR)
				{
					Int ref = SULOKI_INCREMENT_SULOKI(&subSmart->m_ref);//sub.m_ref++;
					if (((wraperSmart->m_attrib & URC_SERIAL_ATTR) == URC_SERIAL_ATTR && ref >= MAXNUM_QUEUESERI_THREAD) || ((wraperSmart->m_attrib & URC_SERIAL_ATTR) != URC_SERIAL_ATTR && ref >= subSmart->m_maxNumQueuePara))//MAXNUM_QUEUEPARA_THREAD))
					{
						SULOKI_DECREMENT_SULOKI(&subSmart->m_ref);//sub.m_ref--;
					}
					else
					{
						m_threadPool.Post(boost::bind(&Urc::CallSub, this, strUrName, subSmart->m_strSubName));
					}
				}
				else
				{
					Int ref = SULOKI_INCREMENT_SULOKI(&subSmart->m_ref);//sub.m_ref++;
					if (((wraperSmart->m_attrib & URC_SERIAL_ATTR) == URC_SERIAL_ATTR && ref >= MAXNUM_QUEUESERI_THREAD) || ((wraperSmart->m_attrib & URC_SERIAL_ATTR) != URC_SERIAL_ATTR && ref >= subSmart->m_maxNumQueuePara))//MAXNUM_QUEUEPARA_THREAD))
					{
						SULOKI_DECREMENT_SULOKI(&subSmart->m_ref);//sub.m_ref--;
					}
					else
					{
						m_threadPool.Post(boost::bind(&Urc::CallSub, this, strUrName, subSmart->m_strSubName));
					}
				}
			}
			if ((wraperSmart->m_attrib & URC_ONLYONE_ATTR) == URC_ONLYONE_ATTR)
			{
				m_threadPool.Post(boost::bind(&Urc::CallResponseDeled, this, strUrName, strSubName));
			}
#else
#endif
		}
		void CallSubMain(std::string strUrName)
		{
			{
				//Wraper* pSwaper = NULL;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
				SmartPtr<Wraper> wraperSmart;
				{
					Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
					ReadLock lock(m_rwLock[hashId]);
					std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
					if (iter == m_urcMap[hashId].end())
						return;
					Wraper* pSwaper = (Wraper*)(iter->second);
					if (pSwaper->m_type != URC_MSGQUEUE_TYPE)
						return;
					wraperSmart = SmartPtr<Wraper>(pSwaper);
				}
				//WraperObject< SmartPtr<Queue<Message>> >* pWraperObject = (WraperObject< SmartPtr<Queue<Message>> >*)(iter->second);
				//SmartPtr<Message> msgSmart = pWraperObject->m_val->Pop();
				while (true)
				{
					SmartPtr<Message> msgSmart = wraperSmart->m_queueSmart->Pop();
					if (msgSmart.IsNULL())
						break;
					//????? !=URC_SERIAL_ATTR maybe use too much thread
					if ((wraperSmart->m_attrib & URC_BROADCAST_ATTR) == URC_BROADCAST_ATTR)
					{
						for (std::map< std::string, SmartPtr<InfoSubscribed> >::iterator iterSub = wraperSmart->m_subscriberMap.begin(); iterSub != wraperSmart->m_subscriberMap.end(); iterSub++)
						{
							SmartPtr<InfoSubscribed> sub = iterSub->second;
							//msgSmart->SetContext(contextSmart);
							sub->m_queueSmart->Push(msgSmart);
							Int ref = SULOKI_INCREMENT_SULOKI(&sub->m_ref);//sub.m_ref++;
							if (((wraperSmart->m_attrib & URC_SERIAL_ATTR) == URC_SERIAL_ATTR && ref >= MAXNUM_QUEUESERI_THREAD) || ((wraperSmart->m_attrib & URC_SERIAL_ATTR) != URC_SERIAL_ATTR && ref >= sub->m_maxNumQueuePara))//MAXNUM_QUEUEPARA_THREAD))
							{
								SULOKI_DECREMENT_SULOKI(&sub->m_ref);//sub.m_ref--;
								continue;
							}
							m_threadPool.Post(boost::bind(&Urc::CallSub, this, strUrName, sub->m_strSubName));
						}
					}
					else
					{
						SULOKI_INCREMENT_SULOKI(&wraperSmart->m_currentSubscriber);////pSwaper->m_currentSubscriber++;
						Int size = wraperSmart->m_subscriberMap.size();
						Int i = 0;
						for (std::map< std::string, SmartPtr<InfoSubscribed> >::iterator iterSub = wraperSmart->m_subscriberMap.begin(); iterSub != wraperSmart->m_subscriberMap.end(); iterSub++, i++)
						{
							//if (((*(pSwaper->m_currentSubscriberSmart)) % size) != i)
							//	continue;
							if ((wraperSmart->m_currentSubscriber % size) != i)
								continue;
							SmartPtr<InfoSubscribed> sub = iterSub->second;
							//msgSmart->SetContext(contextSmart);
							sub->m_queueSmart->Push(msgSmart);
							Int ref = SULOKI_INCREMENT_SULOKI(&sub->m_ref);//sub.m_ref++;
							if (((wraperSmart->m_attrib & URC_SERIAL_ATTR) == URC_SERIAL_ATTR && ref >= MAXNUM_QUEUESERI_THREAD) || ((wraperSmart->m_attrib & URC_SERIAL_ATTR) != URC_SERIAL_ATTR && ref >= sub->m_maxNumQueuePara))//MAXNUM_QUEUEPARA_THREAD))
							{
								SULOKI_DECREMENT_SULOKI(&sub->m_ref);//sub.m_ref--;
								continue;
							}
							m_threadPool.Post(boost::bind(&Urc::CallSub, this, strUrName, sub->m_strSubName));
						}
					}
				}
				if ((wraperSmart->m_attrib & URC_ONLYONE_ATTR) != URC_ONLYONE_ATTR)
				{
					SULOKI_DECREMENT_SULOKI(&wraperSmart->m_ref);//sub.m_ref--;
																	//????? post to a queue to continue send,forbiden miss
					if (wraperSmart->m_queueSmart->GetSize() > 0)
					{
						Int ref = SULOKI_INCREMENT_SULOKI(&wraperSmart->m_ref);//sub.m_ref++;
						if (ref >= MAXNUM_QUEUESERI_THREAD)
						{
							SULOKI_DECREMENT_SULOKI(&wraperSmart->m_ref);//sub.m_ref--;
						}
						else
						{
							m_threadPool.Post(boost::bind(&Urc::CallSubMain, this, strUrName));
						}
					}
				}
#else
#endif
			}
			return;
		}
	public:
		//????? post a message with two thread, is slow
		Ret PostMsgSubUr(std::string strUrName, SmartPtr<Message> msgSmart, SmartPtr<Context> contextSmart)
		{
			if (!(strUrName.length() > 0 && strUrName.length() <= SULOKI_MAXLEN_URNAME_SULOKIDEF))
				return INVALIDPARA_ERRCODE;
			if (msgSmart.IsNULL())
				return INVALIDPARA_ERRCODE;
			msgSmart->SetContext(contextSmart);
			//Wraper* pSwaper = NULL;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
			SmartPtr<Wraper> wraperSmart;
			{
				Int hashId = Hash(strUrName.c_str(), strUrName.length(), MAXNUM_SLOT);
				ReadLock lock(m_rwLock[hashId]);
				std::map<std::string, void*>::iterator iter = m_urcMap[hashId].find(strUrName);
				if (iter == m_urcMap[hashId].end())
					return FAIL;//URC_UNEXISTED_ERRORCODE;
				Wraper* pSwaper = (Wraper*)(iter->second);
				if (pSwaper->m_type != URC_MSGQUEUE_TYPE)
					return FAIL;//INVALIDSTATE_ERRCODE;
				wraperSmart = SmartPtr<Wraper>(pSwaper);
			}
			//WraperObject< SmartPtr<Queue<Message>> >* pWraperObject = (WraperObject< SmartPtr<Queue<Message>> >*)(iter->second);
			//pWraperObject->m_val->Push(msgSmart);
			wraperSmart->m_queueSmart->Push(msgSmart);
			Int ref = SULOKI_INCREMENT_SULOKI(&wraperSmart->m_ref);//sub.m_ref++;
			if (ref >= MAXNUM_QUEUESERI_THREAD)
			{
				SULOKI_DECREMENT_SULOKI(&wraperSmart->m_ref);//sub.m_ref--;
			}
			else
			{
				m_threadPool.Post(boost::bind(&Urc::CallSubMain, this, strUrName));
			}
			return SUCCESS;
#else
#endif
		}
		Ret PostMsgToMainService(SmartPtr<Message> msgSmart, SmartPtr<Context> contextSmart)
		{
			std::stringstream strStream;
			strStream << URC_SYSTEM_NAMEPREFIX << URC_MAINGROUP_SERVICE << "_" << URC_MAINSERVICE_SERVICE << URC_PARAQUEUE_NAMEKEY;
			return PostMsgSubUr(strStream.str(), msgSmart, contextSmart);
		}
		Ret SendMsgToMainService(SmartPtr<Message> msgSmart, SmartPtr<Context> contextSmart, SmartPtr<Message>& resmsgSmart, SmartPtr<Context>& rescontextSmart)
		{
			std::stringstream strStream;
			strStream << URC_SYSTEM_NAMEPREFIX << URC_MAINGROUP_SERVICE << "_" << URC_MAINSERVICE_SERVICE;
			SmartPtr< Base > baseSmart;
			if (GetUr(strStream.str(), baseSmart) == SUCCESS)
			{
				SmartPtr< Service > serviceSmart = baseSmart.Upcast<Service>();
				//SmartPtr<Message> resSmart;
				//SmartPtr<Context> paraOutSmart;
				return serviceSmart->Handle(msgSmart, contextSmart, resmsgSmart, rescontextSmart);
			}
			return FAIL;
		}
	protected:
		/*
		Ret Urc::ReqresMsgToUrcserver(std::string strUrName, SulokiMessage& msg, Int timeout, std::string strObjName)
		{
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		return URC_INVALIDPATH_ERRORCODE;
		{//sync
		if (strObjName == "")
		{
		std::vector<std::string> nameVector;
		if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
		{
		std::string strUrcServerAddr;
		ConfigSingleton::Instance().GetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, strUrcServerAddr);
		size_t pos = strUrcServerAddr.find(":");
		std::string urcServerIp = strUrcServerAddr.substr(0, pos);
		std::string urcServerPort = strUrcServerAddr.substr(pos + 1);
		std::stringstream strStream;
		strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
		if (ConnAndReg(strStream.str(), urcServerIp, atoi(urcServerPort.c_str())) != SUCCESS)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
		return FAIL;
		}
		std::vector<std::string> nameVector;
		if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
		return FAIL;
		}
		}
		strObjName = nameVector[0];
		}
		//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
		//{
		//	std::cout << "get connect to urc server fail" << std::endl;
		//	return FAIL;
		//}
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(GetUr_Urcsys(strObjName, baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
		return FAIL;
		}
		boost::shared_ptr<UrcTcpConnection> connSmartPtr = boost::dynamic_pointer_cast<UrcTcpConnection>(baseSmartPtr);
		if (connSmartPtr.get() == NULL)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server1 fail";
		return FAIL;
		}
		std::string strMsg;
		SulokiProtoSwrap::EncodeProtocol(msg, strMsg);
		//
		Int id = EventManagerSingleton::Instance().GetFreeEvent();
		if (id < 0)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "get GetFreeEvent fail";
		return FAIL;
		}
		std::stringstream strStream;
		if (msg.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
		strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.routers(msg.routers_size() - 1) << "_" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
		else
		strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
		//boost::function<void(Int, Uint, bool)> func = boost::bind(&Urc::FuncRes, this, id, _1, _1);
		//SulokiContext context;
		//context.set_id(id);
		std::auto_ptr<SulokiContext> contextSmart(new SulokiContext());
		if (contextSmart.get() == NULL)
		return FAIL;
		#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(contextSmart.get(), "");
		#endif
		contextSmart->set_id(id);
		//?????
		//SulokiMessage* pMsgBack = contextSmart->mutable_msgori();
		//if (pMsgBack == NULL)
		//	return FAIL;
		//SulokiProtoSwrap::MakeSimpleCopy(msg, *pMsgBack);
		AsyncNewFunc func(BindFirst(AsyncFunc(this, &Urc::FuncRes), contextSmart.release()));
		if (AddUrIn(strStream.str(), func, SULOKI_EVENT_TYPE_URC_BASE, 0, false, -1, Loki::Type2Type<AsyncNewFunc>()) != SUCCESS)
		{
		EventManagerSingleton::Instance().FreeEvent(id);
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUrIn fail";
		return FAIL;
		}
		//
		connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
		std::auto_ptr<SulokiMessage> resSmart;
		if (EventManagerSingleton::Instance().Wait(id, strStream.str(), timeout, resSmart) != SUCCESS)
		return TIMEOUT_ERRORCODE;
		msg = *resSmart;
		if (!resSmart->has_errorcode())
		return FAIL;
		return resSmart->errorcode();
		}
		return URC_INVALIDPATH_ERRORCODE;
		}
		Ret Urc::ReqresMsgToUrcserver(std::string strUrName, SulokiMessage& msg, Int timeout, AsyncNewFunc asyncCb, std::string strObjName)
		{
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		return URC_INVALIDPATH_ERRORCODE;
		{//async
		if (strObjName == "")
		{
		std::vector<std::string> nameVector;
		if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
		{
		std::string strUrcServerAddr;
		ConfigSingleton::Instance().GetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, strUrcServerAddr);
		size_t pos = strUrcServerAddr.find(":");
		std::string urcServerIp = strUrcServerAddr.substr(0, pos);
		std::string urcServerPort = strUrcServerAddr.substr(pos + 1);
		std::stringstream strStream;
		strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
		if (ConnAndReg(strStream.str(), urcServerIp, atoi(urcServerPort.c_str())) != SUCCESS)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
		return FAIL;
		}
		std::vector<std::string> nameVector;
		if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
		return FAIL;
		}
		}
		strObjName = nameVector[0];
		}
		//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
		//{
		//	SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
		//	return FAIL;
		//}
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(GetUr_Urcsys(strObjName, baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
		return FAIL;
		}
		boost::shared_ptr<UrcTcpConnection> connSmartPtr = boost::dynamic_pointer_cast<UrcTcpConnection>(baseSmartPtr);
		if (connSmartPtr.get() == NULL)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server1 fail";
		return FAIL;
		}
		std::string strMsg;
		SulokiProtoSwrap::EncodeProtocol(msg, strMsg);
		//
		std::stringstream strStream;
		if (msg.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
		strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.routers(msg.routers_size() - 1) << "_" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
		else
		strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
		//boost::function<void(Int, Uint, bool)> func = boost::bind(&Urc::FuncRes, this, id, _1, _1);
		if (AddUrIn(strStream.str(), asyncCb, SULOKI_EVENT_TYPE_URC_BASE, 0, false, -1, Loki::Type2Type<AsyncNewFunc>()) != SUCCESS)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUrIn fail";
		return FAIL;
		}
		//
		if (EventManagerSingleton::Instance().AsyncTimer(timeout, strStream.str()) != SUCCESS)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AsyncTimer fail";
		//boost::function<void(Uint, bool)> func;
		AsyncNewFunc func;
		if (DelUr_Urcsys(strStream.str(), func) != SUCCESS)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "DelUr_Urcsys fail, urc name:" << strStream.str();
		}
		return FAIL;
		}
		//
		connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
		return SUCCESS;
		}
		return URC_INVALIDPATH_ERRORCODE;
		}
		Ret Urc::NotifyMsgToUrcserver(std::string strUrName, SulokiMessage& msg, std::string strObjName)
		{
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		return URC_INVALIDPATH_ERRORCODE;
		{//sync
		if (strObjName == "")
		{
		std::vector<std::string> nameVector;
		if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
		{
		std::string strUrcServerAddr;
		ConfigSingleton::Instance().GetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, strUrcServerAddr);
		size_t pos = strUrcServerAddr.find(":");
		std::string urcServerIp = strUrcServerAddr.substr(0, pos);
		std::string urcServerPort = strUrcServerAddr.substr(pos + 1);
		std::stringstream strStream;
		strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
		if (ConnAndReg(strStream.str(), urcServerIp, atoi(urcServerPort.c_str())) != SUCCESS)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
		return FAIL;
		}
		std::vector<std::string> nameVector;
		if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
		return FAIL;
		}
		}
		strObjName = nameVector[0];
		}
		//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
		//{
		//	std::cout << "get connect to urc server fail" << std::endl;
		//	return FAIL;
		//}
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(GetUr_Urcsys(strObjName, baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
		return FAIL;
		}
		boost::shared_ptr<UrcTcpConnection> connSmartPtr = boost::dynamic_pointer_cast<UrcTcpConnection>(baseSmartPtr);
		if (connSmartPtr.get() == NULL)
		{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server1 fail";
		return FAIL;
		}
		std::string strMsg;
		SulokiProtoSwrap::EncodeProtocol(msg, strMsg);
		//
		connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
		return SUCCESS;
		}
		return URC_INVALIDPATH_ERRORCODE;
		}
		*/
	protected:
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
#else
#endif
	protected:
		static Ret AddDir(std::string& strKey, std::string& strIndex)
		{
			char cName[SULOKI_MAXLEN_URNAME_SULOKIDEF + 2];
			memset(cName, 0, SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
			sprintf(cName, "%s", strKey.c_str());
			//
			Int inxFound = 0;
			Ret ret = FindDir(strKey, strIndex, 0, strIndex.length() / (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2) - 1, inxFound);
			if (ret == SUCCESS)
				return FAIL;
			if (inxFound >= strIndex.length() / (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2))
			{
				strIndex.append(cName, SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
			}
			else
			{
				strIndex.insert((SULOKI_MAXLEN_URNAME_SULOKIDEF + 2)*inxFound, cName, SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
			}
			return SUCCESS;
		}
		static Ret DelDir(std::string& strKey, std::string& strIndex)
		{
			Int inxFound = 0;
			Ret ret = FindDir(strKey, strIndex, 0, strIndex.length() / (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2) - 1, inxFound);
			if (ret == SUCCESS)
			{
				strIndex.erase((SULOKI_MAXLEN_URNAME_SULOKIDEF + 2)*inxFound, SULOKI_MAXLEN_URNAME_SULOKIDEF + 2);
			}
			return ret;
		}
		static Ret FindDir(std::string& strKey, std::string& strIndex, Int from, Int to, Int& inxFound)
		{
			/*
			if (strIndex.length() / (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2) == 0)
			{
			inxFound = 0;
			return FAIL;
			}
			if (from == to)
			{
			Int res = strcmp(strKey.c_str(), strIndex.c_str() + (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2)*from);
			if (res == 0)
			{
			inxFound = from;
			return SUCCESS;
			}
			else
			if (res < 0)
			{
			inxFound = from;
			return FAIL;
			}
			else
			{
			inxFound = from + 1;
			return FAIL;
			}
			}
			Int newTo = from + (to - from + 1) / 2 - 1;
			if (strcmp(strKey.c_str(), strIndex.c_str() + (SULOKI_MAXLEN_URNAME_SULOKIDEF + 2)*newTo) <= 0)
			return FindDir(strKey, strIndex, from, newTo, inxFound);
			else
			return FindDir(strKey, strIndex, newTo + 1, to, inxFound);
			*/
			return FindData(strKey, (Int8*)strIndex.c_str(), strIndex.length(), from, to, inxFound);
		}
		static Ret FindData(std::string& strKey, Int8* pData, Int dataLen, Int from, Int to, Int& inxFound, Int offset = SULOKI_MAXLEN_URNAME_SULOKIDEF + 2)
		{
			if (dataLen / (offset) == 0)
			{
				inxFound = 0;
				return FAIL;
			}
			if (from == to)
			{
				Int res = strcmp(strKey.c_str(), (char*)pData + (offset)*from);
				if (res == 0)
				{
					inxFound = from;
					return SUCCESS;
				}
				else
					if (res < 0)
					{
						inxFound = from;
						return FAIL;
					}
					else
					{
						inxFound = from + 1;
						return FAIL;
					}
			}
			Int newTo = from + (to - from + 1) / 2 - 1;
			if (strcmp(strKey.c_str(), (char*)pData + (offset)*newTo) <= 0)
				return FindData(strKey, pData, dataLen, from, newTo, inxFound, offset);
			else
				return FindData(strKey, pData, dataLen, newTo + 1, to, inxFound, offset);
		}
		static Int Hash(const char* pData, Int dataLen, Uint slotNum)
		{
			return HashIn(pData, dataLen, 0xbc9f1d34) % slotNum;
		}
		static Uint HashIn(const char* pData, Int dataLen, Uint seed)
		{
			const Uint m = 0xc6a4a793;
			const Uint r = 24;
			const char* limit = pData + dataLen;
			Uint h = seed ^ (dataLen * m);

			while (pData + sizeof(Uint) <= limit) {
				Uint w = *((Uint*)pData);
				pData += sizeof(Uint);
				h += w;
				h *= m;
				h ^= (h >> (sizeof(Uint) / 2 * 8));
			}

			switch (limit - pData) {
			case 7:
				h += pData[6] << 48;
			case 6:
				h += pData[5] << 40;
			case 5:
				h += pData[4] << 32;
			case 4:
				h += pData[3] << 24;
			case 3:
				h += pData[2] << 16;
			case 2:
				h += pData[1] << 8;
			case 1:
				h += pData[0];
				h *= m;
				h ^= (h >> r);
				break;
			}
			return h;
		}
	private:
		Urc(Urc& ref) {}
		Urc& operator=(Urc& ref) { return *this; }
	private:
		ThreadPool m_threadPool;
#if (defined SULOKI_OPENSOURCE_VERSION_SULOKIDEF)
		LockPolicy m_rwLock[MAXNUM_SLOT];
		std::map<std::string, void*> m_urcMap[MAXNUM_SLOT];
#else
#endif
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
		leveldb::DB* m_pDb;
#endif
		friend class ServiceManager;
	};
	typedef Singleton< Urc > UrcSingleton;
#ifdef SULOKI_DUMP_SULOKIDEF
	class Dump : public Base
	{
	public:
		Dump();
		explicit Dump(std::string strDumpName);
		virtual ~Dump();
		static Ret TestDump(void);
	private:
		Dump(Dump& ref) {}
		Dump& operator=(Dump& ref) { return *this; }
	private:
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		LPTOP_LEVEL_EXCEPTION_FILTER m_oldFilter;
		_invalid_parameter_handler m_oldHandler;
		_purecall_handler m_oldPurecallHandler;
#else
#endif
	};
	typedef Singleton< Dump > DumpSingleton;
#endif
	//tool
	class Tool : public Base
	{
	public:
		static Ret GetMd5(const char  * const buffer, size_t buffer_size, std::string &str_md5)
		{
			if (buffer == nullptr)
				return FAIL;
			boost::uuids::detail::md5 boost_md5;
			boost_md5.process_bytes(buffer, buffer_size);
			boost::uuids::detail::md5::digest_type digest;
			boost_md5.get_digest(digest);
			const auto char_digest = reinterpret_cast<const char*>(&digest);
			str_md5.clear();
			boost::algorithm::hex(char_digest, char_digest + sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(str_md5));
			return SUCCESS;
		}
		static Ret EncodeBase64(SmartPtr< Data >& dataOriSmart, SmartPtr< Data >& dataDestSmart, bool bEncoded = true)
		{
			if (dataOriSmart.IsNULL())
				return FAIL;
			Int dataLen = 0;
			const Int8* pData = dataOriSmart->Get(&dataLen);
			std::string outPut;
			if (bEncoded)
			{
				std::string inPut(pData, dataLen);
				{
					typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8>> Base64EncodeIter;

					std::stringstream  result;
					copy(Base64EncodeIter(inPut.begin()),
						Base64EncodeIter(inPut.end()),
						std::ostream_iterator<char>(result));

					size_t Num = (3 - inPut.length() % 3) % 3;
					for (size_t i = 0; i < Num; i++)
					{
						result.put('=');
					}
					outPut = result.str();
					if (outPut.empty())
						return FAIL;
				}
			}
			else
			{
				std::string inPut(pData, dataLen);
				{
					typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIter;

					std::stringstream result;
					try
					{
						copy(Base64DecodeIter(inPut.begin()),
							Base64DecodeIter(inPut.end()),
							std::ostream_iterator<char>(result));
					}
					catch (...)
					{
						return FAIL;
					}
					outPut = result.str();
					if (outPut.empty())
						return FAIL;
				}
			}
			dataDestSmart = DataStaticCreater::Create((Int8*)outPut.c_str(), (Int)outPut.length());
			return SUCCESS;
		}
	};

	//app statemachine
	class AppStateMachine : public Base, public StateMachine
	{
	public:
		AppStateMachine();
		virtual ~AppStateMachine();
		virtual Ret Init(void);
		virtual Ret Start(void);
		virtual Ret Run(void);
		virtual Ret Stop(void);
		virtual Ret Clear(void);
	public:
		virtual Int GetState(void)
		{
			return GlobalStateMachineSingleton::Instance().GetState();
		}
	protected:
		virtual void SetState(Int state)
		{
			return GlobalStateMachineSingleton::Instance().SetState(state);;
		}
	private:
		AppStateMachine(AppStateMachine& ref) {}
		AppStateMachine& operator=(AppStateMachine& ref) { return *this; }
	private:
	};
}//end namespace Suloki

#endif
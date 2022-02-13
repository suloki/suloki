#ifndef _PACKER_H_
#define _PACKER_H_

#include "suloki.h"

namespace SulokiNet
{
	const Suloki::Int FLAGLENGTH = 8;
	const std::string BLOCK_FALG = "sulokip";
	const Suloki::Int MAXMSGLENGTH = 1024*1024*64;
#pragma pack(push)
#pragma pack(1)
	typedef struct {
		char m_cFlag[FLAGLENGTH];
		Suloki::Int64 m_messageId;
		Suloki::Int32 m_blockId;
		Suloki::Int32 m_messageSize;
	}MsgHead;
#pragma pack(pop)
	//const Suloki::Int MAXBLOCK_SIZE = 1436;// 1450;
	const Suloki::Int BLOCK_SIZE=1400;
	class Packer : public Suloki::Base
	{
		//typedef struct{
		//	char cBuf[MAXBLOCK_SIZE-BLOCK_SIZE];
		//}StruHead;
	public:
		Packer();
		virtual ~Packer();
        void Clear(void);
		Suloki::Ret Encode(const char* pMsg, Suloki::Int msgLen);
		Suloki::Ret PopBlock(Suloki::SmartPtr< Suloki::Data >& dataSmart);
		Suloki::Ret Decode(const char* pBlock, Suloki::Int blockLen);
		Suloki::Ret PopMsg(Suloki::SmartPtr< Suloki::Data >& msgSmart);
	protected:
		inline static Suloki::Int GetUrId(void);
	private:
		Packer(Packer& ref) {}
		Packer& operator=(Packer& ref) { return *this; }
	private:
		std::string m_myUrId;
		Suloki::Int64 m_msgId;
		Suloki::Queue< Suloki::Data > m_blockQueueEncoded;
		Suloki::Queue< Suloki::Data > m_msgQueueDecoded;
		//std::map<Suloki::Int64, Suloki::Data*> m_map;
        //std::auto_ptr< Suloki::Data > m_cacheSmart;
        //std::auto_ptr< Suloki::Data > m_cacheSmartTmp;
		volatile static Suloki::Int m_urId;
	};

}

#endif

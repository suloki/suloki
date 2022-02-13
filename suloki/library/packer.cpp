#include "packer.h"
#include "udp.h"

namespace SulokiNet
{
	volatile Suloki::Int Packer::m_urId = 0;
	Packer::Packer():m_msgId(0)
	{
		Suloki::Int urId = GetUrId();
		std::stringstream strStream;
		strStream << urId << "/";
		m_myUrId = atoll(strStream.str().c_str());
	}
	Packer::~Packer()
	{
		//?????
	}
	void Packer::Clear(void)
	{
		m_msgId = 0;
		m_blockQueueEncoded.Clear();
		m_msgQueueDecoded.Clear();
	}
	Suloki::Ret Packer::Encode(const char* pMsg, Suloki::Int msgLen)
	{
		if(!(pMsg != NULL && msgLen > 0 && msgLen <= MAXMSGLENGTH))
			return Suloki::INVALIDPARA_ERRCODE;
		//
        Suloki::Int blockNum = (msgLen - msgLen%BLOCK_SIZE) / BLOCK_SIZE;
		Suloki::Int lastBlockSize = msgLen%BLOCK_SIZE;
		if(lastBlockSize > 0)
			blockNum++;
		//
		Suloki::Int msgId = SULOKI_INCREMENT_SULOKIDEF(&m_msgId);
		Suloki::Int blockId = 0;
		//
		MsgHead head;
		strcpy(head.m_cFlag, BLOCK_FALG.c_str());
		//
		for(Suloki::Int i=0;i<blockNum;i++, blockId++)
		{
			Suloki::Int blockSize = BLOCK_SIZE;
			if (i == blockNum - 1 && lastBlockSize > 0)
				blockSize = lastBlockSize;
			Suloki::SmartPtr< Suloki::Data > blockSmart = Suloki::DataStaticCreater::Create(sizeof(MsgHead)+ blockSize);
			if (blockSmart.get() == NULL)
			{
				SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
				return Suloki::NOMEMORY_ERRCODE;
			}
			head.m_messageId = msgId;
			head.m_blockId = blockId;
			head.m_messageSize = msgLen;
			Suloki::Int dataLen = 0;
			Suloki::Int8* pData = (Suloki::Int8*)blockSmart->Get(&dataLen);
			memcpy(pData, &head, sizeof(MsgHead));
			memcpy(pData + sizeof(MsgHead), pMsg + BLOCK_SIZE*i, blockSize);
			m_blockQueueEncoded.Push(blockSmart);
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret Packer::PopBlock(Suloki::SmartPtr< Suloki::Data >& dataSmart)
	{
		dataSmart = m_blockQueueEncoded.Pop();
		if(dataSmart.get() == NULL)
			return Suloki::FAIL;
		return Suloki::SUCCESS;
	}
	Suloki::Ret Packer::Decode(const char* pBlock, Suloki::Int blockLen)
	{
		if(!(pBlock != NULL && blockLen > 0 && blockLen <= sizeof(MsgHead)+ BLOCK_SIZE))
			return Suloki::INVALIDPARA_ERRCODE;
		if(strstr(pBlock, BLOCK_FALG.c_str()) != pBlock)
			return Suloki::INVALIDPARA_ERRCODE;
		//
		MsgHead* pMsgHead = (MsgHead*)pBlock;
		if(pMsgHead->m_messageSize > MAXMSGLENGTH)
			return Suloki::INVALIDPARA_ERRCODE;
		//
		{
			std::stringstream strStream;
			strStream << Suloki::URC_SYSTEM_PACKER_NAMEPREFIX << m_myUrId << pMsgHead->m_messageId;
			std::string strVal;
			if (Suloki::UrcSingleton::Instance().GetUr(strStream.str(), strVal) != Suloki::SUCCESS)
			{
				std::stringstream strValStream;
				strValStream << pMsgHead->m_messageSize;
				if (Suloki::UrcSingleton::Instance().AddUr(strStream.str(), strValStream.str(), Suloki::URC_DATA_TYPE, 0, true, 0) != Suloki::SUCCESS)
				{
					return Suloki::FAIL;
				}
			}
		}
		//
		{
			std::stringstream strStream;
			strStream << Suloki::URC_SYSTEM_PACKER_NAMEPREFIX << m_myUrId << pMsgHead->m_messageId << "/" << pMsgHead->m_blockId;
			std::string strVal;
			if (Suloki::UrcSingleton::Instance().GetUr(strStream.str(), strVal) != Suloki::SUCCESS)
			{
				std::string strBlock(pBlock, blockLen);
				if (Suloki::UrcSingleton::Instance().AddUr(strStream.str(), strBlock, Suloki::URC_DATA_TYPE, 0, true, 0) != Suloki::SUCCESS)
				{
					return Suloki::FAIL;
				}
			}
		}
		//
		std::vector<std::string> msgidVector;
		std::stringstream strStream;
		strStream << Suloki::URC_SYSTEM_PACKER_NAMEPREFIX << m_myUrId;
		if (Suloki::UrcSingleton::Instance().GetUr(strStream.str(), msgidVector) != Suloki::SUCCESS)
		{
			return Suloki::FAIL;
		}
		for (std::vector<std::string>::iterator iter = msgidVector.begin(); iter != msgidVector.end(); iter++)
		{
			std::string strUrMsgName = *iter;
			size_t pos = strUrMsgName.rfind("/");
			strUrMsgName = strUrMsgName.substr(0, pos + 1);
			strUrMsgName = strStream.str() + strUrMsgName;// +"/";
			//
			std::string strMsgSize;
			if (Suloki::UrcSingleton::Instance().GetUr(strUrMsgName, strMsgSize) != Suloki::SUCCESS)
			{
				continue;
			}
			Suloki::Int msgSize = atoi(strMsgSize.c_str());
			Suloki::Int blockNum = (msgSize - msgSize%BLOCK_SIZE) / BLOCK_SIZE;
			Suloki::Int lastBlockSize = msgSize%BLOCK_SIZE;
			if (lastBlockSize > 0)
				blockNum++;
			//
			std::string strUrMsgPath = strUrMsgName + "/";
			std::vector<std::string> blockidVector;
			if (Suloki::UrcSingleton::Instance().GetUr(strUrMsgPath, blockidVector) != Suloki::SUCCESS)
			{
				continue;
			}
			if (blockidVector.size()*BLOCK_SIZE != blockNum)
			{
				continue;
			}
			//
			Suloki::SmartPtr< Suloki::Data > msgSmart = Suloki::DataStaticCreater::Create(msgSize);
			if (msgSmart.get() == NULL)
			{
				continue;
			}
			Suloki::Int8* pMsg = (Suloki::Int8*)msgSmart->Get();
			//
			bool bOk = true;
			Suloki::Int blockId = 0;
			for (std::vector<std::string>::iterator iterBlock = blockidVector.begin(); iterBlock != blockidVector.end(); iterBlock++, blockId++)
			{
				std::string bloclKey = *iterBlock;
				std::string strBlockVal;
				if (Suloki::UrcSingleton::Instance().DelUr(bloclKey, strBlockVal) != Suloki::SUCCESS)
				{
					bOk = false;
					continue;
				}
				MsgHead msgHead;
				memcpy(&msgHead, strBlockVal.c_str(), sizeof(MsgHead));
				if(msgHead.m_blockId != blockId)
				{
					bOk = false;
					continue;
				}
				Suloki::Int blockSize = BLOCK_SIZE;
				if (blockId == blockNum - 1 && lastBlockSize > 0)
					blockSize = lastBlockSize;
				memcpy(pMsg + BLOCK_SIZE*blockId, strBlockVal.c_str()+sizeof(MsgHead), blockSize);
			}
			if (!bOk)
				continue;
			m_msgQueueDecoded.Push(msgSmart);
		}
		return Suloki::SUCCESS;
	}
	Suloki::Ret Packer::PopMsg(Suloki::SmartPtr< Suloki::Data >& msgSmart)
	{
		msgSmart = m_msgQueueDecoded.Pop();
		if(msgSmart.get() == NULL)
			return Suloki::FAIL;
		return Suloki::SUCCESS;
	}
	Suloki::Int Packer::GetUrId(void)
	{
		return SULOKI_INCREMENT_SULOKIDEF(&m_urId);;
	}
}

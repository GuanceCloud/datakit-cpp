#pragma once
#ifndef _DATAKIT_LOG_PIPER_H_
#define _DATAKIT_LOG_PIPER_H_

#include "Singleton.h"
#include "AbstractManager.h"
#include <string>
#include "Include/FTSDKConfig.h"

namespace com::ft::sdk::internal
{
	class LogPipeManager
		: public AbstractManager, public Singleton<LogPipeManager>
	{
	public:
		void init(FTLogConfig& config);
		void addLog(std::string content, LogLevel level);

	private:
		bool checkLogLevel(LogLevel level);

	private:
		FTLogConfig* m_pLogPipeConfig;

		const int LOG_LIMIT_SIZE = 30720;
	};

}

#endif // !_DATAKIT_LOG_PIPER_H_


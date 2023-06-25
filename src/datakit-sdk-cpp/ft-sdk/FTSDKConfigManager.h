/*****************************************************************//**
 * \file   FTSDKConfigManager.h
 * \brief  The unified entry to manage all the configs for SDK
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_CONFIG_MANAGER_H_
#define _DATAKIT_SDK_CONFIG_MANAGER_H_

#include "Include/FTSDKConfig.h"
#include "Singleton.h"
#include "Include/FTSDK.h"
#include "InternalEnums.h"
#include <vector>

namespace com::ft::sdk::internal
{
	class FTTestConfig
	{
	public:
		bool isOfflineMode()
		{
			return enableOfflineMode;
		}
		void setOfflineMode(bool enabled)
		{
			enableOfflineMode = enabled;
		}

		HTTP_STATUS getHttpStatus()
		{
			return httpStatus;
		}
		void setHttpStatus(HTTP_STATUS status)
		{
			httpStatus = status;
		}

	private:
		bool enableOfflineMode = false;
		HTTP_STATUS httpStatus = HTTP_STATUS::HTTP_OK;
	};

	class FTSDKConfigManager : public Singleton<FTSDKConfigManager>
	{
	public:
		FTSDKConfigManager&& setGeneralConfig(FTSDKConfig& config);

		FTSDKConfigManager&& setRUMConfig(FTRUMConfig& config);
		//FTSDKConfigManager&& setHttpConfig(HttpConfig& config);
		FTSDKConfigManager&& setTraceConfig(FTTraceConfig& config);
		FTSDKConfigManager&& setLogPipeConfig(FTLogConfig& config);
		FTSDKConfigManager&& enableRUMUserBinding(bool enabled);

		FTSDKConfigManager&& bindUserData(UserData& config);
		FTSDKConfigManager&& unbindUserData();		
		
		void initRUMGlobalContext(FTRUMConfig& config);
		void initGlobalContext(FTSDKConfig& config);

		std::map<std::string, std::string> getRUMPublicDynamicTags(bool includeRUMStatic = false);

		FTSDKConfig& getGeneralConfig()
		{
			return m_generalConfig;
		}		
		UserData& getUserData()
		{
			return m_userData;
		}
		FTRUMConfig& getRUMConfig()
		{
			return m_rumConfig;
		}
		//HttpConfig& getHttpConfig()
		//{
		//	return m_httpConfig;
		//}
		FTTraceConfig& getTraceConfig()
		{
			return m_traceConfig;
		}
		FTLogConfig& getLogPipeConfig()
		{
			return m_logPipeConfig;
		}
		FTTestConfig& getTestConfig()
		{
			return m_testConfig;
		}

		bool isUserDataBinded()
		{
			return m_isUserBinded && getUserData().getId() != "";
		}

		void setConfigFileName(const std::string& path)
		{
			m_configFilePath = m_workingDir + "/" + path;
		}

		std::string getConfigFilePath()
		{
			return m_configFilePath;
		}
		std::string getWorkingDir()
		{
			return m_workingDir;
		}

		void setSDKInited(bool inited)
		{
			m_isSDKInited = inited;
		}
		bool isSDKInited()
		{
			return m_isSDKInited;
		}
	private:
		FTSDKConfigManager();

		bool isDefaultAppVersion();

	private:
		bool m_isSDKInited = false;
		FTSDKConfig m_generalConfig;
		UserData m_userData;
		FTRUMConfig m_rumConfig;
		//HttpConfig m_httpConfig;
		FTTraceConfig m_traceConfig;
		FTLogConfig m_logPipeConfig;
		FTTestConfig m_testConfig;

		//设置全局 tag
		std::map<std::string, std::string> m_mapGlobalContext;

		bool m_isUserBinded = false;

		std::string m_configFilePath;
		std::string m_workingDir;

		ENABLE_SINGLETON();
	};

}

#endif // _DATAKIT_SDK_CONFIG_MANAGER_H_
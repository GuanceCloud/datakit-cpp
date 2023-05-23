#pragma once
#ifndef _DATAKIT_SDK_COMM_MANAGER_H_
#define _DATAKIT_SDK_COMM_MANAGER_H_

#include <map>
#include <string>
#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"
#include "Include/FTSDKConfig.h"
#include "InternalEnums.h"

namespace com::ft::sdk::internal
{
	struct ResponseData
	{
		int code;
		std::string message;
	};

	class CommunicationManager
	{
	private:
		static CommunicationManager* pInstance;
		CommunicationManager() {}
		std::string calculateDate();

	public:
		static CommunicationManager& instance();
		void initialize(FTSDKConfig& config);
		void deinitialize();

		void addHeadParam(const std::string& key, const std::string& value, bool overwrite = true)
		{
			if (overwrite || m_headParams.find(key) == m_headParams.end())
			{
				m_headParams[key] = value;
			}
		}
		void setHeadParam();
		ResponseData post(DataType dt, std::string& data);

	private:
		FTSDKConfig m_generalConfig;
		RestClient::Connection* m_pConnection = nullptr;

		std::map<std::string, std::string> m_headParams;
	};

}

#endif // _DATAKIT_SDK_COMM_MANAGER_H_
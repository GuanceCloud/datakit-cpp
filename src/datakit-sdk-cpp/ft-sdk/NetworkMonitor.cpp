#include "pch.h"
#include "NetworkMonitor.h"
#include <thread>
#include "FTSDKError.h"
#include "FTSDKConfigManager.h"
#include "CommunicationManager.h"
#include "LoggerManager.h"
#include "DataSyncManager.h"
#include "FTSDKConstants.h"

namespace com::ft::sdk::internal
{
	NetworkMonitor::NetworkMonitor()
	{

	}

	void NetworkMonitor::init()
	{
		new std::thread(&NetworkMonitor::checkNetwork, this);
	}

	void NetworkMonitor::deinit()
	{
		m_stopping = true;
	}

	/**
	 * Check if datakit agent is available by getting the monitor page every minute.
	 * 
	 */
	void NetworkMonitor::checkNetwork()
	{
		BEGIN_THREAD();

		while (!m_stopping)
		{
			auto res = CommunicationManager::getInstance().get(constants::URL_MONITOR);
			if (res.code == HTTP_STATUS::HTTP_OK)
			{
				updateNetworkState(NetworkState::Available);
			}
			else
			{
				updateNetworkState(NetworkState::Unavailable);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(60000));
		}

		internal::LoggerManager::getInstance().logInfo("Network Monitor Threading exited.");

		END_THREAD();
	}

	void NetworkMonitor::updateNetworkState(NetworkState state)
	{
		if (m_state != state)
		{
			LoggerManager::getInstance().logInfo("Network State Changed: {} => {}", (int)m_state, (int)state);
			m_state = state;

			if (m_state == NetworkState::Available)
			{
				DataSyncManager::getInstance().notifyNewLinefromDB();
			}
		}
	}
}
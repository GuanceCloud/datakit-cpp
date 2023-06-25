/*****************************************************************//**
 * \file   NetworkMonitor.h
 * \brief  Monitor the network status
 * 
 * \author Zhou Guangyong
 * \date   June 2023
 *********************************************************************/

#pragma once
#ifndef _DATAKIT_SDK_NETOWRK_MONITOR_H_
#define _DATAKIT_SDK_NETOWRK_MONITOR_H_

#include "Singleton.h"

namespace com::ft::sdk::internal
{
	enum class NetworkState : unsigned char
	{
		Available,
		Unavailable
	};

	class NetworkMonitor : public Singleton<NetworkMonitor>
	{
	public:
		void init();
		void deinit();

		NetworkState getState()
		{
			return m_state;
		}

	private:
		NetworkMonitor();
		void checkNetwork();
		void updateNetworkState(NetworkState state);

	private:
		bool m_stopping = false;
		NetworkState m_state = NetworkState::Available;

		ENABLE_SINGLETON();
	};

}
#endif // !_DATAKIT_SDK_NETOWRK_MONITOR_H_




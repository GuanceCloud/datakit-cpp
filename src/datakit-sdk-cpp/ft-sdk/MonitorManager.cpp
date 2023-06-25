#include "pch.h"
#include "MonitorManager.h"
#include "FTSDKConfigManager.h"
#include "Platform.h"
#include "FTSDKError.h"

#include <thread>
#include <algorithm>
#include <mutex>

namespace com::ft::sdk::internal
{
	MonitoredViewContainer::MonitoredViewContainer(RUMView* pView)
	{
		m_stop = false;
		this->pView = pView;
		detectFreq = FTSDKConfigManager::getInstance().getRUMConfig().getDeviceMetricsDetectFrequency();
	}

	void MonitoredViewContainer::start()
	{
		resetMetrics();

		m_pThread = new std::thread(&MonitoredViewContainer::collectMetrics, this);
	}

	void MonitoredViewContainer::stop()
	{
		m_stop = true;

		const std::lock_guard<std::mutex> lock(m_mtx);

		// transfer the collected metrics
		//if (cpuMetrics.count > 0)
		//{
			//pView->setCpuTickCount(cpuMetrics.)
		//}

		if (memoryMetrics.count == 0)
		{
			performCollect();
		}
		if (memoryMetrics.count > 0)
		{
			pView->setMemoryAvg((std::int64_t)memoryMetrics.avgUsage);
			pView->setMemoryMax((std::int64_t)memoryMetrics.maxUsage);
		}
		resetMetrics();
	}

	void MonitoredViewContainer::collectMetrics()
	{
		const std::lock_guard<std::mutex> lock(m_mtx);

		BEGIN_THREAD();

		while (!m_stop)
		{
			performCollect();

			if (m_stop)
			{
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds((int)detectFreq));
		}

		END_THREAD();
	}

	void MonitoredViewContainer::resetMetrics()
	{
		m_stop = false;

		cpuMetrics.count = 0;
		cpuMetrics.minUsage = 0.0f;
		cpuMetrics.maxUsage = 0.0f;

		memoryMetrics.count = 0;
		memoryMetrics.avgUsage = 0.0f;
		memoryMetrics.maxUsage = 0.0f;
		memoryMetrics.minUsage = 0.0f;
	}

	void MonitoredViewContainer::performCollect()
	{
		// collect cpu usage
		//double cpuUsage = platform::getCPULoad();
		//if (cpuMetrics.count == 0) {
			//cpuMetrics.minUsage = cpuUsage;
		//}
		//else {
			//cpuMetrics.maxUsage = cpuUsage;
		//}
		//cpuMetrics.count++;

		// collect memory usage
		double memUsage = platform::getMemoryLoad();
		if (memoryMetrics.count++ == 0)
		{
			memoryMetrics.avgUsage = memUsage;
			memoryMetrics.maxUsage = memUsage;
			memoryMetrics.minUsage = memUsage;
		}
		else
		{
			int count = memoryMetrics.count + 1;
			memoryMetrics.avgUsage = (memUsage + ((double)memoryMetrics.count * memoryMetrics.avgUsage)) / (double)count;
			memoryMetrics.maxUsage = (std::max)(memUsage, memoryMetrics.maxUsage);
			memoryMetrics.minUsage = (std::min)(memUsage, memoryMetrics.minUsage);
			memoryMetrics.count = count;
		}
	}

	void MonitorManager::init()
	{
		errorMonitorType = FTSDKConfigManager::getInstance().getRUMConfig().getExtraMonitorTypeWithError();
		deviceMetricsMonitorType = FTSDKConfigManager::getInstance().getRUMConfig().getDeviceMetricsMonitorType();
	}

	bool MonitorManager::isErrorMonitorType(ErrorMonitorType errorMonitorType)
	{
		//判断某一种监控项是否开启
		return ((int)this->errorMonitorType | (int)errorMonitorType) == (int)this->errorMonitorType;
	}

	bool MonitorManager::isDeviceMetricsMonitorType(DeviceMetricsMonitorType deviceMetricsMonitorType)
	{
		return ((int)this->deviceMetricsMonitorType | (int)deviceMetricsMonitorType) == (int)this->deviceMetricsMonitorType;

	}

	void MonitorManager::addMonitor(RUMView* view)
	{
		if (deviceMetricsMonitorType == DeviceMetricsMonitorType::NO_SET)
		{
			return;
		}

		m_mapViewContainer[view->getId()] = new MonitoredViewContainer(view);

		// start monitor
		m_mapViewContainer[view->getId()]->start();
	}

	void MonitorManager::removeMonitor(const std::string viewId)
	{
		if (deviceMetricsMonitorType == DeviceMetricsMonitorType::NO_SET)
		{
			return;
		}

		if (m_mapViewContainer.find(viewId) != m_mapViewContainer.end())
		{
			MonitoredViewContainer* pContainer = m_mapViewContainer[viewId];
			// stop monitor
			pContainer->stop();

			m_mapViewContainer.erase(viewId);
		}
	}
}
#include "pch.h"
#include "Include/FTSDK.h"
#include "DataSyncManager.h"
#include "FTSDKConfigManager.h"
#include "CommunicationManager.h"
#include "LoggerManager.h"
#include "TrackNodeImpl.h"
#include "TraceManager.h"
#include "InternalStructs.h"
#include "RUMManager.h"
#include "LogPipeManager.h"
#include "Platform.h"
#include "ConfigFileHandler.h"
#include "MonitorManager.h"
#include "LineDBManager.h"
#include "CacheDBManager.h"
#include "FTSDKConstants.h"

namespace com::ft::sdk
{
	//FTSDK* FTSDK::pInstance = nullptr;

	//FTSDK& FTSDK::instance()
	//{
	//	if (pInstance == nullptr)
	//	{
	//		pInstance = new FTSDK();
	//	}

	//	return *pInstance;
	//}

	FTSDK::FTSDK(const std::string& configJson)
	{
		// load config
		internal::FTSDKConfigManager::getInstance().setConfigFileName(configJson);
		internal::ConfigFileHandler::getInstance().load();
	}

	void FTSDK::init()
	{
		internal::LoggerManager::getInstance().init();
		internal::LoggerManager::getInstance().logInfo("SDK initializing...");

		internal::LineDBManager::getInstance().init();
	}

	void FTSDK::deinit()
	{
		internal::LoggerManager::getInstance().logInfo("SDK uninitializing...");

		internal::DataSyncManager::getInstance().deinit();
		internal::CacheDBManager::getInstance().deinit();
	}

	std::string FTSDK::getVersionString()
	{
		return constants::DATAKIT_SDK_VERSION;
	}

	FTSDK&& FTSDK::install(FTSDKConfig& config)
	{
		FTSDKConfig cpConfig = config;
		if (cpConfig.getAppVersion() == "")
		{
			cpConfig.setAppVersion(constants::DEFAULT_APP_VERSION);
		}

		if (cpConfig.getServiceName() == "")
		{
			cpConfig.setServiceName(constants::DEFAULT_LOG_SERVICE_NAME);
		}

		internal::FTSDKConfigManager::getInstance().setGeneralConfig(cpConfig);
		internal::DataSyncManager::getInstance().init();
		internal::CommunicationManager::getInstance().initialize(cpConfig);

		internal::LoggerManager::getInstance().logInfo("initialized general config.");

		return std::move(*this);
	}

	FTSDK&& FTSDK::bindUserData(UserData& config)
	{
		internal::FTSDKConfigManager::getInstance().bindUserData(config);

		internal::LoggerManager::getInstance().logInfo("initialized user config.");

		return std::move(*this);
	}

	void FTSDK::unbindUserData()
	{
		internal::FTSDKConfigManager::getInstance().unbindUserData();
		internal::ConfigFileHandler::getInstance().updateConfig(internal::UpdateConfigType::user, true);
	}

	FTSDK&& FTSDK::initRUMWithConfig(FTRUMConfig& config)
	{
		internal::FTSDKConfigManager::getInstance().setRUMConfig(config);
		internal::MonitorManager::getInstance().init();
		internal::RUMManager::getInstance().init();
		internal::LoggerManager::getInstance().logInfo("initialized RUM config.");

		return std::move(*this);
	}

	FTSDK&& FTSDK::initTraceWithConfig(FTTraceConfig& config)
	{
		internal::FTSDKConfigManager::getInstance().setTraceConfig(config);

		internal::LoggerManager::getInstance().logInfo("initialized trace config.");

		internal::TraceManager::getInstance().initialize(config);

		return std::move(*this);
	}

	FTSDK&& FTSDK::initLogWithConfig(FTLogConfig& config)
	{
		internal::FTSDKConfigManager::getInstance().setLogPipeConfig(config);

		internal::LoggerManager::getInstance().logInfo("initialized Log Pipe config.");

		internal::LogPipeManager::getInstance().init(config);

		return std::move(*this);
	}

#ifdef _SUPPORT_CUSTOM_TRACK_
	void FTSDK::addCustomTrack(const std::shared_ptr<TrackNode> pTrackNodes)
	{
		auto pTN = std::dynamic_pointer_cast<internal::TrackNodeImpl>(pTrackNodes);

		internal::DataMsg msg;
		msg.dataType = DataType::RUM_APP;
		internal::Measurement& msm = msg.addMeasurement();
		msm.measurementName = pTN->getTrackName();
		
		for (auto nd : pTN->getTrackPropertyList())
		{
			msm.fields[nd.first] = nd.second;
		}
		
		auto publicTags = internal::FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
		for (auto nd : publicTags)
		{
			msm.tags[nd.first] = nd.second;
		}
		auto globalTags = internal::FTSDKConfigManager::getInstance().getRUMConfig().getGlobalContext();
		for (auto& gt : globalTags)
		{
			msm.tags[gt.first] = gt.second;
		}
		internal::DataSyncManager::getInstance().sendDataMessage(msg);
	}
#endif // _SUPPORT_CUSTOM_TRACK_

	PropagationHeader FTSDK::generateTraceHeader(const std::string urlStr)
	{
		internal::PropagationUrl url = internal::PropagationUrl::parse(urlStr);
		internal::HttpUrl internalUrl {url.getHost(), url.getPath(), url.getPort()};
		return internal::TraceManager::getInstance().getTraceHeader(internalUrl);
	}

	PropagationHeader FTSDK::generateTraceHeader(const std::string& resourceId, const std::string urlStr)
	{
		internal::PropagationUrl url = internal::PropagationUrl::parse(urlStr);
		internal::HttpUrl internalUrl{ url.getHost(), url.getPath(), url.getPort() };
		return internal::TraceManager::getInstance().getTraceHeader(resourceId, internalUrl);
	}

	void FTSDK::addLog(std::string content, LogLevel level)
	{
		internal::LogPipeManager::getInstance().addLog(content, level);
	}

	// --- RUM interface
	void FTSDK::addLongTask(std::string log, long duration)
	{
		internal::RUMManager::getInstance().addLongTask(log, duration);
	}

	void FTSDK::addError(std::string log, std::string message, RUMErrorType errorType, AppState state)
	{
		internal::RUMManager::getInstance().addError(log, message, errorType, state);
	}

	void FTSDK::addResource(std::string resourceId, ResourceParams params, NetStatus netStatusBean)
	{
		internal::RUMManager::getInstance().addResource(resourceId, params, netStatusBean);
	}

	void FTSDK::startResource(std::string resourceId)
	{
		internal::RUMManager::getInstance().startResource(resourceId);
	}

	void FTSDK::stopResource(std::string resourceId)
	{
		internal::RUMManager::getInstance().stopResource(resourceId);
	}

	void FTSDK::addAction(std::string actionName, std::string actionType, long duration, long startTime)
	{
		internal::RUMManager::getInstance().addAction(actionName, actionType, duration, startTime);
	}

	void FTSDK::startAction(std::string actionName, std::string actionType)
	{
		internal::RUMManager::getInstance().startAction(actionName, actionType);
	}

	void FTSDK::stopAction()
	{
		internal::RUMManager::getInstance().stopAction();
	}

	void FTSDK::startView(std::string viewName)
	{
		internal::RUMManager::getInstance().startView(viewName);
	}

	void FTSDK::stopView()
	{
		internal::RUMManager::getInstance().stopView();
	}
	// --- end RUM interface
}
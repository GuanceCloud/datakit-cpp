#include "pch.h"
#ifdef _WIN32
#include "FTSDKDLLWrapper.h"
#endif
#include "Include/FTSDKFactory.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "Utils.h"
#include "LoggerManager.h"

using namespace com::ft::sdk;
using json = nlohmann::json;


namespace com::ft::sdk::wrapper {

	auto sdk = FTSDKFactory::get();
	
	void Install(const char* jsonString) {
		sdk->init();

		try
		{
			json j = json::parse(jsonString);

			internal::utils::removeNullValues(j);

			auto serverUrl = j.value("serverUrl", "");
			auto envType = j.value("envType", "prod");
			bool enableFileDBCache = j.value("enableFileDBCache", false);
			bool clearDBWhenStartUp = j.value("clearDBWhenStartUp", false);
			auto appVersion = j.value("appVersion", "");
			auto serviceName = j.value("serviceName", "");

			json globalContext = j["globalContext"];

			std::string serverUrlStr(serverUrl);
			std::string envTypeStr(envType);
			internal::LoggerManager::getInstance().logError("");

			FTSDKConfig gc;
			gc.setServerUrl(serverUrlStr);

			if (!envTypeStr.empty()) {

				std::string et = internal::utils::convertToLowerCase(envTypeStr);
				EnvType env = EnvType::PROD;

				if (et == "gray") {
					env = EnvType::GRAY;
				}
				else if (et == "common") {
					env = EnvType::COMMON;
				}
				else if (et == "local") {
					env = EnvType::LOCAL;
				}
				else if (et == "pre") {
					env = EnvType::PRE;
				}

				gc.setEnv(env);
			}
			if (enableFileDBCache) {
				gc.setEnableFileDBCache(enableFileDBCache);
			}

			if (clearDBWhenStartUp) {
				gc.setClearDBWhenStartUp(clearDBWhenStartUp);
			}

			if (!appVersion.empty()) {
				gc.setAppVersion(appVersion);
			}

			if (globalContext != NULL) {

				for (auto it = globalContext.begin(); it != globalContext.end(); ++it)
				{
					gc.addGlobalContext(it.key(), it.value());
				}
			}

			if (!serviceName.empty()) {
				gc.setServiceName(serviceName);
			}

			sdk->install(gc);

		}
		catch (const std::exception& e)
		{
			internal::LoggerManager::getInstance().logError("Install Exception:" + std::string(e.what()));
		}
	}


	void InitRUMConfig(const char* jsonString) {

		try
		{
			json j = json::parse(jsonString);

			internal::utils::removeNullValues(j);

			auto appid = j.value("appId", "");
			auto sampleRate = j.value("sampleRate", 1.0f);
			auto extraMonitorTypeWithError = j.value("extraMonitorTypeWithError", "");

			json globalContext = j["globalContext"];

			FTRUMConfig rc;
			if (!appid.empty()) {
				rc.setRumAppId(appid);
			}

			if (sampleRate) {
				rc.setSamplingRate(sampleRate);
			}

			if (globalContext != NULL) {

				//std::ostringstream oss;

				for (auto it = globalContext.begin(); it != globalContext.end(); ++it)
				{
					rc.addGlobalContext(it.key(), it.value());
				}
			}

			if (!extraMonitorTypeWithError.empty()) {
				std::string emtwe = internal::utils::convertToLowerCase(extraMonitorTypeWithError);

				if (emtwe == "all") {
					rc.setExtraMonitorTypeWithError(ErrorMonitorType::ALL);
				}
				else if (emtwe == "memory") {
					rc.setExtraMonitorTypeWithError(ErrorMonitorType::MEMORY);
				}
				else if (emtwe == "cpu") {
					rc.setExtraMonitorTypeWithError(ErrorMonitorType::CPU);
				}
			}

			sdk->initRUMWithConfig(rc);

		}
		catch (const std::exception&e)
		{
			internal::LoggerManager::getInstance().logError("InitRUMConfig Exception:" + std::string(e.what()));
		}
		
	}

	void InitLogConfig(const char* jsonString) {
		try
		{
			json j = json::parse(jsonString);

			internal::utils::removeNullValues(j);

			auto sampleRate = j.value("sampleRate", 1.0f);
			auto enableCustomLog = j.value("enableCustomLog", false);
			auto enableLinkRumData = j.value("enableLinkRumData", false);
			auto logCacheDiscardStrategy = j.value("logCacheDiscardStrategy", "");
			json globalContext = j["globalContext"];

			FTLogConfig lc;
			if (sampleRate) {
				lc.setSamplingRate(sampleRate);
			}

			if (enableCustomLog) {
				lc.setEnableCustomLog(enableCustomLog);
			}

			if (enableLinkRumData) {
				lc.setEnableLinkRumData(true);
			}

			if (globalContext != NULL) {

				for (auto it = globalContext.begin(); it != globalContext.end(); ++it)
				{
					lc.addGlobalContext(it.key(), it.value());
				}
			}

			if (!logCacheDiscardStrategy.empty()) {
				std::string lcds = internal::utils::convertToLowerCase(logCacheDiscardStrategy);

				if (lcds == "discard") {
					lc.setLogCacheDiscardStrategy(LogCacheDiscard::DISCARD);
				}
				else if (lcds == "discard_old") {
					lc.setLogCacheDiscardStrategy(LogCacheDiscard::DISCARD_OLDEST);
				}
			}

			sdk->initLogWithConfig(lc);

		}
		catch (const std::exception&e)
		{
			internal::LoggerManager::getInstance().logError("InitLogConfig Exception:" + std::string(e.what()));
		}

		

	}

	void InitTraceConfig(const char* jsonString) {
		try
		{
			json j = json::parse(jsonString);

			internal::utils::removeNullValues(j);

			auto sampleRate = j.value("sampleRate", 1.0f);
			auto traceType = j.value("traceType", "ddtrace");
			auto enableLinkRumData = j.value("enableLinkRumData", false);

			FTTraceConfig tc;
			if (sampleRate) {
				tc.setSamplingRate(sampleRate);
			}

			if (enableLinkRumData) {
				tc.setEnableLinkRUMData(enableLinkRumData);
			}

			if (!traceType.empty()) {
				std::string tt = internal::utils::convertToLowerCase(traceType);


				TraceType tte = TraceType::DDTRACE;

				if (tt == "jaeger") {
					tte = TraceType::JAEGER;
				}
				else if (tt == "skywalking") {
					tte = TraceType::SKYWALKING;
				}
				else if (tt == "traceparent") {
					tte = TraceType::TRACEPARENT;
				}
				else if (tt == "zipkin_single_header") {
					tte = TraceType::ZIPKIN_SINGLE_HEADER;
				}
				else if (tt == "zipkin_multi_header") {
					tte = TraceType::ZIPKIN_MULTI_HEADER;
				}
				tc.setTraceType(tte);
			}

			sdk->initTraceWithConfig(tc);
		}
		catch (const std::exception&e)
		{
			internal::LoggerManager::getInstance().logError("InitTraceConfig Exception:" +std::string(e.what()));
		}
		
	}

	void BindUserData(const char* jsonString) {

		try
		{
			json j = json::parse(jsonString);

			internal::utils::removeNullValues(j);

			auto userId = j.value("userId", "");
			auto userName = j.value("userName", "");
			auto userEmail = j.value("userEmail", "");
			json extra = j["extra"];

			std::string userIdStr(userId);
			std::string userNameStr(userName);
			std::string userEmailStr(userEmail);

			UserData uc;
			uc.init(userIdStr, userNameStr, userEmailStr);

			if (extra != NULL) {

				for (auto it = extra.begin(); it != extra.end(); ++it)
				{
					uc.addCustomizeItem(it.key(), it.value());
				}
			}
			sdk->bindUserData(uc);
		}
		catch (const std::exception&e)
		{
			internal::LoggerManager::getInstance().logError("BindUserData Exception:" + std::string(e.what()));
		}
		
	}


	void UnBindUserData() {
		sdk->unbindUserData();
	}

	void StartAction(const char* actionName, const char* actionType) {

		std::string actionNameStr(actionName);
		std::string actionTypeStr(actionType);
		sdk->startAction(actionNameStr, actionTypeStr);
	
	}

	void AddAction(const char* actionName, const char* actionType, long duration, long startTime) {
	
		std::string actionNameStr(actionName);
		std::string actionTypeStr(actionType);

		sdk->addAction(actionNameStr, actionTypeStr, duration, startTime);
	}

	void StartView(const char* viewName) {
		std::string viewNameStr(viewName);
		sdk->startView(viewNameStr);
	}

	void StopView() {
		sdk->stopView();
	}

	void AddError(const char* log, const char* message, const char* errorType, const char* state) {
		std::string logStr(log);
		std::string messageStr(message);
		std::string stateStr(state);
		std::string errorTypeStr(errorType);

		std::string as = internal::utils::convertToLowerCase(stateStr);
		AppState ase = AppState::UNKNOWN;

		if (as == "startup") {
			ase = AppState::STARTUP;
		}
		else if (as == "run") {
			ase = AppState::RUN;
		}

		std::string et = internal::utils::convertToLowerCase(errorType);

		RUMErrorType ret = RUMErrorType::NATIVE_CRASH;

		if (et == "network_error") {
			ret= RUMErrorType::NETWORK_ERROR;
		}

		sdk->addError(logStr, messageStr, ret, ase);
	
	}

	void AddLongTask(const char* log,long duration) {
		std::string logStr(log);
		sdk->addLongTask(logStr, duration);
	}

	void StartResource(const char* resourceId) {
		std::string resourceIdStr(resourceId);
		sdk->startResource(resourceIdStr);
	}

	void StopResource(const char* resourceId) {
		std::string resourceIdStr(resourceId);
		sdk->stopResource(resourceIdStr);
	}

	void AddResource(const char* resourceId, const char* resourceParamsJsonStr, const char* netStatusJsonStr) {
		std::string resourceIdStr(resourceId);

		try
		{
			json resourceJson = json::parse(resourceParamsJsonStr);

			internal::utils::removeNullValues(resourceJson);

			auto resourceMethod = resourceJson.value("resourceMethod", "");
			auto requestHeader = resourceJson.value("requestHeader", "");
			auto responseHeader = resourceJson.value("responseHeader", "");
			auto responseBody = resourceJson.value("responseBody", "");
			auto responseConnection = resourceJson.value("responseConnection", "");
			auto responseContentEncoding = resourceJson.value("responseContentEncoding", "");
			auto responseContentType = resourceJson.value("responseContentType", "");
			auto url = resourceJson.value("url", "");
			auto resourceStatus = resourceJson.value("resourceStatus", -1);

			json netStatusJson = json::parse(netStatusJsonStr);

			internal::utils::removeNullValues(netStatusJson);

			long dnsTime = netStatusJson.value("dnsTime", -1l);
			long tcpTime = netStatusJson.value("tcpTime", -1l);
			long sslTime = netStatusJson.value("sslTime", -1l);
			long ttfb = netStatusJson.value("ttfb", -1l);
			long responseTime = netStatusJson.value("responseTime", -1l);
			long firstByteTime = netStatusJson.value("firstByteTime", -1l);

			long fetchStartTime = netStatusJson.value("fetchStartTime", -1l);
			long dnsStartTime = netStatusJson.value("dnsStartTime", -1l);
			long dnsEndTime = netStatusJson.value("dnsEndTime", -1l);
			long responseStartTime = netStatusJson.value("responseStartTime", -1l);
			long responseEndTime = netStatusJson.value("responseEndTime", -1l);
			long sslStartTime = netStatusJson.value("sslStartTime", -1l);
			long sslEndTime = netStatusJson.value("sslEndTime", -1l);
			long tcpStartTime = netStatusJson.value("tcpStartTime", -1l);
			long tcpEndTime = netStatusJson.value("tcpEndTime", -1l);

			NetStatus status;
			ResourceParams params;
			params.resourceMethod = resourceMethod;
			params.requestHeader = requestHeader;
			params.responseHeader = responseHeader;

			params.responseBody = responseBody;
			params.responseConnection = responseConnection;
			params.responseContentEncoding = responseContentEncoding;
			params.responseContentType = responseContentType;
			params.url = url;
			params.resourceStatus = resourceStatus;

			status.dnsTime = dnsTime;
			status.tcpTime = tcpTime;
			status.sslTime = sslTime;
			status.ttfb = ttfb;
			status.responseTime = responseTime;
			status.firstByteTime = firstByteTime;

			status.fetchStartTime = fetchStartTime;
			status.dnsStartTime = dnsStartTime;
			status.dnsEndTime = dnsEndTime;
			status.responseStartTime = responseStartTime;
			status.responseEndTime = responseEndTime;
			status.sslStartTime = sslStartTime;
			status.sslEndTime = sslEndTime;
			status.tcpStartTime = tcpStartTime;
			status.tcpEndTime = tcpEndTime;

			sdk->addResource(resourceIdStr, params, status);

		}
		catch (const std::exception&e)
		{
			internal::LoggerManager::getInstance().logError("AddResource Exception:" + std::string(e.what()));
		}

		
	}

	void AddLog(const char* content, const char* logLevel) {
		std::string contentStr(content);
		std::string logLevelStr(content);

		std::string ll = internal::utils::convertToLowerCase(logLevel);
		LogLevel lle =LogLevel::INFO;

		if (ll == "critical") {
			lle = LogLevel::CRITICAL;
		}
		else if (ll == "error") {
			lle = LogLevel::ERR;
		}
		else if (ll == "warning") {
			lle= LogLevel::WARNING;
		}
		else if (ll == "ok") {
			lle= LogLevel::OK;
		}

		sdk->addLog(contentStr, lle);
	}

	const char* GetTraceHeader(const char* resourceId,const char* url) {
		std::string resourceIdStr(resourceId);
		std::string urlStr(url);
		sdk->generateTraceHeader(resourceIdStr,urlStr);

		auto headerMap = sdk->generateTraceHeader(resourceIdStr, urlStr);


		json json_obj;

		for (const auto& pair : headerMap) {
			json_obj[pair.first] = pair.second;
		}

		std::string jsonString = json_obj.dump();
		char* result = new char[jsonString.size() + 1];
		std::strcpy(result, jsonString.c_str());

		return result;
	}

	const char* GetTraceHeaderWithUrl(const char* url) {
		std::string urlStr(url);
		auto headerMap = sdk->generateTraceHeader(urlStr);

		json json_obj;

		for (const auto& pair : headerMap) {
			json_obj[pair.first] = pair.second;
		}

		std::string jsonString = json_obj.dump();
		char* result = new char[jsonString.size() + 1];
		std::strcpy(result, jsonString.c_str());

		return result;
	}


	void DeInit() {
        sdk->deinit();
	}	

}


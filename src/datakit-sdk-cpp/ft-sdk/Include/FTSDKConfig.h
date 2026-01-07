#pragma once
#ifndef _DATAKIT_SDK_CONFIG_H_
#define _DATAKIT_SDK_CONFIG_H_

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include "datakit_exports.h"
#include "FTSDKDataContracts.h"

#define PROPERTY(_class, _type, _method_name, _property_name)                \
  _class&& set##_method_name(_type val) {   \
        _property_name = val;   \
        return std::move(*this);    \
    }   \
  _type get##_method_name() const { return _property_name; };  

namespace com::ft::sdk
{
    /**
     * Global property configuration base class
     */
    template<typename T>
    class FTSDK_EXPORT ContextConfig
    {
    public:
        ContextConfig()
        {
        }

        /**
         * Add global property
         * 
         * @param key
         * @param value
         * @return 
         */
        T&& addGlobalContext(const std::string& key, const std::string& value)
        {
            m_globalContext[key] = value;

            return std::move(*(static_cast<T*>(this)));
        }

        /**
         * Get global property
         * 
         * @return 
         */
        std::map<std::string, std::string>& getGlobalContext()
        {
            return m_globalContext;
        }

    protected:
        // Set global tag
        std::map<std::string, std::string> m_globalContext;
    };

    /**
     * SDK common configuration items
     */
    class FTSDK_EXPORT FTSDKConfig : public ContextConfig<FTSDKConfig>
    {
    public:
        /**
         * Set datakit installation address
         */
        PROPERTY(FTSDKConfig, std::string, ServerUrl, serverUrl)

        /**
         * Set dataway client token
         */
        PROPERTY(FTSDKConfig, std::string, ClientToken, clientToken)

        /**
         * Set service name
         */
        PROPERTY(FTSDKConfig, std::string, ServiceName, serviceName)

        /**
         * Set external application version
         */
        PROPERTY(FTSDKConfig, std::string, AppVersion, appVersion)

        /**
         * Set the environment for data transmission
         */
        PROPERTY(FTSDKConfig, EnvType, Env, env)

        /**
         * Set whether to use file cache
         */
        PROPERTY(FTSDKConfig, bool, EnableFileDBCache, enableFileDBCache)

        /**
         * Set whether to clear cache data on startup
         */
        PROPERTY(FTSDKConfig, bool, ClearDBWhenStartUp, clearDBWhenStartUp)

    private:
        // Environment for crash logs
        EnvType env = EnvType::PROD;

        std::string appVersion = "";

        bool enableFileDBCache = false;
        bool clearDBWhenStartUp = true;

        // HttpConfig
        std::string serverUrl;
        std::string clientToken = "";

        std::string serviceName = "";

    };


    		/**
		 * User data
		 */
    class FTSDK_EXPORT UserData 
    {
    public:
        UserData() {}

        /**
         * Initialize user data
         * 
         * @param name
         * @param id
         * @param email
         * @return 
         */
        UserData&& init(const std::string& name, const std::string& id, const std::string& email) 
        {
            this->name = name;
            this->id = id;
            this->email = email;

            return std::move(*this);
        }

        /**
         * Reset user data
         * 
         */
        void reset()
        {
            this->name = "";
            this->id = "";
            this->email = "";

            this->exts.clear();
        }

        /**
         * Add user custom field
         * 
         * @param key
         * @param value
         * @return 
         */
        UserData&& addCustomizeItem(const std::string& key, const std::string& value)
        {
            exts[key] = value;

            return std::move(*this);
        }

        /**
         * Set user name
         */
        PROPERTY(UserData, std::string, Name, name);

        /**
         * Set user Id
         */
        PROPERTY(UserData, std::string, Id, id);

        /**
         * Set user email address
         */
        PROPERTY(UserData, std::string, Email, email);


        std::map<std::string, std::string>& getExts()
        {
            return exts;
        }

    private:
        std::string name;
        std::string id;
        std::string email;
        std::map<std::string, std::string> exts;
    };

    /**
     * RUM related configuration items
     */
    class FTSDK_EXPORT FTRUMConfig : public ContextConfig<FTRUMConfig>
    {
    public:
        FTRUMConfig() {}

        bool isRumEnable() 
        {
            return getRumAppId() != "";
        }

        /**
         * Set RUM sampling rate
         */
        PROPERTY(FTRUMConfig, float, SamplingRate, samplingRate);

        /**
         * Set application Id
         */
        PROPERTY(FTRUMConfig, std::string, RumAppId, rumAppId);

        /**
         * Set error data monitoring type
         */
        PROPERTY(FTRUMConfig, ErrorMonitorType, ExtraMonitorTypeWithError, extraMonitorTypeWithError);

        /**
         * Set device data monitoring type
         */
        //PROPERTY(FTRUMConfig, DeviceMetricsMonitorType, DeviceMetricsMonitorType, deviceMetricsMonitorType);

        /**
         * Set data monitoring frequency
         */
        //PROPERTY(FTRUMConfig, DetectFrequency, DeviceMetricsDetectFrequency, deviceMetricsDetectFrequency);
    private:
        float samplingRate = 1.0f;

        std::string rumAppId = "";
        		//Set whether to collect crash logs
        bool enableTrackAppCrash = false;
        		//Set whether to detect UI lag
        bool enableTrackAppUIBlock = false;
        		//Set whether to detect ANR
        bool enableTrackAppANR = false;
        		//Whether to enable user behavior action tracking
        bool enableTraceUserAction = false;
        		//Whether to enable user behavior view tracking
        bool enableTraceUserView = false;
        		//Whether to enable user behavior Resource tracking
        bool enableTraceUserResource = false;
        		//Crash collection data additional type
        ErrorMonitorType extraMonitorTypeWithError = ErrorMonitorType::NO_SET;

        		//Monitoring metrics data type
        DeviceMetricsMonitorType deviceMetricsMonitorType = DeviceMetricsMonitorType::NO_SET;
        DetectFrequency deviceMetricsDetectFrequency = DetectFrequency::DEFAULT;

    };

    /**
     * Trace related configuration items
     */
    class FTSDK_EXPORT FTTraceConfig
    {
    public:
        FTTraceConfig() {}

        /**
         * Set Trace sampling rate
         */
        PROPERTY(FTTraceConfig, float, SamplingRate, samplingRate);

        /**
         * Set Trace type
         */
        PROPERTY(FTTraceConfig, TraceType, TraceType, traceType);

        /**
         * Set whether to associate RUM data
         */
        PROPERTY(FTTraceConfig, bool, EnableLinkRUMData, enableLinkRUMData);

    private:
        float samplingRate = 1;
        TraceType traceType = TraceType::DDTRACE;
        bool enableWebTrace = false;
        bool enableAutoTrace = false;
        bool enableLinkRUMData = false;
    };

    /**
     * Log related configuration items
     */
    class FTSDK_EXPORT FTLogConfig : public ContextConfig<FTLogConfig>
    {
    public:
        FTLogConfig() {}

        /**
         * Set Log sampling rate
         */
        PROPERTY(FTLogConfig, float, SamplingRate, samplingRate);

        /**
         * Set whether to associate RUM data
         */
        PROPERTY(FTLogConfig, bool, EnableLinkRumData, enableLinkRumData);

        /**
         * Set whether to upload custom logs
         */
        PROPERTY(FTLogConfig, bool, EnableCustomLog, enableCustomLog);

                /**
         * Set log data database storage strategy.
         */
        PROPERTY(FTLogConfig, LogCacheDiscard, LogCacheDiscardStrategy, logCacheDiscardStrategy);

        std::vector<LogLevel>& getLogLevelFilters() {
            return logLevelFilters;
        }

        FTLogConfig&& setLogLevelFilters(std::vector<LogLevel>& logLevelFltrs) {
            std::copy_n(logLevelFltrs.begin(), logLevelFltrs.size(), std::back_inserter(this->logLevelFilters));
            return std::move(*this);
        }

        bool checkLogLevel(LogLevel status) {
            return (logLevelFilters.size() == 0)
                || (std::find(logLevelFilters.begin(), logLevelFilters.end(), status) != logLevelFilters.end());
        }


    private:
        float samplingRate = 1;
        bool enableLinkRumData = false;
        bool enableConsoleLog = false;
        bool enableCustomLog = false;
        std::string logPrefix = "";
        std::vector<LogLevel> logLevelFilters;

        LogCacheDiscard logCacheDiscardStrategy = LogCacheDiscard::DISCARD;
    };
}

#endif // _DATAKIT_SDK_CONFIG_H_

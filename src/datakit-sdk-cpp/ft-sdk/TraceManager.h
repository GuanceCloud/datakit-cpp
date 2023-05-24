﻿#pragma once
#ifndef _DATAKIT_SDK_TRACE_MANAGER_H_
#define _DATAKIT_SDK_TRACE_MANAGER_H_

#include "AbstractManager.h"
#include "Singleton.h"
#include <map>
#include <string>
#include "Include/FTSDKConfig.h"
#include "InternalStructs.h"
#include "TraceHeader.h"
#include "FTSDKConfigManager.h"

namespace com::ft::sdk::internal
{
    struct TraceHeaderContainer 
    {
        bool addResourced = false;
        bool resourceStop = false;
        std::int64_t startTime = 0;

        const int TIME_OUT = 60000;//暂不考虑长链接情况

        TraceHeader* header;

        TraceHeaderContainer(TraceHeader* header) 
        {
            this->header = header;
            addResourced = !FTSDKConfigManager::getInstance().getRUMConfig().isRumEnable();
            resourceStop = addResourced;
            startTime = internal::utils::getCurrentNanoTime();
        }

        /**
         * 未避免错误调用造成内存溢出
         *
         * @return
         */
        bool isTimeOut() {
            return internal::utils::getCurrentNanoTime() - startTime > TIME_OUT;
        }
    };

	class TraceManager :
		public AbstractManager, public Singleton<TraceManager>
	{
	public:
		void initialize(FTTraceConfig& config);
		void deinitialize();

		std::map<std::string, std::string> getTraceHeader(HttpUrl& httpUrl);
		std::map<std::string, std::string> getTraceHeader(std::string key, HttpUrl httpUrl);

        TraceHeader* getHeader(std::string resourceId);
        void removeByAddResource(std::string key);
        void removeByStopResource(std::string key);
        void checkToRemove(std::string key, TraceHeaderContainer* container);
	private:
        std::map<std::string, TraceHeaderContainer*> m_mapTraceHeaderContainer;
		std::string m_traceID = "";
		std::string m_spanID = "";

		//是否可以采样
		bool m_enableTrace;

		FTTraceConfig m_traceConfig;
	};

}

#endif //_DATAKIT_SDK_TRACE_MANAGER_H_
#include "pch.h"
#include "TraceManager.h"
#include "TraceHeader.h"

namespace com::ft::sdk::internal
{
    void TraceManager::initialize(FTTraceConfig& config)
    {
        m_traceConfig = config;

        m_enableTrace = utils::enableTraceSamplingRate(config.getSamplingRate());
    }

    void TraceManager::deinitialize()
    {

    }

    std::map<std::string, std::string> TraceManager::getTraceHeader(HttpUrl& httpUrl)
    {
        TraceHeader th(m_enableTrace, m_traceConfig.getTraceType());

        return th.getTraceHeader(httpUrl);
    }

    std::map<std::string, std::string> TraceManager::getTraceHeader(std::string key, HttpUrl httpUrl)
    {
        TraceHeader* pTH = new TraceHeader(m_enableTrace, m_traceConfig.getTraceType());
        m_mapTraceHeaderContainer[key] = new TraceHeaderContainer(pTH);

        std::map<std::string, std::string> header = pTH->getTraceHeader(httpUrl);
        return header;
    }

    TraceHeader* TraceManager::getHeader(std::string resourceId)
    {
        TraceHeaderContainer* pContainer = m_mapTraceHeaderContainer[resourceId];
        if (pContainer != nullptr)
        {
            return pContainer->header;
        }

        return nullptr;
    }

    void TraceManager::removeByAddResource(std::string key)
    {
        TraceHeaderContainer* pContainer = m_mapTraceHeaderContainer[key];
        if (pContainer != nullptr) 
        {
            pContainer->addResourced = true;
            checkToRemove(key, pContainer);
        }
    }

    void TraceManager::removeByStopResource(std::string key)
    {
        TraceHeaderContainer* pContainer = m_mapTraceHeaderContainer[key];
        if (pContainer != nullptr)
        {
            pContainer->resourceStop = true;
            checkToRemove(key, pContainer);
        }
    }

    void TraceManager::checkToRemove(std::string key, TraceHeaderContainer* pContainer)
    {
        if (pContainer->addResourced && pContainer->resourceStop
            || pContainer->isTimeOut()) 
        {
            m_mapTraceHeaderContainer.erase(key);
        }
    }
}
﻿#pragma once
#ifndef _DATAKIT_SDK_CACHE_DB_MANAGER_H_
#define _DATAKIT_SDK_CACHE_DB_MANAGER_H_

#include "Singleton.h"
#include "DataSyncManager.h"
#include <deque>

namespace com::ft::sdk::internal
{
	class CacheDBManager : public Singleton<CacheDBManager>
	{
	public:
		CacheDBManager();
		void deinit();
		void insertRUMItem(std::string measurementName, std::map<std::string, std::string> tags, std::map<std::string, std::any> fields);
		void insertLogItem(std::string measurementName, std::map<std::string, std::string> tags, std::map<std::string, std::any> fields);
		void insertTraceItem(std::string measurementName, std::map<std::string, std::string> tags, std::map<std::string, std::any> fields);

		void updateRUMCollect(bool collect);
	private:
		void processData();
		void addGlobalContexts(TagMap& tags, TagMap& src);
	private:
		std::mutex m_dataQueMtx;
		std::deque<Measurement> m_dqMeasurement;

		bool m_enableFileCache = false;
		bool m_enableRUMCollect = true;
		bool m_stopping = false;
	};

}

#endif //_DATAKIT_SDK_CACHE_DB_MANAGER_H_
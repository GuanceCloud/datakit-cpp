/*****************************************************************//**
 * \file   DataStoreTest.cpp
 * \brief  Data Store Test
 * 
 * \author Zhou Guangyong
 * \date   June 2023
 *********************************************************************/

#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineDBManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LoggerManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/DataSyncManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConfigManager.h"
#include "TestHelper.h"
#include <algorithm>

using namespace com::ft::sdk;
using namespace _test::helper;

class DataStoreTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
		static bool inited = false;

		if (!inited)
		{
			inited = true;
			internal::LoggerManager::getInstance().init();
			internal::LineDBManager::getInstance().init();
		}
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;
		internal::LineDBManager::getInstance().uninit();
	}

	internal::DataMsg createDataMsg(DataType type)
	{
		internal::DataMsg msg;
		msg.dataType = type;
		internal::Measurement meas;
		meas.measurementName = std::string("TEST_NAME");
		meas.tags["tag1"] = std::string("value1");
		meas.fields["field1"] = 100;
		msg.vtLine.push_back(meas);

		return msg;
	}
};

TEST_F(DataStoreTest, TesDB_RUM_CURD) {
	DataType type = DataType::RUM_APP;
	internal::DataMsg msg = createDataMsg(type);

	bool ret = internal::LineDBManager::getInstance().insertLine(msg);
	EXPECT_TRUE(ret);

	auto lines = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines.size() == 1);
	EXPECT_TRUE(lines[0]->dataType == type);
	EXPECT_TRUE(lines[0]->rawLine.size() > 0);

	internal::LineDBManager::getInstance().deleteLineFromDB(*(lines[0]));
	auto lines2 = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines2.size() == 0);
}

TEST_F(DataStoreTest, TesDB_LOG_CURD) {
	DataType type = DataType::LOG;
	internal::DataMsg msg = createDataMsg(type);

	bool ret = internal::LineDBManager::getInstance().insertLine(msg);
	EXPECT_TRUE(ret);

	auto lines = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines.size() == 1);
	EXPECT_TRUE(lines[0]->dataType == type);
	EXPECT_TRUE(lines[0]->rawLine.size() > 0);

	internal::LineDBManager::getInstance().deleteLineFromDB(*(lines[0]));
	auto lines2 = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines2.size() == 0);
}

TEST_F(DataStoreTest, TesDB_LOG_OVERFLOW_DISCARD) {
	DataType type = DataType::LOG;
	internal::DataMsg msg = createDataMsg(type);

	internal::LineDBManager::getInstance().setUserLogCountThreshold(10);
	internal::LineDBManager::getInstance().updateLogCacheDiscard(LogCacheDiscard::DISCARD);

	for (int i = 0; i < 15; i++)
	{
		bool ret = internal::LineDBManager::getInstance().insertLine(msg);
		if (i < 10)
		{
			EXPECT_TRUE(ret);
		}
		else
		{
			EXPECT_TRUE(!ret);
		}
	}

	auto lines = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines.size() > 0);

	int lnCnt = 0;
	std::for_each(lines.begin(), lines.end(), [&](auto pMsg) {
		lnCnt += pMsg->vtId.size();
		});
	EXPECT_TRUE(lnCnt == 10);
	EXPECT_TRUE(lines[0]->dataType == type);

	for (auto ln : lines)
	{
		internal::LineDBManager::getInstance().deleteLineFromDB(*ln);
	}
	auto lines2 = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines2.size() == 0);
}

TEST_F(DataStoreTest, TesDB_LOG_OVERFLOW_OLDEST) {
	DataType type = DataType::LOG;
	internal::DataMsg msg = createDataMsg(type);

	internal::LineDBManager::getInstance().setUserLogCountThreshold(10);
	internal::LineDBManager::getInstance().updateLogCacheDiscard(LogCacheDiscard::DISCARD_OLDEST);

	for (int i = 0; i < 15; i++)
	{
		bool ret = internal::LineDBManager::getInstance().insertLine(msg);
		EXPECT_TRUE(ret);
	}

	auto lines = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines.size() > 0);

	int lnCnt = 0;
	std::for_each(lines.begin(), lines.end(), [&](auto pMsg) {
		lnCnt += pMsg->vtId.size();
	});
	EXPECT_TRUE(lnCnt == 10);
	EXPECT_TRUE(lines[0]->dataType == type);

	for (auto ln : lines)
	{
		internal::LineDBManager::getInstance().deleteLineFromDB(*ln);
	}
	auto lines2 = internal::LineDBManager::getInstance().queryLineFromDB(type, true);
	EXPECT_TRUE(lines2.size() == 0);
}
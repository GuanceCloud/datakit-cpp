/*****************************************************************//**
 * \file   DataSyncTest.cpp
 * \brief  Data Sync Test
 * 
 * \author Zhou Guangyong
 * \date   June 2023
 *********************************************************************/

#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/LoggerManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/DataSyncManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConfigManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineDBManager.h"
#include "TestHelper.h"

using namespace com::ft::sdk;
using namespace _test::helper;

class DataSyncTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
		static bool inited = false;

		if (!inited)
		{
			waitForCompleted();
			inited = true;
			internal::LoggerManager::getInstance().init();
			internal::FTSDKConfigManager::getInstance().getGeneralConfig().setEnableFileDBCache(true);
			internal::LineDBManager::getInstance().init();
			internal::DataSyncManager::getInstance().init();
		}
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;

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

TEST_F(DataSyncTest, DataSync_RUM_With_200) {
	auto msg = createDataMsg(DataType::RUM_APP);

	internal::FTSDKConfigManager::getInstance().getTestConfig().setOfflineMode(true);
	internal::FTSDKConfigManager::getInstance().getTestConfig().setHttpStatus(HTTP_STATUS::HTTP_OK);
	internal::LineDBManager::getInstance().insertLine(msg);
	
	waitForCompleted();

	// http return code 200, message is sent and removed from db.
	auto lines = internal::LineDBManager::getInstance().queryLineFromDB(msg.dataType);
	EXPECT_TRUE(lines.size() == 0);
}

TEST_F(DataSyncTest, DataSync_RUM_With_400) {
	auto msg = createDataMsg(DataType::RUM_APP);

	internal::FTSDKConfigManager::getInstance().getTestConfig().setOfflineMode(true);
	internal::FTSDKConfigManager::getInstance().getTestConfig().setHttpStatus(HTTP_STATUS::HTTP_BAD_REQUEST);
	internal::LineDBManager::getInstance().insertLine(msg);

	waitForCompleted();

	// http return code 400, message is invalid and just removed from db.
	auto lines = internal::LineDBManager::getInstance().queryLineFromDB(msg.dataType);
	EXPECT_TRUE(lines.size() == 0);
}

TEST_F(DataSyncTest, DataSync_RUM_With_500) {
	auto msg = createDataMsg(DataType::RUM_APP);

	internal::FTSDKConfigManager::getInstance().getTestConfig().setOfflineMode(true);
	internal::FTSDKConfigManager::getInstance().getTestConfig().setHttpStatus(HTTP_STATUS::HTTP_SERVER_ERROR);
	internal::LineDBManager::getInstance().insertLine(msg);

	waitForCompleted();

	// http return code 500, message is put back into db, and wait for next chance.
	auto lines = internal::LineDBManager::getInstance().queryLineFromDB(msg.dataType);
	EXPECT_TRUE(lines.size() == 1);
}

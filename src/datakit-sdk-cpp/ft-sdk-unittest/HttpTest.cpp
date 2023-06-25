/*****************************************************************//**
 * \file   HttpTest.cpp
 * \brief  HTTP Test
 * 
 * \author Zhou Guangyong
 * \date   June 2023
 *********************************************************************/
#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/LoggerManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/CommunicationManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConfigManager.h"
#include "TestHelper.h"

using namespace com::ft::sdk;
using namespace _test::helper;

class HTTPTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
		internal::LoggerManager::getInstance().init();

		FTSDKConfig config;
		config.setServerUrl("http://www.baidu.com");
		internal::CommunicationManager::getInstance().initialize(config);
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;

	}

};

TEST_F(HTTPTest, HTTP_GET_TEST) {
	internal::FTSDKConfigManager::getInstance().getTestConfig().setOfflineMode(false);
	internal::ResponseData res = internal::CommunicationManager::getInstance().get("");
	EXPECT_TRUE(res.code == 200);
}

TEST_F(HTTPTest, HTTP_GET_TEST_WITH_LINK) {
	internal::FTSDKConfigManager::getInstance().getTestConfig().setOfflineMode(false);
	internal::ResponseData res = internal::CommunicationManager::getInstance().get("/void");
	EXPECT_TRUE(res.code != 200);
}

TEST_F(HTTPTest, HTTP_POST_TEST) {
	internal::FTSDKConfigManager::getInstance().getTestConfig().setOfflineMode(false);
	std::string data = "for test";
	internal::ResponseData res = internal::CommunicationManager::getInstance().post(DataType::LOG, data);
	EXPECT_TRUE(res.code != 200);
}

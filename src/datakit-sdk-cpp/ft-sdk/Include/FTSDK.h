/*****************************************************************//**
 * \file   FTSDK.h
 * \brief  Datakit SDK C++ interface.
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/

#pragma once
#ifndef _DATAKIT_SDK_H_
#define _DATAKIT_SDK_H_

#include <string>
#include <vector>
#include "datakit_exports.h"
#include "FTSDKConfig.h"
#include "FTSDKDataContracts.h"

namespace com::ft::sdk
{
	class FTSDKFactory;
	typedef std::map<std::string, std::string> PropagationHeader;

	/**
	 * Datakit SDK interface class, all operations are accessed through this interface.
	 */
	class FTSDK_EXPORT FTSDK
	{
	public:
		/// Disable copy constructor
		FTSDK& operator=(const FTSDK&) = delete;

		/// Disable copy constructor
		FTSDK(const FTSDK&) = delete;

		/**
		 * Initialize SDK
		 * 
		 */
		void init();

		/**
		 * Shutdown SDK, perform related resource cleanup operations
		 * 
		 */
		void deinit();

		/**
		 * Get SDK version
		 * 
		 * @return SDK version number
		 */
		std::string getVersionString();

		/**
		 * Configure global common parameters
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& install(FTSDKConfig& config);

		/**
		 * Configure RUM parameters
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& initRUMWithConfig(FTRUMConfig& config);
		
		/**
		 * Configure Trace parameters
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& initTraceWithConfig(FTTraceConfig& config);
		
		/**
		 * Configure Log parameters
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& initLogWithConfig(FTLogConfig& config);

		/**
		 * Bind user data
		 * 
		 * @param config	User data
		 * @return
		 */
		FTSDK&& bindUserData(UserData& config);		

		/**
		 * Unbind user data
		 * 
		 */
		void unbindUserData();

		/**
		 * Generate trace data according to configuration
		 * 
		 * @param url	Network address
		 * @return		Trace data
		 */
		PropagationHeader generateTraceHeader(const std::string& url);

		/**
		 * Generate trace data according to configuration
		 * 
		 * @param resourceId	Associated resource id
		 * @param url			Network address
		 * @return				Trace data
		 */
		PropagationHeader generateTraceHeader(const std::string& resourceId, const std::string& url);

		/**
		 * Upload user logs to datakit
		 * 
		 * @param content	Log content
		 * @param level		Log level
		 */
		void addLog(std::string content, LogLevel level);

		// ---RUM interface
		/**
		 * Add long-running task
		 * 
		 * @param log		Log
		 * @param duration	Duration (ns)
		 */
		void addLongTask(std::string log, long duration);

		/**
		 * Add error information
		 * 
		 * @param log		Log
		 * @param message	Message
		 * @param errorType	Error type
		 * @param state		Program running state
		 */
		void addError(std::string log, std::string message, RUMErrorType errorType, AppState state);

		/**
		 * Set network transmission content
		 * 
		 * @param resourceId		Resource Id
		 * @param params			Network transmission parameters
		 * @param netStatusBean		Network status statistics
		 */
		void addResource(std::string resourceId, ResourceParams params, NetStatus netStatusBean);

		/**
		 * Resource start
		 * 
		 * @param resourceId		Resource Id
		 */
		void startResource(std::string resourceId);

		/**
		 * Resource end
		 * 
		 * @param resourceId
		 */
		void stopResource(std::string resourceId);

		void addAction(std::string actionName, std::string actionType, long duration, long startTime);

		/**
		 * Action start
		 * 
		 * @param actionName Action name
		 * @param actionType Action type
		 */
		void startAction(std::string actionName, std::string actionType);

		/**
		 * Action end
		 * 
		 */
		void stopAction();

		/**
		 * View start.
		 * 
		 * @param viewName Current page name
		 */
		void startView(std::string viewName);

		/**
		 * View end.
		 * 
		 */
		void stopView();

		// ---end RUM interface

	private:
		/// Constructor required config json file
		explicit FTSDK(const std::string& configJson);

		friend class FTSDKFactory;
	};

}

#endif // _DATAKIT_SDK_H_
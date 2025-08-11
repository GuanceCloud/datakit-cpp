#pragma once
#ifndef _DATAKIT_CPP_DATA_CONTRACT_H_
#define _DATAKIT_CPP_DATA_CONTRACT_H_

#include <string>
#include <vector>
#include <stdexcept>
#include "datakit_exports.h"

#define ENUM_MACRO10(name, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10)\
    enum class name { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10 };\
    const char *name##Strings[] = { #v1, #v2, #v3, #v4, #v5, #v6, #v7, #v8, #v9, #v10};\
    template<typename T>\
    constexpr const char *name##ToString(T value) { return name##Strings[static_cast<int>(value)]; }

#define ENUM_MACRO2(name, v1, v2)\
    enum class name { v1, v2};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

#define ENUM_MACRO3(name, v1, v2, v3)\
    enum class name { v1, v2, v3};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2, #v3};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

#define ENUM_MACRO4(name, v1, v2, v3, v4)\
    enum class name { v1, v2, v3, v4};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2, #v3, #v4};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

#define ENUM_MACRO5(name, v1, v2, v3, v4, v5)\
    enum class name { v1, v2, v3, v4, v5};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2, #v3, #v4, #v5};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

namespace com::ft::sdk
{
	/**
	 * Trace type
	 */
	enum class TraceType : char {
		/**
		 * datadog trace
		 *
		 * x-datadog-trace-id
		 * x-datadog-parent-id
		 * x-datadog-sampling-priority
		 * x-datadog-origin
		 */
		DDTRACE = 0x0,

		/**
		 * zipkin multi header
		 *
		 * X-B3-TraceId
		 * X-B3-SpanId
		 * X-B3-Sampled
		 */
		 ZIPKIN_MULTI_HEADER,

		 /**
		  * zipkin single header b3
		 */
		 ZIPKIN_SINGLE_HEADER,

		 /**
		  * w3c traceparent
		 */
		 TRACEPARENT,

		 /**
		  * skywalking 8.0+
		 */
		 SKYWALKING,

		 /**
		  * jaeger,header uber-trace-id
		 */
		 JAEGER
	};

	/**
	 * Sky Walking Trace version
	 */
	enum class SkyWalkingVersion : char
	{
		V2,
		V3
	};

	/**
	 * Program running state
	 */
	ENUM_MACRO3(AppState, UNKNOWN, STARTUP, RUN)

	/**
	 * RUM error type
	 */
	ENUM_MACRO4(RUMErrorType, NATIVE_CRASH, JAVA_CRASH, FLUTTER_CRASH, NETWORK_ERROR)

	/**
	 * Error source
	 */
	ENUM_MACRO2(ErrorSource, LOGGER, NETWORK)

	/**
	 * Log level
	 */
	ENUM_MACRO5(LogLevel, INFO, WARNING, ERR, CRITICAL, OK)

	/**
	 * Data transmission environment
	 */
	ENUM_MACRO5(EnvType, PROD, GRAY, PRE, COMMON, LOCAL)

	/**
	 * Monitoring type that needs to be recorded when reporting errors.
	 */
	enum class ErrorMonitorType : unsigned int {
		ALL = 0xFFFFFFFF,
		// BATTERY = 1 << 1,
		MEMORY = 1 << 2,
		CPU = 1 << 3,
		NO_SET = 0
	};

	/**
	 * Monitoring type that needs to be recorded when reporting device data.
	 */
	enum class DeviceMetricsMonitorType : unsigned int {
		ALL = 0xFFFFFFFF,
		BATTERY = 1 << 1,
		MEMORY = 1 << 2,
		CPU = 1 << 3,
		FPS = 1 << 4,
		NO_SET = 0
	};

	/**
	 * Monitoring frequency when reporting device data.
	 */
	enum class DetectFrequency : unsigned short {
		DEFAULT = 500,
		FREQUENT = 100,
		RARE = 1000
	};

	/**
	 * Log database cache discard strategy.
	 */
	enum class LogCacheDiscard : unsigned char {
		DISCARD,
		DISCARD_OLDEST
	};

    	/**
	 * Network time consumption statistics, supported methods:
	 * 1. Time period
	 * 2. Time point
	 */
	struct FTSDK_EXPORT NetStatus {
		//----------Time period parameters-------------------
        		/** TCP connection time consumption */
        long tcpTime = -1;
        		/** DNS resolution time consumption */
        long dnsTime = -1;
        		/** SSL connection time consumption */
        long sslTime = -1;
        		/** Total time from DNS resolution to receiving the first data packet */
        long firstByteTime = -1;
        		/** Request response time, time from sending request to receiving response first packet */
        long ttfb = -1;
        		/** Response content transmission time consumption */
        long responseTime = -1;

		//----------Time point parameters-------------------
        		/** Request start time */
		long fetchStartTime = -1;
        		/** TCP connection time */
		long tcpStartTime = -1;
        		/** TCP end time */
		long tcpEndTime = -1;
        		/** DNS start time */
		long dnsStartTime = -1;
        		/** DNS end time */
		long dnsEndTime = -1;
        		/** Response start time */
		long responseStartTime = -1;
        		/** Response end time */
		long responseEndTime = -1;
        		/** SSL start time */
		long sslStartTime = -1;
        		/** SSL end time */
		long sslEndTime = -1;

	};

    	/**
	 * Network transmission parameters
	 */
	struct FTSDK_EXPORT ResourceParams {
        		/** Network address */
		std::string url = "";
        		/** Request header parameters */
		std::string requestHeader = "";
        		/** Response header parameters */
		std::string responseHeader = "";
        		/** Response connection */
		std::string responseConnection = "";
        		/** Response ContentType */
		std::string responseContentType = "";
        /** Response ContentEncoding */
		std::string responseContentEncoding = "";
        /** Response method */
		std::string resourceMethod = "";
        /** Response body content */
		std::string responseBody = "";
        /** Response HTTP status */
		int resourceStatus = -1;
	};

	class FTSDK_EXPORT FTSDKException : public std::exception
	{
	public:
		FTSDKException(const std::string& file, int line, const std::string& source, const std::string& message)
			: std::exception(std::logic_error(source + " : " + message))
		{
		}
	};

}

#endif // !_DATAKIT_CPP_DATA_CONTRACT_H_


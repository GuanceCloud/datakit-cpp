#pragma once

#ifdef SUPERDLLS_EXPORTS
#define SUPERDLLS_API __declspec(dllexport)
#else
#define SUPERDLLS_API __declspec(dllimport)
#endif

namespace com::ft::sdk::wrapper {
	extern "C" SUPERDLLS_API void __stdcall Install(const char* jsonString);
	extern "C" SUPERDLLS_API void __stdcall InitRUMConfig(const char* jsonString);
	extern "C" SUPERDLLS_API void __stdcall InitLogConfig(const char* jsonString);
	extern "C" SUPERDLLS_API void __stdcall InitTraceConfig(const char* jsonString);
	extern "C" SUPERDLLS_API void __stdcall BindUserData(const char* jsonString);
	extern "C" SUPERDLLS_API void __stdcall UnBindUserData();
	extern "C" SUPERDLLS_API void __stdcall StartAction(const char* actionName, const char* actionType);
	extern "C" SUPERDLLS_API void __stdcall AddAction(const char* actionName, const char* actionType, long duration, long startTime);
	extern "C" SUPERDLLS_API void __stdcall StartView(const char* viewName);
	extern "C" SUPERDLLS_API void __stdcall StopView();
	extern "C" SUPERDLLS_API void __stdcall AddError(const char* log, const char* message, const char* errorType, const char* state);
	extern "C" SUPERDLLS_API void __stdcall AddLongTask(const char* log, long duration);
	extern "C" SUPERDLLS_API void __stdcall StartResource(const char* resourceId);
	extern "C" SUPERDLLS_API void __stdcall StopResource(const char* resourceId);
	extern "C" SUPERDLLS_API void __stdcall AddResource(const char* resourceId, const char* resourceParamsJsonStr, const char* netStatusJsonStr);
	extern "C" SUPERDLLS_API void __stdcall AddLog(const char* content, const char* logLevel);
	extern "C" SUPERDLLS_API const char* __stdcall GetTraceHeader(const char* resourceId, const char* url);
	extern "C" SUPERDLLS_API const char* __stdcall GetTraceHeaderWithUrl(const char* url);
	extern "C" SUPERDLLS_API void __stdcall DeInit();

}
using System;
using System.Runtime.InteropServices;

namespace ft_sdk_wrapper_sample
{
    class Program
    {
        const string FAKE_URL = "https://www.guance.com/path_test/?param=1";


        static void Main(string[] args)
        {

            FTWrapper.Install(@"
            {
                ""serverUrl"": ""http://10.0.0.1:9529"",
                ""envType"": ""prod"",
                ""serviceName"": ""Your Services"",
                ""globalContext"": {
                    ""custom_key"": ""custom value""
                }
            }");

            FTWrapper.InitRUMConfig(@"
            {
                ""appId"": ""appid_win_xxxxxx"",
                ""sampleRate"": 0.8,
                ""globalContext"": {
                    ""rum_custom"": ""rum custom value""
                }
            }");

            FTWrapper.InitLogConfig(@"
            {
                ""sampleRate"": 0.9,
                ""enableCustomLog"": true,
                ""enableLinkRumData"": true,
                ""globalContext"": {
                    ""log_custom"": ""log custom value""
                }
            }");

            FTWrapper.InitTraceConfig(@"
            {
                ""sampleRate"": 0.9,
                ""traceType"": ""ddtrace"",
                ""enableLinkRumData"": true
            }");

            FTWrapper.BindUserData(@"
            {
                ""userId"": ""userid"",
                ""userName"": ""someone"",
                ""userEmail"": ""someone@email.com"",
                ""extra"": {
                    ""custom_data"": ""custom data""
                }
            }");

            FTWrapper.StartView("Main Page");

            FTWrapper.AddLog("test log", "test message");

            FTWrapper.StartAction("click", "test");

            FTWrapper.AddLongTask("long task test", 100002);

            FTWrapper.AddError("error", "error msg", "native_crash", "run");

            string resourceId = "unique_uuid_xxxx";

            IntPtr headData = FTWrapper.GetTraceHeader(resourceId, FAKE_URL);
            string helloJsonString = Marshal.PtrToStringAnsi(headData);

            Console.WriteLine(helloJsonString);

            FTWrapper.StartResource(resourceId);

            FTWrapper.StopResource(resourceId);

            FTWrapper.AddResource(resourceId, @"
            {
                ""url"": ""https://api.fxbsports.com/commune"",
                ""requestHeader"": null,
                ""responseHeader"": null,
                ""responseConnection"": null,
                ""responseContentType"": null,
                ""responseContentEncoding"": null,
                ""resourceMethod"": null,
                ""responseBody"": null,
                ""resourceStatus"": 200
            }",
            @"{
                ""dnsTime"": 0,
                ""fetchStartTime"": null,
                ""tcpTime"": 0,
                ""responseTime"": 0,
                ""sslTime"": 0,
                ""firstByteTime"": 0,
                ""ttfb"": null,
                ""tcpStartTime"": 0,
                ""tcpEndTime"": 0,
                ""dnsStartTime"": 0,
                ""dnsEndTime"": 0,
                ""responseStartTime"": 0,
                ""responseEndTime"": 0,
                ""sslStartTime"": 0,
                ""sslEndTime"": 0
            }");

            FTWrapper.StopView();

            while (true)
            {

            }

        }
    }
}

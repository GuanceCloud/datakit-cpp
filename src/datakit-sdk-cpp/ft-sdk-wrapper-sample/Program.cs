using System;
using System.Runtime.InteropServices;

namespace ft_sdk_wrapper_sample
{
    class Program
    {
        const string FAKE_URL= "https://www.guance.com/path_test/?param=1";

        static void Main(string[] args)
        {

            FTWrapper.Install("{\"serverUrl\":\"http://10.100.64.166:9529\",\"envType\":\"prod\",\"serviceName\":\"Your Services\",\"globalContext\":{\"custom_key\":\"custom value\"}}");

            FTWrapper.InitRUMConfig("{\"appId\":\"appid_win_57931d89\",\"sampleRate\":0.8,\"globalContext\":{\"rum_custom\":\"rum custom value\"}}");

            FTWrapper.InitLogConfig("{\"sampleRate\":0.9,\"enableCustomLog\":true,\"enableLinkRumData\":true,\"globalContext\":{\"log_custom\":\"log custom value\"}}");

            FTWrapper.InitTraceConfig("{\"sampleRate\":0.9,\"traceType\":\"ddtrace\",\"enableLinkRumData\":true}");

            FTWrapper.BindUserData("{\"userId\": \"userid\",\"userName\": \"someone\",\"userEmail\":\"someone@email.com\",\"extra\": {\"custom_data\": \"custom data\"}}");

            FTWrapper.StartView("Main Page");

            FTWrapper.AddLog("test log", "test message");

            FTWrapper.StartAction("click", "test");

            FTWrapper.AddLongTask("long task test", 100002);

            FTWrapper.AddError("error", "error msg","","native_crash");

            string resourceId = "unique_uuid_xxxx";

            IntPtr headData = FTWrapper.GetTraceHeader(resourceId,FAKE_URL);
            string helloJsonString = Marshal.PtrToStringAnsi(headData);

            Console.WriteLine(helloJsonString);

            FTWrapper.StartResource(resourceId);

            FTWrapper.StopResource(resourceId);

            FTWrapper.AddResource(resourceId, "{\"resourceStatus\":200,\"url\":\""+ FAKE_URL + "\"}", "{}");

            FTWrapper.StopView();
  
            while (true) {
                
            }

        }
    }
}

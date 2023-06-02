using System;
using System.Runtime.InteropServices;
public static class FTWrapper

{

    [DllImport(@"ft-sdkd.dll")]
    public static extern void Install(string jsonConfig);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void InitRUMConfig(string jsonConfig);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void InitLogConfig(string jsonConfig);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void InitTraceConfig(string jsonConfig);


    [DllImport(@"ft-sdkd.dll")]
    public static extern void BindUserData(string jsonConfig);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void UnbindUserdata();

    [DllImport(@"ft-sdkd.dll")]
    public static extern void AddAction(string actionName, string actionType, long duration, long startTime);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void StartAction(string actionName,string actionType);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void StartView(string actionName);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void StopView();

    [DllImport(@"ft-sdkd.dll")]
    public static extern void AddError(string log,string message,string errorType,string state);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void AddLongTask(string log,long duration);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void StartResource(string resourceId);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void StopResource(string resourceId);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void AddResource(string resourceId,string resourceParams,string netStauts);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void AddLog(string log, string message);

    [DllImport(@"ft-sdkd.dll")]
    public static extern IntPtr GetTraceHeader(string resourceId, string url);

    [DllImport(@"ft-sdkd.dll")]
    public static extern IntPtr GetTraceHeaderWithUrl(string url);

    [DllImport(@"ft-sdkd.dll")]
    public static extern void DeInit();
}
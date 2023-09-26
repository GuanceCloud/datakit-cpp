using System;
using System.Runtime.InteropServices;
public static class FTWrapper

{
    // vcpkg 安装目录 
    //const string dllName = "C:\\{vcpkg_root}\\vcpkg\\installed\\{platform}\\bin\\ft-sdk.dll";
     
    const string dllName = "C:\\Users\\dell\\Desktop\\vcpkg_install\\vcpkg\\installed\\x64-windows\\bin\\ft-sdk.dll";


    [DllImport(dllName)]
    public static extern void Install(string jsonConfig);

    [DllImport(dllName)]
    public static extern void InitRUMConfig(string jsonConfig);

    [DllImport(dllName)]
    public static extern void InitLogConfig(string jsonConfig);

    [DllImport(dllName)]
    public static extern void InitTraceConfig(string jsonConfig);

    [DllImport(dllName)]
    public static extern void BindUserData(string jsonConfig);

    [DllImport(dllName)]
    public static extern void UnBindUserdata();

    [DllImport(dllName)]
    public static extern void AddAction(string actionName, string actionType, long duration, long startTime);

    [DllImport(dllName)]
    public static extern void StartAction(string actionName,string actionType);

    [DllImport(dllName)]
    public static extern void StartView(string actionName);

    [DllImport(dllName)]
    public static extern void StopView();

    [DllImport(dllName)]
    public static extern void AddError(string log,string message,string errorType,string state);

    [DllImport(dllName)]
    public static extern void AddLongTask(string log,long duration);

    [DllImport(dllName)]
    public static extern void StartResource(string resourceId);

    [DllImport(dllName)]
    public static extern void StopResource(string resourceId);

    [DllImport(dllName)]
    public static extern void AddResource(string resourceId,string resourceParams,string netStauts);

    [DllImport(dllName)]
    public static extern void AddLog(string log, string logLevel);

    [DllImport(dllName)]
    public static extern IntPtr GetTraceHeader(string resourceId, string url);

    [DllImport(dllName)]
    public static extern IntPtr GetTraceHeaderWithUrl(string url);

    [DllImport(dllName)]
    public static extern void DeInit();
}
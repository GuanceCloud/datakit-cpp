## 1.0.3 -alpha
1. Add Windows DLL bundling guidance and redist handling.
2. Align build/export workflow for ft-sdk DLL outputs.
3. Bundle Windows runtime DLLs from `windows/third_party` for easier distribution without vcpkg.
4. Add optional export of built `ft-sdk.dll` to `windows/ft-sdk-prebuilt/bin/<CONFIG>`.
5. Add `flutter_dotenv` support and `config/.env` for runtime configuration loading.
6. Switch example config to environment-based values (compile-time or .env) and validate required keys at startup.
7. Update Windows CMake bundling logic to include dependency DLLs and avoid missing DLL errors.

---
# 1.0.2-alpha
1. Optimized synchronization mechanism
2. Added log accumulation strategy cleanup strategy
3. Fixed FTWrapper error EntryPoint

---
# 1.0.1-alpha
1. Fixed the issue of missing field parameters in the wrapper part and null parameter crash issues
2. Fixed the issue of being unable to send Action, Resource, Error, LongTask before starting View
3. Fixed the issue of incorrect custom attribute parameter passing

---
# 1.0.0-alpha
1. Support for logs, user data access, and trace data transmission
2. Support for Windows and Linux



# Changelog

## 0.0.1 - 2026-01-05
- Add Windows DLL bundling guidance and redist handling.
- Align build/export workflow for ft-sdk DLL outputs.
- Bundle Windows runtime DLLs from `windows/third_party` for easier distribution without vcpkg.
- Add optional export of built `ft-sdk.dll` to `windows/ft-sdk-prebuilt/bin/<CONFIG>`.
- Add `flutter_dotenv` support and `config/.env` for runtime configuration loading.
- Switch example config to environment-based values (compile-time or .env) and validate required keys at startup.
- Update Windows CMake bundling logic to include dependency DLLs and avoid missing DLL errors.

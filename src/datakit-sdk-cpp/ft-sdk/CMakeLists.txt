cmake_minimum_required(VERSION 3.14)

# ============ vcpkg toolchain file ============
# Set vcpkg toolchain file if not already set
# This must be done before project() command
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  endif()
  if(DEFINED VCPKG_TOOLCHAIN_FILE AND EXISTS "${VCPKG_TOOLCHAIN_FILE}")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_FILE}" CACHE STRING "vcpkg toolchain file")
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
  else()
    message(WARNING "vcpkg toolchain file not found. Set CMAKE_TOOLCHAIN_FILE or VCPKG_ROOT.")
  endif()
endif()

project(ft-sdk VERSION 1.0.0 LANGUAGES CXX C)

# C++17 standard (stduuid 1.2.3 is C++17-compatible)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable install rules by default to avoid INSTALL target failures in plugin builds.
option(FTSDK_ENABLE_INSTALL "Enable ft-sdk install rules" OFF)
#include("../module.cmake")
#include_directories(${PROJECT_SOURCE_DIR}/../include)
#include ("${VCPKG_CMAKE_SHARE}/restclient-cpp/restclient-cppConfig.cmake")
#include ("${VCPKG_CMAKE_SHARE}/fmt/fmt-config.cmake")

message(STATUS "VCPKG_SHARE_DIR=" ${VCPKG_CMAKE_SHARE})
message(STATUS "libpath=" ${CMAKE_SYSTEM_LIBRARY_PATH})
if(DEFINED BUILD_FROM_VCPKG)
  message(STATUS "build from vcpkg!")
endif()

# Repo-local vcpkg_installed fallback
get_filename_component(REAL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" REALPATH)
set(VCPKG_INSTALLED_DIR "${REAL_SOURCE_DIR}/../../../vcpkg_installed")
if(EXISTS "${VCPKG_INSTALLED_DIR}" AND DEFINED VCPKG_TARGET_TRIPLET)
  list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
  message(STATUS "Added vcpkg installed directory to CMAKE_PREFIX_PATH: ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
  if(NOT DEFINED VCPKG_CMAKE_SHARE AND EXISTS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share")
    set(VCPKG_CMAKE_SHARE "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share")
  endif()
elseif(EXISTS "${VCPKG_INSTALLED_DIR}")
  foreach(TRIPLET "x64-windows" "x86-windows")
    if(EXISTS "${VCPKG_INSTALLED_DIR}/${TRIPLET}")
      list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED_DIR}/${TRIPLET}")
      message(STATUS "Added vcpkg installed directory to CMAKE_PREFIX_PATH: ${VCPKG_INSTALLED_DIR}/${TRIPLET}")
      if(NOT DEFINED VCPKG_CMAKE_SHARE AND EXISTS "${VCPKG_INSTALLED_DIR}/${TRIPLET}/share")
        set(VCPKG_CMAKE_SHARE "${VCPKG_INSTALLED_DIR}/${TRIPLET}/share")
      endif()
      break()
    endif()
  endforeach()
endif()

if(WIN32)
add_definitions(-DFTSDK_EXPORTS)
add_definitions(-D_USRDLL)
add_definitions(/EHa)
add_definitions(-D_WINDLL)
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
# Use CMake's CMAKE_CXX_STANDARD for MSVC; no explicit /std:c++20 override
endif()

set(DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../datakit_sdk_redist/${CMAKE_BUILD_TYPE}")
if(DEFINED VCPKG_CMAKE_SHARE)
  set(CURL_DIR "${VCPKG_CMAKE_SHARE}/curl/")
  set(stduuid_DIR "${VCPKG_CMAKE_SHARE}/stduuid/")
  set(fmt_DIR "${VCPKG_CMAKE_SHARE}/fmt/")
  set(spdlog_DIR "${VCPKG_CMAKE_SHARE}/spdlog/")
  set(nlohmann_json_DIR "${VCPKG_CMAKE_SHARE}/nlohmann_json/")
  set(SQLiteCpp_DIR "${VCPKG_CMAKE_SHARE}/sqlitecpp/")
  set(restclient-cpp_DIR "${VCPKG_CMAKE_SHARE}/restclient-cpp/")
endif()
find_package(CURL CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
#find_package(nlohmann_json REQUIRED CONFIG PATHS "/usr/lib/cmake/")
find_package(nlohmann_json REQUIRED CONFIG)
find_package(SQLiteCpp CONFIG REQUIRED)
find_package(restclient-cpp CONFIG REQUIRED)

#find_package(Filesystem REQUIRED)

#file(GLOB PROJECT_SOURCE "*.cpp")
#file(GLOB PROJECT_HEADER "../include/*.h" "*.h")

message(STATUS "PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR} ALL_CPPs)

add_library (${PROJECT_NAME} SHARED  ${ALL_CPPs})

if(WIN32)
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}d)
  endif()
endif()

if(UNIX)
add_library (${PROJECT_NAME}_static STATIC  ${ALL_CPPs})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_custom_target(
    auto_gen_app_uuid
    COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/autogen.sh ${CMAKE_CURRENT_SOURCE_DIR}/Include/Datakit_UUID.h
)
add_dependencies(${PROJECT_NAME} auto_gen_app_uuid)
add_dependencies(${PROJECT_NAME}_static auto_gen_app_uuid)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "/usr/local/include/")
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/include/")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/)

if(FTSDK_ENABLE_INSTALL)
  install(TARGETS ${PROJECT_NAME} DESTINATION ${DIST_DIR}/lib/${ARCHITECTURE}/)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Include/ DESTINATION ${DIST_DIR}/include)

  if(UNIX)
    install(TARGETS ${PROJECT_NAME}_static DESTINATION ${DIST_DIR}/lib/${ARCHITECTURE}/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/install.sh DESTINATION ${DIST_DIR})
  endif()
endif()

#target_link_libraries(${PROJECT_NAME} "/usr/local/lib/librestclient-cpp.a")
target_link_libraries(${PROJECT_NAME} PRIVATE stduuid SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE restclient-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
#target_link_libraries(${PROJECT_NAME}  PRIVATE std::filesystem)

if(UNIX)
target_link_libraries(${PROJECT_NAME}_static PRIVATE stduuid SQLiteCpp)
target_link_libraries(${PROJECT_NAME}_static PRIVATE restclient-cpp)
target_link_libraries(${PROJECT_NAME}_static PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME}_static PRIVATE nlohmann_json::nlohmann_json)
endif()

if (WIN32)
#find_library(VERSION_LIB version)
target_link_libraries(${PROJECT_NAME} PRIVATE version.lib)
#target_link_libraries(${PROJECT_NAME}_static PRIVATE version.lib)
endif()

if(UNIX)
  set(TARGET_FOLDER ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
elseif(WIN32)
  set(TARGET_FOLDER "$(OutDir)")  # Use MSVC's $(OutDir) for dynamic copying
endif()

# ============ Prebuilt package staging ============
# Copies DLL/LIB + public headers + internal headers into ft-sdk-prebuilt.
set(FTSDK_PREBUILT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../ft-sdk-prebuilt"
  CACHE PATH "Destination root for ft-sdk prebuilt package")
set(FTSDK_PREBUILT_INCLUDE_DIR "${FTSDK_PREBUILT_DIR}/Include")
set(FTSDK_PREBUILT_INTERNAL_DIR "${FTSDK_PREBUILT_DIR}/ft-sdk/src")
set(FTSDK_PREBUILT_BIN_DIR "${FTSDK_PREBUILT_DIR}/bin/$<CONFIG>")
set(FTSDK_PREBUILT_LIB_DIR "${FTSDK_PREBUILT_DIR}/lib/$<CONFIG>")

file(GLOB FTSDK_INTERNAL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${FTSDK_PREBUILT_INCLUDE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${FTSDK_PREBUILT_INTERNAL_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${FTSDK_PREBUILT_BIN_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${FTSDK_PREBUILT_LIB_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${FTSDK_PREBUILT_INCLUDE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:${PROJECT_NAME}>"
    "${FTSDK_PREBUILT_BIN_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_LINKER_FILE:${PROJECT_NAME}>"
    "${FTSDK_PREBUILT_LIB_DIR}"
  COMMENT "Staging ft-sdk prebuilt headers and binaries"
)

if(FTSDK_INTERNAL_HEADERS)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${FTSDK_INTERNAL_HEADERS}
      "${FTSDK_PREBUILT_INTERNAL_DIR}"
  )
endif()



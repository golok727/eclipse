#pragma once

#include "../managers/logmanager.h"
#include "spdlog/spdlog.h"

#define ECLIPSE_DEFAULT_LOGGER_NAME "eclipselogger"

#if defined(ECLIPSE_PLATFORM_WINDOWS)
#define ECLIPSE_BREAK() __debugbreak()
#elif defined(ECLIPSE_PLATFORM_MAC)
#define ECLIPSE_BREAK() __builtin_debugtrap()
#else
#define ECLIPSE_BREAK() __builtin_trap()
#endif

#ifndef ECLIPSE_CONFIG_RELEASE
#define ECLIPSE_LOG_CALL(level, ...)                                           \
  do {                                                                         \
    const auto eclipseLogger = spdlog::get(ECLIPSE_DEFAULT_LOGGER_NAME);        \
    if (eclipseLogger) {                                                        \
      eclipseLogger->level(__VA_ARGS__);                                        \
    }                                                                          \
  } while (false)
#define ECLIPSE_TRACE(...) ECLIPSE_LOG_CALL(trace, __VA_ARGS__)
#define ECLIPSE_DEBUG(...) ECLIPSE_LOG_CALL(debug, __VA_ARGS__)
#define ECLIPSE_INFO(...) ECLIPSE_LOG_CALL(info, __VA_ARGS__)
#define ECLIPSE_WARN(...) ECLIPSE_LOG_CALL(warn, __VA_ARGS__)
#define ECLIPSE_ERROR(...) ECLIPSE_LOG_CALL(error, __VA_ARGS__)
#define ECLIPSE_FATAL(...) ECLIPSE_LOG_CALL(critical, __VA_ARGS__)
#define ECLIPSE_ASSERT(expression, message)                                    \
  do {                                                                         \
    if (!(expression)) {                                                        \
      ECLIPSE_FATAL("ASSERT - {}\n\t{}\n\tin file: {}\n\ton line {}",       \
                    #expression, message, __FILE__, __LINE__);                  \
    }                                                                          \
  } while (false)
#else
#define ECLIPSE_TRACE(...) do { } while (false)
#define ECLIPSE_DEBUG(...) do { } while (false)
#define ECLIPSE_INFO(...) do { } while (false)
#define ECLIPSE_WARN(...) do { } while (false)
#define ECLIPSE_ERROR(...) do { } while (false)
#define ECLIPSE_FATAL(...) do { } while (false)
#define ECLIPSE_ASSERT(expression, message) do { } while (false)
#endif

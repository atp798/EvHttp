/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */
#pragma once

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "NonCopyable.h"

namespace Utilis {

const int kLogMsgLen = 4096;

// FATAL must be the last level, otherwise first line of AppendLog should be
// modified. This is compatible with libevent log level definition
typedef enum eLogLevel {
  LL_DEBUG = 0,
  LL_INFO,
  LL_WARN,
  LL_ERROR,
  LL_FATAL
} LogLevel;

const char kLogLevelStr[][6] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

// use __PRETTY_FUNCTION__(gcc) or __FUNCSIG__ (vs) instead will be more
// powerful
#define LogAppend(level, ...)                                                  \
  Logger::GetInstance().AppendLog(Utilis::level, __FILE__, __FUNCTION__,       \
                                  __LINE__, __VA_ARGS__)

#define LogDebug(...) LogAppend(LL_DEBUG, __VA_ARGS__)
#define LogInfo(...) LogAppend(LL_INFO, __VA_ARGS__)
#define LogWarn(...) LogAppend(LL_WARN, __VA_ARGS__)
#define LogError(...) LogAppend(LL_ERROR, __VA_ARGS__)
#define LogFatal(...) LogAppend(LL_FATAL, __VA_ARGS__)

#ifdef DEBUG
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

// #TODO:
// 1. Add file rotate
// 2. Improve time get / msg buffer/ efficient
// 3. Support flush strategy.
class Logger : Utilis::NonCopyable {
public:
  static Logger &GetInstance();
  void SetFilePrefix(std::string const &prefix);
  void SetLogLevel(LogLevel level);

  // This will start a new thread to handle log output.
  bool StartLogging();
  // When call this, it will force worker to exit, which lead to log missing
  bool StopLogging();
  // It's better not to call this directly, call LogXXX() MACRO instead
  // It will auto log the file name, func name, line number, time happened and
  // line break
  void AppendLog(LogLevel level, const char *file, const char *func, int line,
                 const char *fmt, ...);
  void SimpleAppendLog(LogLevel level, const char *fmt, ...);

private:
  void logWriter();

private:
  std::string filePrefix_{"log"};
  std::string filePath_{""};
  FILE *fp_{nullptr};
  LogLevel level_{LL_DEBUG};

  bool bStop_{true};
  std::mutex muxThread_;
  std::shared_ptr<std::thread> spLogThread_;

  std::mutex muxLog_;
  std::condition_variable cvLog_; // flag to indicate new coming message
  std::list<std::string> logQueue_;
};

} // namespace Utilis

/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <time.h>

#ifdef WIN32
#define localtime_r(pTimet, pTm) localtime_s((pTm), (pTimet))
#endif

namespace Utilis {

Logger *Logger::logger_ = new Logger;

void Logger::SetFilePrefix(std::string const &prefix) {
  if (prefix.empty()) {
    return;
  }
  filePrefix_ = prefix;
}

void Logger::SetLogLevel(LogLevel level) { level_ = level; }

bool Logger::StartLogging() {
  bStop_ = false;
  time_t t;
  time(&t);
  struct tm now;
  localtime_r(&t, &now);
  char nowFmt[64] = {0};
  strftime(nowFmt, sizeof(nowFmt), "%Y%m%d_%H%M%S.log", &now);
  filePath_ = filePrefix_ + nowFmt;
#ifdef WIN32
  errno_t err = fopen_s(&fp_, filePath_.c_str(), "wt+");
  if (err != 0) {
    printf("Error open log file: %s\n", filePath_.c_str());
    return false;
  }
#else
  fp_ = fopen(filePath_.c_str(), "wt+");
  if (fp_ == NULL) {
    printf("Error open log file: %s\n", filePath_.c_str());
    return false;
  }
#endif
  spLogThread_.reset(new std::thread(std::bind(&Logger::logWriter, this)));
  printf("Logging started!\n");
  return true;
}

void Logger::StopLogging() {
  bStop_ = true;
  cvLog_.notify_one();
  spLogThread_->join();
  printf("Logging stopped!\n");
}

void Logger::AppendLog(LogLevel level, const char *file, const char *func,
                       int line, const char *fmt, ...) {
  if (level < level_ || level > LL_FATAL) {
    return;
  }
  if (file == nullptr)
    file = "";
  if (func == nullptr)
    func = "";
  if (fmt == nullptr)
    fmt = "";
  time_t t;
  time(&t);
  struct tm now;
  localtime_r(&t, &now);
  char msg[kLogMsgLen] = {0};
  strftime(msg, sizeof(msg) - 1, "[%Y%m%d-%H%M%S]", &now);

  std::stringstream sstid;
  sstid << std::this_thread::get_id();
  std::string tidstr = sstid.str();
  unsigned long long nTid = std::stoull(tidstr);

  // Get file name, ignore full path
#ifdef WIN32
  const char *pTmp = strrchr(file, '\\');
#else
  const char *pTmp = strrchr(file, '/');
#endif
  const char *pFileStart = pTmp == nullptr ? file : pTmp + 1;

  int len = strlen(msg);
  snprintf(msg + len, sizeof(msg) - len - 1,
           "[%s][%llu:%s:%s:%d]:", kLogLevelStr[level], nTid, pFileStart, func,
           line);

  len = strlen(msg);
  va_list vArgList;
  va_start(vArgList, fmt);
  vsnprintf(msg + len, sizeof(msg) - len - 1, fmt, vArgList);
  va_end(vArgList);

  // Auto Append '\n'
  if ((strlen(msg) < kLogMsgLen - 1) && (msg[strlen(msg) - 1] != '\n')) {
    msg[strlen(msg)] = '\n';
    msg[strlen(msg) + 1] = '\0';
  }

  DEBUGPrintf("Receive log msg: %s\n", msg.c_str());
  {
    std::lock_guard<std::mutex> guard(muxLog_);
    logQueue_.emplace_back(msg);
  }
  cvLog_.notify_one();
}

void Logger::SimpleAppendLog(LogLevel level, const char *fmt, ...) {
  va_list vArgList;
  va_start(vArgList, fmt);
  AppendLog(level, "", "", -1, "", vArgList);
  va_end(vArgList);
}

void Logger::logWriter() {
  if (fp_ == nullptr)
    return;

  while (!bStop_) {
    std::string strMsg;
    {
      std::unique_lock<std::mutex> guard(muxLog_);
      while (logQueue_.empty()) {
        if (bStop_)
          return;
        cvLog_.wait(guard);
      }
      strMsg = logQueue_.front();
      logQueue_.pop_front();
    }
    DEBUGPrintf("Write to log: %s\n", strMsg.c_str());
    fwrite((void *)(strMsg.c_str()), strMsg.length(), 1, fp_);
    fflush(fp_);
  }
}

} // namespace Utilis
/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#ifndef __UTILIS_EXCEPTIONS_H__
#define __UTILIS_EXCEPTIONS_H__

#include <stdexcept>

/// #TODO Add stacktrace
#define EXCEPTION_WRAP(name, ext, base) \
  class name##ext \
    : public base \
  { \
  public: \
    template <typename ... Args> \
    name##ext(Args const & ... args) \
      : base(args ... ) {  } \
    template <typename ... Args> \
    name##ext(int code, Args const & ... args) \
      : base(args ... ) \
      , code_(code) { } \
	~name##ext() noexcept override = default; \
    virtual int GetCode() const {  return code_; } \
  private: \
    int code_ = 0; \
  }

#define RUNTIME_EXCEPTION(name) EXCEPTION_WRAP(name, RTEXCP, std::runtime_error)
#define LOGIC_EXCEPTION(name) EXCEPTION_WRAP(name, LogicEXCP, std::logic_error)
#define EXCEPTION(name) EXCEPTION_WRAP(name, EXCP, std::logic_error)

#endif  // !__UTILIS_EXCEPTIONS_H__

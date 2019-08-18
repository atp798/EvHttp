/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#ifndef __UTILIS_NONCOPYABLE_H__
#define __UTILIS_NONCOPYABLE_H__

namespace Utilis
{
  class NonCopyable
  {
  public:
    NonCopyable(NonCopyable const &) = delete;
    NonCopyable& operator = (NonCopyable const &) = delete;
  protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
  };
}  // namespace Utilis

#endif  // __UTILIS_NONCOPYABLE_H__

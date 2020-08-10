//
// Created by tianpeng.lan on 2019-07-24.
//

#ifndef __UTILIS_INITIALIZE_H_
#define __UTILIS_INITIALIZE_H_

#include <functional>

#include "Exceptions.h"
#include "NonCopyable.h"

namespace Utilis {

RUNTIME_EXCEPTION(Initialize);
/// This can only be used in main file
class Initialize : Utilis::NonCopyable {
public:
  /// The functor implementation should not contain other static object
  /// reference, for the initialize order is not certain
  explicit Initialize(std::function<bool(void)> f,
                      std::function<void(void)> e) throw(InitializeRTEXCP) {
    if (!f()) {
      printf("Initialize failed!\n");
      throw InitializeRTEXCP("Initialize failed!\n");
    }
    exitFunc_ = e;
  }

private:
  std::function<void(void)> exitFunc_;
  Initialize();
  ~Initialize() { exitFunc_(); }
};

} // namespace Utilis
#endif //__UTILIS_INITIALIZE_H_

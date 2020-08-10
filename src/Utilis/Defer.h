/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#ifndef __UTILIS_DEFER_H__
#define __UTILIS_DEFER_H__
#include <functional>
#include "NonCopyable.h"

namespace Utilis
{
/// Due to macro expansion delay mechanism, define twice
#define ON_DEFER_NAME_WRAP(line) DEFER_##line
#define DEFER_NAME_WRAP(line)    ON_DEFER_NAME_WRAP(line)

/// Use this micro to name the defer guard automatically
#define DEFER(func) DeferGuard DEFER_NAME_WRAP(__LINE__)(func) 

class DeferGuard : Utilis::NonCopyable {
public:
	explicit DeferGuard(std::function<void(void)> f) :func_(f), dismiss_(false) {}
	~DeferGuard() {
		if (!dismiss_) {
			func_();
		}
	}
	void Dismiss() { dismiss_ = true; }
private:
	std::function<void(void)> 	func_;
	bool dismiss_ = 			false;
};

} // namespace Utilis
#endif // __UTILIS_DEFER_H__
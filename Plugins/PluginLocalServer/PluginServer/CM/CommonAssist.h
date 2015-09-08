#pragma once

#pragma warning(disable: 4251)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#pragma warning(disable: 4356)
#pragma warning(disable: 4996)

#include <assert.h>
#include <string>
#include <atlstr.h>

#define _CA_BEGIN	namespace CA {
#define _CA_END		};

// ∫Í∂®“Â
#define NORET
#define NOOP

#define ASSERT_RET(p, r) { \
	if(!(p)) { \
	assert(false); \
	return r; \
	} \
	}

#define ASSERT_OP(p, op) { \
	if(!(p)) { \
	assert(false); \
	op; \
	} \
	}
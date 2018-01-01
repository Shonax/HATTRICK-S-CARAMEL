// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef CPPTL_JSON_ASSERTIONS_H_INCLUDED
#define CPPTL_JSON_ASSERTIONS_H_INCLUDED

#include <stdlib.h>
#include <sstream>

#if !defined(JSON_IS_AMALGAMATION)
#include "config.h"
#endif // if !defined(JSON_IS_AMALGAMATION)

/** It should not be possible for a maliciously designed file to
 *  cause an abort() or seg-fault, so these macros are used only
 *  for pre-condition violations and internal logic errors.
 */
#if JSON_USE_EXCEPTION

# define JSON_ASSERT(condition)
# define JSON_FAIL_MESSAGE(message)

#else // JSON_USE_EXCEPTION

# define JSON_ASSERT(condition)
# define JSON_FAIL_MESSAGE(message)

#endif

#define JSON_ASSERT_MESSAGE(condition, message)

#endif // CPPTL_JSON_ASSERTIONS_H_INCLUDED

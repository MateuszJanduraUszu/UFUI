// utils.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_IMPL_UTILS_HPP_
#define _UMLS_IMPL_UTILS_HPP_
#include <crtdbg.h>

// generic assert macro, useful in debug mode
#ifndef _INTERNAL_ASSERT
#define _INTERNAL_ASSERT(_Cond, _Msg)                                   \
    if (!(_Cond)) {                                                     \
        ::_CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, nullptr, _Msg); \
    }
#endif // _INTERNAL_ASSERT

#endif // _UMLS_IMPL_UTILS_HPP_
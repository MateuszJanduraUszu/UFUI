// utils.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _MKUTS_UTILS_HPP_
#define _MKUTS_UTILS_HPP_
#include <mjmem/object_allocator.hpp>
#include <vector>

namespace mjx {
    template <class _Ty>
    using vector = ::std::vector<_Ty, object_allocator<_Ty>>;
} // namespace mjx

#endif // _MKUTS_UTILS_HPP_
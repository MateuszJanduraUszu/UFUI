// color.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _URE_IMPL_COLOR_HPP_
#define _URE_IMPL_COLOR_HPP_
#include <cmath>

namespace mjx {
    namespace ure_impl {
        inline float _Min_float(const float _Val0, const float _Val1, const float _Val2) noexcept {
            return ::fminf(::fminf(_Val0, _Val1), _Val2);
        }

        inline float _Max_float(const float _Val0, const float _Val1, const float _Val2) noexcept {
            return ::fmaxf(::fmaxf(_Val0, _Val1), _Val2);
        }
    } // namespace ure_impl
} // namespace mjx

#endif // _URE_IMPL_COLOR_HPP_
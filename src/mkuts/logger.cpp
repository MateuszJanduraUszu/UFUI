// logger.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <mkuts/logger.hpp>
#include <mkuts/tinywin.hpp>

namespace mjx {
    void _Write_unicode_console(const unicode_string_view _Str) noexcept {
        void* const _Handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        ::WriteConsoleW(_Handle, _Str.data(),
#ifdef _M_X64
            static_cast<unsigned long>(_Str.size()),
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
            _Str.size(),
#endif // _M_X64
                nullptr, nullptr);
        if (!_Str.ends_with(L'\n')) { // break the line
            ::WriteConsoleW(_Handle, "\n", 1, nullptr, nullptr);
        }
    }
} // namespace mjx
// logger.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _MKUTS_LOGGER_HPP_
#define _MKUTS_LOGGER_HPP_
#include <cstdio>
#include <mjstr/string.hpp>
#include <mjstr/string_view.hpp>
#include <type_traits>

namespace mjx {
    void _Write_unicode_console(const unicode_string_view _Str) noexcept;

    template <class... _Types>
    inline unicode_string _Format_string(const unicode_string_view _Fmt, const _Types&... _Args) {
        if constexpr (sizeof...(_Types) > 0) { // format the string with the given arguments
            const size_t _Buf_size = static_cast<size_t>(::swprintf(nullptr, 0, _Fmt.data(), _Args...));
            unicode_string _Buf(_Buf_size, L'\0');
            ::swprintf(_Buf.data(), _Buf_size + 1, _Fmt.data(), _Args...);
            return ::std::move(_Buf);
        } else { // arguments not specified, don't format the string
            return _Fmt;
        }
    }

    template <class... _Types>
    inline void rtlog(const unicode_string_view _Fmt, const _Types&... _Args) {
        // write formatted message to the runtime log
        _Write_unicode_console(_Format_string(_Fmt, _Args...));
    }
} // namespace mjx

#endif // _MKUTS_LOGGER_HPP_
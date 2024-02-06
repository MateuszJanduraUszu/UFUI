// format.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _ULS_FORMAT_HPP_
#define _ULS_FORMAT_HPP_
#include <mjmem/object_allocator.hpp>
#include <mjstr/string.hpp>
#include <mjstr/string_view.hpp>
#include <uls/api.hpp>
#include <type_traits>
#include <vector>

namespace mjx {
    class _ULS_API format_args { // stores format arguments used in string formatting
    public:
        format_args() noexcept              = default;
        format_args(const format_args&)     = default;
        format_args(format_args&&) noexcept = default;
        ~format_args() noexcept             = default;

        format_args& operator=(const format_args&)     = default;
        format_args& operator=(format_args&&) noexcept = default;

        // returns the number of arguments
        size_t count() const noexcept;

        // returns the specified argument
        unicode_string_view get(const size_t _Idx) const;

        // reserves storage for future arguments
        void reserve(const size_t _New_capacity);

        // appends a new argument
        void append(const unicode_string_view _Arg);

    private:
        using _Alloc  = object_allocator<unicode_string_view>;
        using _Vector = ::std::vector<unicode_string_view, _Alloc>;
    
#pragma warning(suppress : 4251) // C4251: std::vector needs to have dll-interface
        _Vector _Myargs;
    };

    template <class... _Types>
    format_args make_format_args(_Types&&... _Vals) {
        static_assert(::std::conjunction_v<::std::is_constructible<unicode_string_view, _Types>...>,
            "Requires a type from which unicode_string_view can be constructed");
        format_args _Args;
        _Args.reserve(sizeof...(_Types)); // reserve space for arguments
        (_Args.append(::std::forward<_Types>(_Vals)), ...);
        return _Args;
    }

    _ULS_API bool is_formattable(const unicode_string_view _Str) noexcept;
    _ULS_API unicode_string format_string(const unicode_string_view _Str, const format_args& _Args);
} // namespace mjx

#endif // _ULS_FORMAT_HPP_
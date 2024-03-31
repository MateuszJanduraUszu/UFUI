// format.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_FORMAT_HPP_
#define _UMLS_FORMAT_HPP_
#include <mjmem/object_allocator.hpp>
#include <mjstr/string.hpp>
#include <mjstr/string_view.hpp>
#include <type_traits>
#include <umls/api.hpp>
#include <vector>

namespace mjx {
    class _UMLS_API format_args { // provides access to all formatting arguments
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
        static_assert(::std::conjunction_v<::std::is_constructible_v<unicode_string_view, _Types>...>,
            "All types must be convertible to unicode_string_view");
        format_args _Args;
        _Args.reserve(sizeof...(_Types)); // reserve space for arguments
        (_Args.append(::std::forward<_Types>(_Vals)), ...);
        return _Args;
    }

    _UMLS_API bool is_formattable(const unicode_string_view _Fmt) noexcept;
    _UMLS_API unicode_string format_string(const unicode_string_view _Fmt, const format_args& _Args);
} // namespace mjx

#endif // _UMLS_FORMAT_HPP_
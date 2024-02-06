// format.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <mjmem/exception.hpp>
#include <uls/format.hpp>
#include <uls/impl/format.hpp>

namespace mjx {
    size_t format_args::count() const noexcept {
        return _Myargs.size();
    }

    unicode_string_view format_args::get(const size_t _Idx) const {
        if (_Idx >= _Myargs.size()) {
            resource_overrun::raise();
        }

        return _Myargs[_Idx];
    }

    void format_args::reserve(const size_t _New_capacity) {
        _Myargs.reserve(_New_capacity);
    }

    void format_args::append(const unicode_string_view _Arg) {
        _Myargs.push_back(_Arg);
    }

    bool is_formattable(const unicode_string_view _Str) noexcept {
        // search for at least one valid format specifier
        if (_Str.empty()) { // definitely not formattable
            return false;
        }

        using _Traits              = unicode_string_view::traits_type;
        const wchar_t* _First      = _Str.data();
        const wchar_t* const _Last = _First + _Str.size();
        size_t _Off;
        for (; _First < _Last;) {
            _Off = _Traits::find(_First, _Last - _First, L"{%", 2);
            if (_Off == uls_impl::_Not_found) { // definitely no format specifiers
                break;
            }

            _First += _Off + 2; // skip "{%"
            if (uls_impl::_Validate_format_specifier(_First, _Last)) { // found valid format specifier
                return true;
            }
        }

        return false; // not formattable
    }

    unicode_string format_string(const unicode_string_view _Str, const format_args& _Args) {
        if (_Str.empty()) { // no formatting
            return unicode_string{};
        }

        unicode_string _Fmt_str;
        _Fmt_str.reserve(uls_impl::_Estimate_formatted_string_length(_Str.size(), _Args));
        const wchar_t* _First      = _Str.data();
        const wchar_t* const _Last = _First + _Str.size();
        uls_impl::_Fmt_spec _Spec;
        for (;;) {
            _Spec = uls_impl::_Find_format_specifier(_First, _Last);
            if (!_Spec._Found()) { // no more specifiers, append the rest of the string and break
                _Fmt_str.append(_First, _Last - _First);
                break;
            }

            if (_Spec._Idx >= _Args.count()) { // requested argument not provided, break
                return unicode_string{};
            }

            _Fmt_str.append(_First, _Spec._Off); // append the substring that is before the format specifier
            _Fmt_str.append(_Args.get(_Spec._Idx)); // append the requested argument
            _First += _Spec._Off + _Spec._Len; // skip the format specifier
        }

        _Fmt_str.shrink_to_fit(); // free unused memory
        return ::std::move(_Fmt_str);
    }
} // namespace mjx
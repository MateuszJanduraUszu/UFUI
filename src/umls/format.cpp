// format.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <mjmem/exception.hpp>
#include <umls/format.hpp>
#include <umls/impl/format.hpp>

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

    bool is_formattable(const unicode_string_view _Fmt) noexcept {
        // search for at least one valid format specifier
        if (_Fmt.empty()) { // definitely not formattable
            return false;
        }

        using _Traits              = char_traits<wchar_t>;
        const wchar_t* _First      = _Fmt.data();
        const wchar_t* const _Last = _First + _Fmt.size();
        size_t _Off;
        while (_First < _Last) {
            _Off = _Traits::find(_First, _Last - _First, L"{%", 2);
            if (_Off == umls_impl::_Spec_not_found) { // definitely no format specifiers
                break;
            }

            _First += _Off + 2; // skip '{%'
            if (umls_impl::_Is_valid_format_spec(_First, _Last)) { // valid format specifier found
                return true;
            }
        }

        return false; // not formattable
    }

    unicode_string format_string(const unicode_string_view _Fmt, const format_args& _Args) {
        if (_Fmt.empty()) { // no formatting
            return unicode_string{};
        }

        unicode_string _Fmt_str;
        _Fmt_str.reserve(umls_impl::_Estimate_formatted_string_length(_Fmt.size(), _Args));
        const wchar_t* _First      = _Fmt.data();
        const wchar_t* const _Last = _First + _Fmt.size();
        umls_impl::_Fmt_spec _Spec;
        for (;;) {
            _Spec = umls_impl::_Find_format_spec(_First, _Last);
            if (!_Spec._Found()) { // no more format specifiers, append the rest of the string and break
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
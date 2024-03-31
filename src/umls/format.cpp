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
        (void) _Fmt;
        (void) _Args;
        return unicode_string{}; // not implemented yet
    }
} // namespace mjx
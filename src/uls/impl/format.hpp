// format.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _ULS_IMPL_FORMAT_HPP_
#define _ULS_IMPL_FORMAT_HPP_
#include <mjstr/string_view.hpp>
#include <uls/format.hpp>

namespace mjx {
    namespace uls_impl {
        inline constexpr size_t _Not_found           = static_cast<size_t>(-1);
        inline constexpr size_t _Invalid_index       = static_cast<size_t>(-1);
        inline constexpr size_t _Index_multiplier[3] = {1, 10, 100};

        constexpr bool _Is_digit(const wchar_t _Ch) noexcept {
            return _Ch >= L'0' && _Ch <= L'9';
        }

        constexpr size_t _Chars_to_index(const wchar_t* _Chars, const size_t _Count) noexcept {
            // assumes that _Count is greater than zero, but not more than three
            size_t _Multiplier             = _Index_multiplier[_Count - 1];
            const wchar_t* const _Sentinel = _Chars + _Count;
            size_t _Value                  = 0;
            for (; _Chars != _Sentinel; ++_Chars, _Multiplier /= 10) {
                if (!_Is_digit(*_Chars)) { // index must consist of valid digits
                    return _Invalid_index;
                }

                _Value += static_cast<size_t>(*_Chars - L'0') * _Multiplier;
            }

            return _Value;
        }

        constexpr bool _Validate_format_specifier(const wchar_t* _First, const wchar_t* const _Last) noexcept {
            uint8_t _Digits = 0; // number of index digits processed
            for (; _First != _Last; ++_First) {
                if (*_First == L'}') { // found end of the format specifier
                    if (_Digits > 0) { // index must consist of at least one digit
                        return true;
                    }
                } else if (_Is_digit(*_First)) { // found index digit
                    if (++_Digits > 3) { // index must consist of at most three digits
                        break;
                    }
                } else { // found unrecognized sign, break
                    break;
                }
            }

            return false; // invalid format specifier
        }

        inline size_t _Calculate_args_length(const format_args& _Args) noexcept {
            size_t _Length = 0;
            for (size_t _Idx = 0; _Idx < _Args.count(); ++_Idx) {
                _Length += _Args.get(_Idx).size();
            }

            return _Length;
        }

        inline size_t _Estimate_formatted_string_length(const size_t _Str_size, const format_args& _Args) noexcept {
            return _Str_size + _Calculate_args_length(_Args);
        }

        struct _Fmt_index {
            size_t _Digits    = 0; // number of digits
            wchar_t _Chars[3] = {L'\0'}; // array of digits (at most three digits)
        };

        struct _Fmt_spec {
            size_t _Off = _Not_found; // offset in string
            size_t _Len = 0; // total length
            size_t _Idx = _Invalid_index; // requested argument index

            constexpr bool _Found() const noexcept {
                return _Off != _Not_found;
            }
        };

        inline _Fmt_spec _Find_format_specifier(const wchar_t* const _First, const wchar_t* const _Last) noexcept {
            using _Traits     = char_traits<wchar_t>;
            const size_t _Off = _Traits::find(_First, _Last - _First, L"{%", 2);
            if (_Off == _Not_found) { // definitely no format specifiers
                return _Fmt_spec{};
            }

            const wchar_t* _Spec_first = _First + _Off + 2; // skip "{%"
            _Fmt_index _Idx;
            for (; _Spec_first != _Last; ++_Spec_first) {
                if (*_Spec_first == L'}') { // found end of the format specifier
                    if (_Idx._Digits > 0) { // index must consist of at least one digit
                        break;
                    } else {
                        return _Fmt_spec{};
                    }
                } else if (_Is_digit(*_Spec_first)) { // found index digit
                    if (++_Idx._Digits > 3) { // index must consist of at most three digits
                        return _Fmt_spec{};
                    }

                    _Idx._Chars[_Idx._Digits - 1] = *_Spec_first;
                } else { // found unrecognized sign, break
                    return _Fmt_spec{};
                }
            }

            // format specifier's length is always 3 ("{%" and "}") + number of index digits
            return _Fmt_spec{_Off, 3 + _Idx._Digits, _Chars_to_index(_Idx._Chars, _Idx._Digits)};
        }
    } // namespace uls_impl
} // namespace mjx

#endif // _ULS_IMPL_FORMAT_HPP_
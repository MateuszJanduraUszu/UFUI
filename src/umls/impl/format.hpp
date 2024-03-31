// format.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_IMPL_FORMAT_HPP_
#define _UMLS_IMPL_FORMAT_HPP_
#include <cstdint>
#include <cstddef>
#include <mjstr/string_view.hpp>
#include <umls/format.hpp>

namespace mjx {
    namespace umls_impl {
        inline constexpr size_t _Spec_not_found = static_cast<size_t>(-1);
        inline constexpr size_t _Invalid_index  = static_cast<size_t>(-1);
    
        constexpr bool _Is_digit(const wchar_t _Ch) noexcept {
            return _Ch >= L'0' && _Ch <= L'9';
        }

        constexpr size_t _Chars_to_index(const wchar_t* _Chars, const size_t _Count) noexcept {
            // assumes that _Count is greater than zero and not more than three
            const wchar_t* const _Sentinel = _Chars + _Count;
            size_t _Value                  = 0;
            for (; _Chars != _Sentinel; ++_Chars) {
                if (!_Is_digit(*_Chars)) { // index must consist only of digits
                    return _Invalid_index;
                }

                _Value = _Value * 10 + static_cast<size_t>(*_Chars - L'0');
            }

            return _Value;
        }

        constexpr bool _Is_valid_format_spec(const wchar_t* _First, const wchar_t* const _Last) noexcept {
            uint8_t _Digits = 0; // number of digits processed
            for (; _First != _Last; ++_First) {
                if (*_First == L'}') { // end of the format specifier found
                    if (_Digits > 0) { // index must consist of at least one digit
                        return true;
                    }
                } else if (_Is_digit(*_First)) { // digit found
                    if (++_Digits > 3) { // index must consist of at most three digits
                        break;
                    }
                } else { // unexpected character found, break
                    break;
                }
            }

            return false; // invalid format specifier
        }

        struct _Fmt_index {
            size_t _Digits    = 0; // number of digits
            wchar_t _Chars[3] = {L'\0'}; // array of digits, with a maximum of three digits
        };

        struct _Fmt_spec {
            size_t _Off = _Spec_not_found; // offset in string
            size_t _Len = 0; // total specifier length
            size_t _Idx = _Invalid_index; // requested argument index
        
            constexpr bool _Found() const noexcept {
                return _Off != _Spec_not_found;
            }
        };

        inline _Fmt_spec _Find_format_spec(const wchar_t* const _First, const wchar_t* const _Last) noexcept {
            using _Traits     = char_traits<wchar_t>;
            const size_t _Off = _Traits::find(_First, _Last - _First, L"{%", 2);
            if (_Off == _Spec_not_found) { // definitely no format specifiers
                return _Fmt_spec{};
            }

            const wchar_t* _Spec_first = _First + _Off + 2; // skip '{%'
            _Fmt_index _Idx;
            for (; _Spec_first != _Last; ++_Spec_first) {
                if (*_Spec_first == L'}') { // end of the format specifier found
                    if (_Idx._Digits > 0) {
                        break;
                    } else { // index must consist of at least one digit
                        return _Fmt_spec{};
                    }
                } else if (_Is_digit(*_Spec_first)) {
                    if (++_Idx._Digits > 3) { // index must consist of at most three digits
                        return _Fmt_spec{};
                    }

                    _Idx._Chars[_Idx._Digits - 1] = *_Spec_first;
                } else { // unexpected character found, break
                    return _Fmt_spec{};
                }
            }

            // format specifier's length is always 3 ('{%' and '}') + the number of index digits
            return _Fmt_spec{_Off, 3 + _Idx._Digits, _Chars_to_index(_Idx._Chars, _Idx._Digits)};
        }
    } // namespace umls_impl
} // namespace mjx

#endif // _UMLS_IMPL_FORMAT_HPP_
// color.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <cmath>
#include <ure/color.hpp>
#include <ure/impl/color.hpp>
#include <ure/impl/utils.hpp>

namespace mjx {
    bool operator==(const rgb& _Left, const rgb& _Right) noexcept {
        return _Left.r == _Right.r && _Left.g == _Right.g && _Left.b == _Right.b;
    }

    bool operator!=(const rgb& _Left, const rgb& _Right) noexcept {
        return !(_Left == _Right);
    }

    bool operator==(const rgba& _Left, const rgba& _Right) noexcept {
        return _Left.r == _Right.r && _Left.g == _Right.g && _Left.b == _Right.b && _Left.a == _Right.a;
    }

    bool operator!=(const rgba& _Left, const rgba& _Right) noexcept {
        return !(_Left == _Right);
    }

    bool operator==(const hsv& _Left, const hsv& _Right) noexcept {
        return _Left.h == _Right.h && _Left.s == _Right.s && _Left.v == _Right.v;
    }

    bool operator!=(const hsv& _Left, const hsv& _Right) noexcept {
        return !(_Left == _Right);
    }

    bool operator==(const hsva& _Left, const hsva& _Right) noexcept {
        return _Left.h == _Right.h && _Left.s == _Right.s && _Left.v == _Right.v && _Left.a == _Right.a;
    }

    bool operator!=(const hsva& _Left, const hsva& _Right) noexcept {
        return !(_Left == _Right);
    }

    color::color(const uint32_t _Val) noexcept : _Myval(_Val) {}

    color color::from_rgb(const rgb& _Rgb) noexcept {
        return color{
            static_cast<uint32_t>(_Rgb.r) << 24 |
            static_cast<uint32_t>(_Rgb.g) << 16 |
            static_cast<uint32_t>(_Rgb.b) << 8
        };
    }

    color color::from_rgba(const rgba& _Rgba) noexcept {
        return color{
            static_cast<uint32_t>(_Rgba.r) << 24 |
            static_cast<uint32_t>(_Rgba.g) << 16 |
            static_cast<uint32_t>(_Rgba.b) << 8 |
            static_cast<uint32_t>(_Rgba.a)
        };
    }

    color color::from_hsv(const hsv& _Hsv) noexcept {
#ifdef _DEBUG
        _INTERNAL_ASSERT(_Hsv.h >= 0.0f && _Hsv.h <= 360.0f, "invalid H value");
        _INTERNAL_ASSERT(_Hsv.s >= 0.0f && _Hsv.s <= 1.0f, "invalid S value");
        _INTERNAL_ASSERT(_Hsv.v >= 0.0f && _Hsv.v <= 1.0f, "invalid V value");
#endif // _DEBUG
        const float _Cx = _Hsv.v * _Hsv.s;
        const float _Xx = _Cx * (1.0f - ::fabsf(::fmodf(_Hsv.h / 60.0f, 2.0f) - 1.0f));
        const float _Mx = _Hsv.v - _Cx;
        float _Rx       = 0.0f;
        float _Gx       = 0.0f;
        float _Bx       = 0.0f;
        if (_Hsv.h < 60.0f) {
            // _Bx already stores 0.0f
            _Rx = _Cx;
            _Gx = _Xx;
        } else if (_Hsv.h < 120.0f) {
            // _Bx already stores 0.0f
            _Rx = _Xx;
            _Gx = _Cx;
        } else if (_Hsv.h < 180.0f) {
            // _Rx already stores 0.0f
            _Gx = _Cx;
            _Bx = _Xx;
        } else if (_Hsv.h < 240.0f) {
            // _Rx already stores 0.0f
            _Gx = _Xx;
            _Bx = _Cx;
        } else if (_Hsv.h < 300.0f) {
            // _Gx already stores 0.0f
            _Rx = _Xx;
            _Bx = _Cx;
        } else {
            // _Gx already stores 0.0f
            _Rx = _Cx;
            _Bx = _Xx;
        }

        return color{
            static_cast<uint32_t>((_Rx + _Mx) * 255.0f) << 24 |
            static_cast<uint32_t>((_Gx + _Mx) * 255.0f) << 16 |
            static_cast<uint32_t>((_Bx + _Mx) * 255.0f) << 8
        };
    }

    color color::from_hsva(const hsva& _Hsva) noexcept {
        color _Color   = from_hsv(hsv{_Hsva.h, _Hsva.s, _Hsva.v});
        _Color._Myval |= static_cast<uint32_t>(_Hsva.a * 255.0f);
        return _Color;
    }

    color color::from_hex(const hex _Hex) noexcept {
        return color{_Hex};
    }

    bool color::is_transparent() const noexcept {
        return (_Myval & 0x0000'00FF) == 0;
    }

    bool color::is_opaque() const noexcept {
        return (_Myval & 0x0000'00FF) == 255;
    }

    rgb color::to_rgb() const noexcept {
        return rgb{
            static_cast<uint8_t>((_Myval & 0xFF00'0000) >> 24),
            static_cast<uint8_t>((_Myval & 0x00FF'0000) >> 16),
            static_cast<uint8_t>((_Myval & 0x0000'FF00) >> 8)
        };
    }

    rgba color::to_rgba() const noexcept {
        return rgba{
            static_cast<uint8_t>((_Myval & 0xFF00'0000) >> 24),
            static_cast<uint8_t>((_Myval & 0x00FF'0000) >> 16),
            static_cast<uint8_t>((_Myval & 0x0000'FF00) >> 8),
            static_cast<uint8_t>(_Myval & 0x0000'00FF)
        };
    }

    hsv color::to_hsv() const noexcept {
        const float _Rx    = static_cast<float>((_Myval & 0xFF00'0000) >> 24) / 255.0f;
        const float _Gx    = static_cast<float>((_Myval & 0x00FF'0000) >> 16) / 255.0f;
        const float _Bx    = static_cast<float>((_Myval & 0x0000'FF00) >> 8) / 255.0f;
        const float _Min   = ure_impl::_Min_float(_Rx, _Gx, _Bx);
        const float _Max   = ure_impl::_Max_float(_Rx, _Gx, _Bx);
        const float _Delta = _Max - _Min;
        hsv _Hsv           = {0.0f, 0.0f, _Max};
        if (_Delta == 0.0f) {
            _Hsv.h = 0.0f;
        } else {
            if (_Max == _Rx) {
                _Hsv.h = 60.0f * ::fmodf((_Gx - _Bx) / _Delta, 6.0f);
            } else if (_Max == _Gx) {
                _Hsv.h = 60.0f * (((_Bx - _Rx) / _Delta) + 2.0f);
            } else if (_Max == _Bx) {
                _Hsv.h = 60.0f * (((_Rx - _Gx) / _Delta) + 4.0f);
            }

            if (_Hsv.h < 0.0f) { // correct Hue value
                _Hsv.h += 360.0f;
            }
        }

        if (_Max == 0.0f) {
            _Hsv.s = 0.0f;
        } else {
            _Hsv.s = _Delta / _Max;
        }

        return _Hsv;
    }

    hsva color::to_hsva() const noexcept {
        const hsv& _Hsv = to_hsv();
        return hsva{_Hsv.h, _Hsv.s, _Hsv.v, static_cast<float>(_Myval & 0x0000'00FF) / 255.0f};
    }

    hex color::to_hex() const noexcept {
        return _Myval;
    }

    bool operator==(const color& _Left, const color& _Right) noexcept {
        return _Left.to_hex() == _Right.to_hex();
    }

    bool operator!=(const color& _Left, const color& _Right) noexcept {
        return _Left.to_hex() != _Right.to_hex();
    }
} // namespace mjx
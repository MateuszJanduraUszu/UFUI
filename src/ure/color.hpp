// color.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _URE_COLOR_HPP_
#define _URE_COLOR_HPP_
#include <cstdint>
#include <ure/api.hpp>

namespace mjx {
    struct rgb {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    _URE_API bool operator==(const rgb& _Left, const rgb& _Right) noexcept;
    _URE_API bool operator!=(const rgb& _Left, const rgb& _Right) noexcept;

    struct rgba {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    _URE_API bool operator==(const rgba& _Left, const rgba& _Right) noexcept;
    _URE_API bool operator!=(const rgba& _Left, const rgba& _Right) noexcept;

    struct hsv {
        float h;
        float s;
        float v;
    };

    _URE_API bool operator==(const hsv& _Left, const hsv& _Right) noexcept;
    _URE_API bool operator!=(const hsv& _Left, const hsv& _Right) noexcept;

    struct hsva {
        float h;
        float s;
        float v;
        float a;
    };

    _URE_API bool operator==(const hsva& _Left, const hsva& _Right) noexcept;
    _URE_API bool operator!=(const hsva& _Left, const hsva& _Right) noexcept;

    using hex = uint32_t;

    class _URE_API color { // any color representation
    public:
        color() noexcept             = default;
        color(const color&) noexcept = default;
        color(color&&) noexcept      = default;
        ~color() noexcept            = default;

        color& operator=(const color&) noexcept = default;
        color& operator=(color&&) noexcept      = default;

        // constructs the color from RGB model
        static color from_rgb(const rgb& _Rgb) noexcept;

        // constructs the color from RGBA model
        static color from_rgba(const rgba& _Rgba) noexcept;

        // constructs the color from HSV model
        static color from_hsv(const hsv& _Hsv) noexcept;

        // constructs the color from HSVA model
        static color from_hsva(const hsva& _Hsva) noexcept;

        // constructs the color from hexadecimal model
        static color from_hex(const hex _Hex) noexcept;

        // checks whether alpha channel is set to 0
        bool is_transparent() const noexcept;
        
        // checks whether alpha channel is set to 255
        bool is_opaque() const noexcept;

        // returns the stored color in RGB format
        rgb to_rgb() const noexcept;

        // returns the stored color in RGBA format
        rgba to_rgba() const noexcept;

        // returns the stored color in HSV format
        hsv to_hsv() const noexcept;

        // returns the stored color in HSVA format
        hsva to_hsva() const noexcept;

        // returns the stored color in hexadecimal format
        hex to_hex() const noexcept;

    private:
        explicit color(const uint32_t _Val) noexcept;

        uint32_t _Myval;
    };

    _URE_API bool operator==(const color& _Left, const color& _Right) noexcept;
    _URE_API bool operator!=(const color& _Left, const color& _Right) noexcept;
} // namespace mjx

#endif // _URE_COLOR_HPP_
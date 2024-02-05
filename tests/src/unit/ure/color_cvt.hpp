// color_cvt.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _TEST_UNIT_URE_COLOR_CVT_HPP_
#define _TEST_UNIT_URE_COLOR_CVT_HPP_
#include <gtest/gtest.h>
#include <ure/color.hpp>

namespace mjx {
    namespace test {
        inline rgba _Make_rgba(const uint8_t _Rx, const uint8_t _Gx, const uint8_t _Bx, const uint8_t _Ax) noexcept {
            // allows rgba{r, g, b, a} construction in macro
            return rgba{_Rx, _Gx, _Bx, _Ax};
        }

        inline void _Test_rgb_to_hsv(const rgb& _Rgb, const hsv& _Expected_hsv) noexcept {
            // to avoid floating-point issues, we must tolerate some very small errors
            const hsv& _Hsv = color::from_rgb(_Rgb).to_hsv();
            EXPECT_NEAR(_Hsv.h, _Expected_hsv.h, 0.001f);
            EXPECT_NEAR(_Hsv.s, _Expected_hsv.s, 0.001f);
            EXPECT_NEAR(_Hsv.v, _Expected_hsv.v, 0.001f);
        }

        inline void _Test_hsv_to_rgb(const rgb& _Expected_rgb) noexcept {
            // to avoid floating-point issues, we must tolerate some very small errors
            const hsv& _Hsv = color::from_rgb(_Expected_rgb).to_hsv();
            const rgb& _Rgb = color::from_hsv(_Hsv).to_rgb();
            EXPECT_NEAR(_Rgb.r, _Expected_rgb.r, 1);
            EXPECT_NEAR(_Rgb.g, _Expected_rgb.g, 1);
            EXPECT_NEAR(_Rgb.b, _Expected_rgb.b, 1);
        }

        TEST(color_cvt, hex_to_rgba) {
            EXPECT_EQ(color::from_hex(0x6A5ACDF0).to_rgba(), _Make_rgba(0x6A, 0x5A, 0xCD, 0xF0));
            EXPECT_EQ(color::from_hex(0x7FFF00AC).to_rgba(), _Make_rgba(0x7F, 0xFF, 0x00, 0xAC));
            EXPECT_EQ(color::from_hex(0xDC143C00).to_rgba(), _Make_rgba(0xDC, 0x14, 0x3C, 0x00));
            EXPECT_EQ(color::from_hex(0x00CED1FF).to_rgba(), _Make_rgba(0x00, 0xCE, 0xD1, 0xFF));
            EXPECT_EQ(color::from_hex(0x9400D312).to_rgba(), _Make_rgba(0x94, 0x00, 0xD3, 0x12));
            EXPECT_EQ(color::from_hex(0xFFD70030).to_rgba(), _Make_rgba(0xFF, 0xD7, 0x00, 0x30));
            EXPECT_EQ(color::from_hex(0xADFF2F5E).to_rgba(), _Make_rgba(0xAD, 0xFF, 0x2F, 0x5E));
            EXPECT_EQ(color::from_hex(0xFF69B4AF).to_rgba(), _Make_rgba(0xFF, 0x69, 0xB4, 0xAF));
            EXPECT_EQ(color::from_hex(0x87CEFA91).to_rgba(), _Make_rgba(0x87, 0xCE, 0xFA, 0x91));
            EXPECT_EQ(color::from_hex(0x32CD32DB).to_rgba(), _Make_rgba(0x32, 0xCD, 0x32, 0xDB));
        }

        TEST(color_cvt, rgba_to_hex) {
            EXPECT_EQ(color::from_rgba(_Make_rgba(0x6A, 0x5A, 0xCD, 0xF0)).to_hex(), 0x6A5ACDF0);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0x7F, 0xFF, 0x00, 0xAC)).to_hex(), 0x7FFF00AC);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0xDC, 0x14, 0x3C, 0x00)).to_hex(), 0xDC143C00);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0x00, 0xCE, 0xD1, 0xFF)).to_hex(), 0x00CED1FF);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0x94, 0x00, 0xD3, 0x12)).to_hex(), 0x9400D312);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0xFF, 0xD7, 0x00, 0x30)).to_hex(), 0xFFD70030);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0xAD, 0xFF, 0x2F, 0x5E)).to_hex(), 0xADFF2F5E);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0xFF, 0x69, 0xB4, 0xAF)).to_hex(), 0xFF69B4AF);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0x87, 0xCE, 0xFA, 0x91)).to_hex(), 0x87CEFA91);
            EXPECT_EQ(color::from_rgba(_Make_rgba(0x32, 0xCD, 0x32, 0xDB)).to_hex(), 0x32CD32DB);
        }

        TEST(color_cvt, rgb_to_hsv) {
            _Test_rgb_to_hsv(rgb{0x6A, 0x5A, 0xCD}, hsv{248.347f, 0.561f, 0.804f});
            _Test_rgb_to_hsv(rgb{0x7F, 0xFF, 0x00}, hsv{90.117f, 1.000f, 1.000f});
            _Test_rgb_to_hsv(rgb{0xDC, 0x14, 0x3C}, hsv{348.000f, 0.909f, 0.862f});
            _Test_rgb_to_hsv(rgb{0x00, 0xCE, 0xD1}, hsv{180.861f, 1.000f, 0.819f});
            _Test_rgb_to_hsv(rgb{0x94, 0x00, 0xD3}, hsv{282.085f, 1.000f, 0.827f});
            _Test_rgb_to_hsv(rgb{0xFF, 0xD7, 0x00}, hsv{50.588f, 1.000f, 1.000f});
            _Test_rgb_to_hsv(rgb{0xAD, 0xFF, 0x2F}, hsv{83.653f, 0.815f, 1.000f});
            _Test_rgb_to_hsv(rgb{0xFF, 0x69, 0xB4}, hsv{330.000f, 0.588f, 1.000f});
            _Test_rgb_to_hsv(rgb{0x87, 0xCE, 0xFA}, hsv{202.956f, 0.460f, 0.980f});
            _Test_rgb_to_hsv(rgb{0x32, 0xCD, 0x32}, hsv{120.000f, 0.756f, 0.803f});
        }

        TEST(color_cvt, hsv_to_rgb) {
            _Test_hsv_to_rgb(rgb{0x6A, 0x5A, 0xCD});
            _Test_hsv_to_rgb(rgb{0x7F, 0xFF, 0x00});
            _Test_hsv_to_rgb(rgb{0xDC, 0x14, 0x3C});
            _Test_hsv_to_rgb(rgb{0x00, 0xCE, 0xD1});
            _Test_hsv_to_rgb(rgb{0x94, 0x00, 0xD3});
            _Test_hsv_to_rgb(rgb{0xFF, 0xD7, 0x00});
            _Test_hsv_to_rgb(rgb{0xAD, 0xFF, 0x2F});
            _Test_hsv_to_rgb(rgb{0xFF, 0x69, 0xB4});
            _Test_hsv_to_rgb(rgb{0x87, 0xCE, 0xFA});
            _Test_hsv_to_rgb(rgb{0x32, 0xCD, 0x32});
        }
    } // namespace test
} // namespace mjx

#endif // _TEST_UNIT_URE_COLOR_CVT_HPP_
// color.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _BENCH_BENCHMARKS_COLOR_HPP_
#define _BENCH_BENCHMARKS_COLOR_HPP_
#include <benchmark/benchmark.h>
#include <ure/color.hpp>
#include <random>

namespace mjx {
    namespace bench {
        inline rgb _Get_random_rgb_color() noexcept {
            ::std::random_device _Device;
            ::std::default_random_engine _Engine(_Device());
            ::std::uniform_int_distribution<uint16_t> _Dist(0, 255);
            return rgb{
                static_cast<uint8_t>(_Dist(_Engine)),
                static_cast<uint8_t>(_Dist(_Engine)),
                static_cast<uint8_t>(_Dist(_Engine))
            };
        }

        const rgb& _Rgb_color = _Get_random_rgb_color();
        const hsv& _Hsv_color = color::from_rgb(_Rgb_color).to_hsv();

        int rgb_to_hsv(const rgb& _Rgb) noexcept {
            color::from_rgb(_Rgb).to_hsv();
            return 0;
        }

        int hsv_to_rgb(const hsv& _Hsv) noexcept {
            color::from_hsv(_Hsv).to_rgb();
            return 0;
        }

        void bm_rgb_to_hsv(benchmark::State& _State) {
            for (const auto& _Step : _State) {
                ::benchmark::DoNotOptimize(rgb_to_hsv(_Rgb_color));
            }
        }

        void bm_hsv_to_rgb(benchmark::State& _State) {
            for (const auto& _Step : _State) {
                ::benchmark::DoNotOptimize(hsv_to_rgb(_Hsv_color));
            }
        }

        BENCHMARK(bm_rgb_to_hsv)->DenseRange(0, 10)->Unit(::benchmark::TimeUnit::kNanosecond);
        BENCHMARK(bm_hsv_to_rgb)->DenseRange(0, 10)->Unit(::benchmark::TimeUnit::kNanosecond);
    } // namespace bench
} // namespace mjx

#endif // _BENCH_BENCHMARKS_COLOR_HPP_
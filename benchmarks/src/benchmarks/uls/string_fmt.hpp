// string_fmt.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _BENCH_BENCHMARKS_ULS_STRING_FMT_HPP_
#define _BENCH_BENCHMARKS_ULS_STRING_FMT_HPP_
#include <benchmark/benchmark.h>
#include <uls/format.hpp>

namespace mjx {
    namespace bench {
        void bm_is_formattable_short(::benchmark::State& _State) {
            for (const auto& _Step : _State) {
                ::benchmark::DoNotOptimize(::mjx::is_formattable(
                    L"The {% 0} sets over the {%1 }, casting a { %1} glow on the {%%2} {%3}."));
            }
        }

        void bm_is_formattable_long(::benchmark::State& _State) {
            for (const auto& _Step : _State) {
                ::benchmark::DoNotOptimize(::mjx::is_formattable(
                    L"As the {% 0} sun began its ascent, { %1} the sky in hues of pink and {%2 }, "
                    L"the bustling {%3%} stirred to life. {%_4} hurried along the {%%5} streets, "
                    L"{%6-} cups in hand, as the aroma of {  %7} baked pastries wafted from {% 8 } bakeries. "
                    L"The distant { % 9 } of honking {%1%0} and distant chatter mingled with the {%11%} of "
                    L"urban life. Meanwhile, in {%12 } parks, early { %13 } found solace amid {%1x4} flowers "
                    L"and {%1G} birds. The city, with its {%16%} tapestry of {%17 }, "
                    L"unfolded a new { % 1 8 } with each passing {%19}."
                ));
            }
        }

        void bm_format_string_short(::benchmark::State& _State) {
            for (const auto& _Step : _State) {
                ::benchmark::DoNotOptimize(::mjx::format_string(
                    L"The old {%0} tree stood tall and {%1}, its {%2} reaching towards the {%3}, "
                    L"whispering stories of {%4} gone by.",
                    ::mjx::make_format_args(L"oak", L"proud", L"branches", L"sky", L"seasons")
                ));
            }
        }

        void bm_format_string_long(::benchmark::State& _State) {
            for (const auto& _Step : _State) {
                ::benchmark::DoNotOptimize(::mjx::format_string(
                    L"As the {%0} dipped below the {%1}, painting the sky with hues of {%2} and pink, "
                    L"a {%3} breeze rustled the {%4}, creating a soothing melody in the {%5} forest. "
                    L"{%6} the trees, a hidden path {%7}, inviting explorers to {%8} on a journey "
                    L"filled with the {%9} of {%10} and the promise of {%11} wonders.",
                    ::mjx::make_format_args(L"sun", L"horizon", L"orange", L"gentle",
                        L"leaves", L"tranquil", L"Amidst", L"unfolded", L"embark", L"magic",
                            L"nature", L"undiscovered")
                ));
            }
        }

        BENCHMARK(bm_is_formattable_short)->DenseRange(0, 10)->Unit(::benchmark::TimeUnit::kNanosecond);
        BENCHMARK(bm_is_formattable_long)->DenseRange(0, 10)->Unit(::benchmark::TimeUnit::kNanosecond);
        BENCHMARK(bm_format_string_short)->DenseRange(0, 10)->Unit(::benchmark::TimeUnit::kNanosecond);
        BENCHMARK(bm_format_string_long)->DenseRange(0, 10)->Unit(::benchmark::TimeUnit::kNanosecond);
    } // namespace bench
} // namespace mjx

#endif // _BENCH_BENCHMARKS_ULS_STRING_FMT_HPP_
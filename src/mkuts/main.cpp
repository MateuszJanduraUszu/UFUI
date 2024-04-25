// main.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <mjmem/exception.hpp>
#include <mjstr/char_traits.hpp>
#include <mkuts/logger.hpp>
#include <mkuts/options.hpp>
#include <mkuts/settings_file.hpp>

namespace mjx {
    inline bool _Should_print_help(int _Count, wchar_t** _Args) noexcept {
        if (_Count == 0) { // no user-provided arguments, help should be printed
            return true;
        }

        using _Traits = char_traits<wchar_t>;
        for (; _Count > 0; --_Count, ++_Args) {
            if (_Traits::eq(*_Args, L"--help", 6) || _Traits::eq(*_Args, L"-h", 2)) { // help flag found
                return true;
            }
        }

        return false;
    }

    inline void _Print_help() noexcept {
        rtlog(
            L"MKUTS usage:\n"
            L"\n"
            L"mkuts.exe [options...]\n"
            L"mkuts.exe --help\n"
            L"\n"
            L"Options:\n"
            L"    --help (or -h)             display this help message and exit\n"
            L"\n"
            L"    --catalog=\"[...]\"          include the specified catalog\n"
            L"    --catalog-dir=\"[...]\"      include all catalogs from the specified directory\n"
            L"    --output-dir=\"[...]\"       set the output directory for the created settings file\n"
            L"\n"
            L"    --default-lcid=<value>     set the default LCID\n"
            L"    --preferred-lcid=<value>   set the preferred LCID"
        );
    }
} // namespace mjx

int wmain(int _Count, wchar_t** _Args) {
    try {
        // skip the first argument, which is always the path or name of the executable file
        if (::mjx::_Should_print_help(--_Count, ++_Args)) { // print help and exit
            ::mjx::_Print_help();
            return 0;
        }

        ::mjx::parse_program_args(_Count, _Args);
        ::mjx::create_or_overwrite_settings_file();
        return 0;
    } catch (const ::mjx::allocation_failure&) {
        ::mjx::rtlog(L"Error: Insufficient memory to complete the operation.");
        return -1;
    } catch (...) {
        ::mjx::rtlog(L"Error: An unknown error occured.");
        return -2;
    }
}
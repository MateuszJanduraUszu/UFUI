// options.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _MKUTS_OPTIONS_HPP_
#define _MKUTS_OPTIONS_HPP_
#include <cstdint>
#include <mjfs/path.hpp>
#include <mjstr/string_view.hpp>
#include <mkuts/utils.hpp>

namespace mjx {
    class program_options {
    public:
        vector<path> catalogs;
        path output_dir;
        uint32_t default_lcid;
        uint32_t preferred_lcid;

        ~program_options() noexcept;

        program_options(const program_options&)            = delete;
        program_options& operator=(const program_options&) = delete;

        // returns the global instance of the program options
        static program_options& global() noexcept;
    
    private:
        program_options() noexcept;
    };
    
    bool _Is_catalog_included(const path& _Path) noexcept;
    path _Absolute_path(const path& _Path);
    void _Check_lcids() noexcept;

    struct _Options_parser {
        // parses '--catalog' option
        static void _Parse_catalog(const unicode_string_view _Value);

        // parses '--catalog-dir' option
        static void _Parse_catalog_directory(const unicode_string_view _Value);

        // parses '--output-dir' option
        static void _Parse_output_directory(const unicode_string_view _Value);

        // parses '--default-lcid' or '--preferred-lcid' option
        static void _Parse_lcid(const unicode_string_view _Value, uint32_t& _Lcid) noexcept;
    };

    void parse_program_args(int _Count, wchar_t** _Args);
} // namespace mjx

#endif // _MKUTS_OPTIONS_HPP_
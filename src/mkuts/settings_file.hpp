// settings_file.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _MKUTS_SETTINGS_FILE_HPP_
#define _MKUTS_SETTINGS_FILE_HPP_
#include <cstdint>
#include <mjfs/file_stream.hpp>
#include <mjfs/path.hpp>
#include <mkuts/utils.hpp>

namespace mjx {
    size_t _Unicode_to_utf8_required_buffer_size(const unicode_string_view _Str) noexcept;
    bool _Unicode_to_utf8(
        const unicode_string_view _Str, byte_t* const _Buf, const size_t _Buf_size) noexcept;

    struct _Uts_catalog_name_traits {
        static constexpr size_t _Space_size      = 64;
        static constexpr size_t _Max_name_length = _Space_size - 1; // minus null-terminator
    };

    struct _Uts_catalog {
        byte_t _Name[_Uts_catalog_name_traits::_Space_size] = {'\0'};
        uint32_t _Lcid                                      = 0;
    };

    struct _Uts_catalog_creator {
        // creates a UTS catalog from the UMC one
        static bool _Create(const path& _Target, _Uts_catalog& _Catalog);
    
    private:
        // reads the associated LCID
        static bool _Get_associated_lcid(const path& _Target, uint32_t& _Lcid);
    };

    class _Uts_file_writer {
    public:
        explicit _Uts_file_writer(file_stream& _Stream) noexcept;
        ~_Uts_file_writer() noexcept;

        _Uts_file_writer()                                   = delete;
        _Uts_file_writer(const _Uts_file_writer&)            = delete;
        _Uts_file_writer& operator=(const _Uts_file_writer&) = delete;

        // returns the number of catalogs within valid range
        static size_t _Checked_catalog_count(size_t _Count) noexcept;

        // writes the signature to the UTS file
        bool _Write_signature() noexcept;

        // writes LCIDs to the UTS file
        bool _Write_lcids(const uint32_t _Default, const uint32_t _Preferred) noexcept;

        // writes a number of catalogs to the UTS file
        bool _Write_catalog_count(const size_t _Count) noexcept;

        // writes catalogs to the UTS file
        bool _Write_catalogs(const vector<_Uts_catalog>& _Catalogs);
        
    private:
        file_stream& _Mystream;
    };

    bool _Make_uts_catalogs_from_umc(const vector<path>& _Umc_catalogs, vector<_Uts_catalog>& _Uts_catalogs);
    void _Write_settings_file(file_stream& _Stream);
    void _Create_settings_file(const path& _Target);
    void _Overwrite_settings_file(const path& _Target);

    void create_or_overwrite_settings_file();
} // namespace mjx

#endif // _MKUTS_SETTINGS_FILE_HPP_
// settings_file.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <mjfs/file.hpp>
#include <mjfs/status.hpp>
#include <mjmem/smart_pointer.hpp>
#include <mkuts/logger.hpp>
#include <mkuts/options.hpp>
#include <mkuts/settings_file.hpp>
#include <mkuts/tinywin.hpp>

namespace mjx {
    size_t _Unicode_to_utf8_required_buffer_size(const unicode_string_view _Str) noexcept {
        if (_Str.empty()) { // empty string, no conversion needed
            return 0;
        }

        const int _Req_size = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
            _Str.data(), static_cast<int>(_Str.size()), nullptr, 0, nullptr, nullptr);
        return _Req_size > 0 ? static_cast<size_t>(_Req_size) : static_cast<size_t>(-1);
    }

    bool _Unicode_to_utf8(const unicode_string_view _Str, byte_t* const _Buf, const size_t _Buf_size) noexcept {
        return static_cast<size_t>(::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
            _Str.data(), static_cast<int>(_Str.size()), reinterpret_cast<char*>(_Buf),
                static_cast<int>(_Buf_size), nullptr, nullptr)) == _Buf_size;
    }

    bool _Uts_catalog_creator::_Get_associated_lcid(const path& _Target, uint32_t& _Lcid) {
        file _File(_Target, file_access::read, file_share::read);
        file_stream _Stream(_File);
        if (!_Stream.is_open()) { // invalid stream, break
            return false;
        }

        // Note: The LCID is the fourth component of the UMC header, immediately following
        //       the variable-length language name. Thus, to access the LCID, we need to
        //       determine the length of the language name first and skip over it.
        uint64_t _Off = 4; // set offset to language name length component
        if (!_Stream.seek(_Off)) {
            return false;
        }

        uint8_t _Len;
        if (!_Stream.read_exactly(&_Len, sizeof(uint8_t))) {
            return false;
        }

        _Off += 1 + static_cast<uint64_t>(_Len); // set offset to the LCID component
        if (!_Stream.seek(_Off)) {
            return false;
        }

        return _Stream.read_exactly(reinterpret_cast<byte_t*>(&_Lcid), sizeof(uint32_t));
    }

    bool _Uts_catalog_creator::_Create(const path& _Target, _Uts_catalog& _Catalog) {
        const unicode_string _Name = _Target.filename().native();
        const size_t _Cvt_size     = _Unicode_to_utf8_required_buffer_size(_Name);
        if (_Cvt_size > _Uts_catalog_name_traits::_Max_name_length) { // catalog name is too long, break
            return false;
        }

        if (!_Unicode_to_utf8(_Name, _Catalog._Name, _Cvt_size)) { // conversion failed, break
            return false;
        }
        
        return _Get_associated_lcid(_Target, _Catalog._Lcid);
    }

    _Uts_file_writer::_Uts_file_writer(file_stream& _Stream) noexcept : _Mystream(_Stream) {}

    _Uts_file_writer::~_Uts_file_writer() noexcept {}

    size_t _Uts_file_writer::_Checked_catalog_count(size_t _Count) noexcept {
        constexpr size_t _Max = 0xFFFF;
        if (_Count > _Max) { // won't fit in 2-byte integer, trim
            _Count = _Max;
            rtlog(L"Warning: Requested too many catalogs, trimmed to %zu.", _Max);
        }

        return _Count;
    }

    bool _Uts_file_writer::_Write_signature() noexcept {
        // write predefined 4-byte signature to the file
        constexpr size_t _Signature_size             = 4;
        constexpr byte_t _Signature[_Signature_size] = { 'U', 'T', 'S', '\0'};
        return _Mystream.write(_Signature, _Signature_size);
    }

    bool _Uts_file_writer::_Write_lcids(const uint32_t _Default, const uint32_t _Preferred) noexcept {
        // write default and preferred LCIDs to the file
        constexpr size_t _Bytes_per_lcid = sizeof(uint32_t);
        constexpr size_t _Buf_size       = 2 * _Bytes_per_lcid; // must fit two LCIDs
        byte_t _Buf[_Buf_size];
        ::memcpy(_Buf, &_Default, _Bytes_per_lcid);
        ::memcpy(_Buf + _Bytes_per_lcid, &_Preferred, _Bytes_per_lcid);
        return _Mystream.write(_Buf, _Buf_size);
    }

    bool _Uts_file_writer::_Write_catalog_count(const size_t _Count) noexcept {
        // write a number of catalogs to the file, assumes that _Count is in range [0, 65535]
        constexpr size_t _Buf_size = 2;
        byte_t _Buf[_Buf_size];
        ::memcpy(_Buf, reinterpret_cast<const uint16_t*>(&_Count), _Buf_size); // copy as 2-byte integer
        return _Mystream.write(_Buf, _Buf_size);
    }

    bool _Uts_file_writer::_Write_catalogs(const vector<_Uts_catalog>& _Catalogs) {
        constexpr size_t _Bytes_per_catalog = sizeof(_Uts_catalog);
        const size_t _Buf_size              = _Catalogs.size() * _Bytes_per_catalog;
        unique_smart_array<byte_t> _Buf     = ::mjx::make_unique_smart_array<byte_t>(_Buf_size);
        for (size_t _Off = 0; const _Uts_catalog& _Catalog : _Catalogs) {
            ::memcpy(_Buf.get() + _Off, reinterpret_cast<const byte_t*>(&_Catalog), _Bytes_per_catalog);
            _Off += _Bytes_per_catalog;
        }

        return _Mystream.write(_Buf.get(), _Buf_size);
    }

    bool _Make_uts_catalogs_from_umc(const vector<path>& _Umc_catalogs, vector<_Uts_catalog>& _Uts_catalogs) {
        _Uts_catalogs.reserve(_Umc_catalogs.size());
        _Uts_catalog _Uts_catalog;
        for (const path& _Umc_catalog : _Umc_catalogs) {
            if (!_Uts_catalog_creator::_Create(_Umc_catalog, _Uts_catalog)) { // creation failed, break
                rtlog(L"Error: Failed to create a UTS catalog from '%s'.", _Umc_catalog.c_str());
                return false;
            }

            _Uts_catalogs.push_back(::std::move(_Uts_catalog));
        }

        return true;
    }

    void _Write_settings_file(file_stream& _Stream) {
        const program_options& _Options = program_options::global();
        vector<_Uts_catalog> _Catalogs;
        if (!_Make_uts_catalogs_from_umc(_Options.catalogs, _Catalogs)) {
            rtlog(L"Error: Failed to create UTS catalogs.");
            return;
        }

        _Uts_file_writer _Writer(_Stream);
        if (!_Writer._Write_signature()) {
            rtlog(L"Error: Failed to write the signature.");
            return;
        }
        
        if (!_Writer._Write_lcids(_Options.default_lcid, _Options.preferred_lcid)) {
            rtlog(L"Error: Failed to write the LCIDs.");
            return;
        }

        // Note: A number of catalogs is limited to 65535 due to the 2-byte section reserved for
        //       this information. When the number of catalogs exceeds this limit, we must trim
        //       the UTS catalogs accordingly. To maintain consistency, the trimming process
        //       starts from the end, resulting in the discarding of the last catalogs.
        const size_t _Unchecked_count = _Catalogs.size();
        const size_t _Count           = _Uts_file_writer::_Checked_catalog_count(_Unchecked_count);
        if (_Count < _Unchecked_count) { // some catalogs must be discarded
            _Catalogs.resize(_Count);
        }

        if (!_Writer._Write_catalog_count(_Count)) {
            rtlog(L"Error: Failed to write the number of catalogs.");
            return;
        }

        if (!_Writer._Write_catalogs(_Catalogs)) {
            rtlog(L"Error: Failed to write the UTS catalogs.");
        }
    }

    void _Create_settings_file(const path& _Target) {
        file _File;
        if (!::mjx::create_file(_Target, ::std::addressof(_File))) { // failed to create the file, break
            rtlog(L"Error: Failed to create the settings file.");
            return;
        }

        file_stream _Stream(_File);
        if (!_Stream.is_open()) { // invalid stream, break
            rtlog(L"Error: Failed to open the settings file.");
            return;
        }

        _Write_settings_file(_Stream);
    }

    void _Overwrite_settings_file(const path& _Target) {
        file _File(_Target, file_access::write);
        file_stream _Stream(_File);
        if (!_Stream.is_open()) { // invalid stream, break
            rtlog(L"Error: Failed to open the settings file.");
            return;
        }

        if (!_File.resize(0)) { // the file must be empty
            rtlog(L"Error: Failed to clear the settings file.");
            return;
        }

        _Write_settings_file(_Stream);
    }

    void create_or_overwrite_settings_file() {
        const path& _Path = program_options::global().output_dir / L"settings.uts";
        if (::mjx::exists(_Path)) { // file already exists, overwrite it
            _Overwrite_settings_file(_Path);
        } else { // file does not exist, create a new one
            _Create_settings_file(_Path);
        }
    }
} // namespace mjx
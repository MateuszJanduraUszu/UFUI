// catalog.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_IMPL_CATALOG_HPP_
#define _UMLS_IMPL_CATALOG_HPP_
#include <mjfs/file.hpp>
#include <mjfs/file_stream.hpp>
#include <mjmem/object_allocator.hpp>
#include <mjmem/smart_pointer.hpp>
#include <mjstr/conversion.hpp>
#include <mjstr/string.hpp>
#include <umls/impl/utils.hpp>
#include <xxhash/xxhash.h>

namespace mjx {
    namespace umls_impl {
        inline uint64_t _Hash_message_id(const utf8_string_view _Id) noexcept {
            return ::XXH3_64bits(_Id.data(), _Id.size());
        }

        template <class _Integer>
        inline _Integer _Load_integer(const byte_t* const _Bytes) noexcept {
            // assumes that _Bytes is at least sizeof(_Integer) bytes long
            _Integer _Value;
            ::memcpy(&_Value, _Bytes, sizeof(_Integer));
            return _Value;
        }

        class _Umc_blob { // stores UMC messages blob
        public:
            _Umc_blob() noexcept : _Mydata(nullptr), _Mysize(0) {}

            ~_Umc_blob() noexcept {
                _Destroy();
            }

            _Umc_blob(const _Umc_blob&)            = delete;
            _Umc_blob& operator=(const _Umc_blob&) = delete;

            bool _Valid() const noexcept {
                return _Mydata != nullptr && _Mysize > 0;
            }

            size_t _Size() const noexcept {
                return _Mysize;
            }

            byte_t* _Data() noexcept {
                return _Mydata;
            }

            const byte_t* _Data() const noexcept {
                return _Mydata;
            }

            bool _Fetch_message(unicode_string& _Str, const size_t _Off, const size_t _Size) const noexcept {
                if (_Off + _Size > _Mysize) { // message exceeds the blob, break
                    return false;
                }

                // messages are stored in UTF-8 encoding, therefore we must decode them to Unicode before use
                _Str = ::mjx::to_unicode_string(byte_string_view{_Mydata + _Off, _Size});
                return true;
            }

            void _Destroy() noexcept {
                if (_Valid()) {
                    ::mjx::delete_object_array(_Mydata, _Mysize);
                    _Mydata = nullptr;
                    _Mysize = 0;
                }
            }

            void _Resize(const size_t _New_size) {
                _Destroy(); // destroy the old blob
                _Mydata = ::mjx::allocate_object_array<byte_t>(_New_size);
                _Mysize = _New_size;
            }

        private:
            byte_t* _Mydata;
            size_t _Mysize;
        };

        class _Umc_lookup_table { // stores UMC lookup table
        public:
#pragma pack(push)
#pragma pack(4) // align structure members on 4-byte boundaries to avoid alignment issues
                // while copying raw data
            struct _Table_entry {
                uint64_t _Hash   = 0;
                uint64_t _Offset = 0;
                uint32_t _Length = 0;
            };
#pragma pack(pop)

            _Umc_lookup_table() noexcept : _Myentries(nullptr), _Mysize(0), _Myoff(0) {}

            ~_Umc_lookup_table() noexcept {
                _Destroy();
            }

            _Umc_lookup_table(const _Umc_lookup_table&)            = delete;
            _Umc_lookup_table& operator=(const _Umc_lookup_table&) = delete;

            size_t _Size() const noexcept {
                return _Mysize;
            }

            const _Table_entry* _At(const size_t _Idx) const noexcept {
#ifdef _DEBUG
                _INTERNAL_ASSERT(_Idx < _Mysize, "attempt to access non-existent table entry");
#endif // _DEBUG
                return &_Myentries[_Idx];
            }

            const _Table_entry* _Find_message(const uint64_t _Hash) const noexcept {
                for (size_t _Idx = 0; _Idx < _Mysize; ++_Idx) {
                    if (_Myentries[_Idx]._Hash == _Hash) {
                        return &_Myentries[_Idx];
                    }
                }

                return nullptr; // not found
            }

            void _Destroy() noexcept {
                if (_Myentries && _Mysize > 0) {
                    ::mjx::delete_object_array(_Myentries, _Mysize);
                    _Myentries = nullptr;
                    _Mysize    = 0;
                    _Myoff     = 0;
                }
            }

            void _Resize(const size_t _New_size) {
                _Destroy(); // destroy the existing table
                _Myentries = ::mjx::allocate_object_array<_Table_entry>(_New_size);
                _Mysize    = _New_size;
            }

            void _Append_entry(const _Table_entry& _Entry) noexcept {
#ifdef _DEBUG
                _INTERNAL_ASSERT(_Myoff < _Mysize, "the table is too small");
#endif // _DEBUG
                _Myentries[_Myoff++] = _Entry;
            }

        private:
            _Table_entry* _Myentries;
            size_t _Mysize;
            size_t _Myoff;
        };

        class _Catalog_loader { // manages a catalog loading process
        public:
            explicit _Catalog_loader(file_stream& _Stream) noexcept : _Mystream(_Stream) {}

            ~_Catalog_loader() noexcept {}

            _Catalog_loader()                                  = delete;
            _Catalog_loader(const _Catalog_loader&)            = delete;
            _Catalog_loader& operator=(const _Catalog_loader&) = delete;

            bool _Verify_signature() noexcept {
                // compare the stored signature with the original
                using _Traits = char_traits<byte_t>;
                byte_t _Buf[_Signature_size];
                return _Mystream.read_exactly(_Buf, _Signature_size)
                    && _Traits::eq(_Buf, _Signature, _Signature_size);
            }

            bool _Get_language_and_lcid(unicode_string& _Language, uint32_t& _Lcid) {
                size_t _Lang_length;
                { // load language name length first
                    uint8_t _Len;
                    if (!_Mystream.read_exactly(&_Len, 1)) {
                        return false;
                    }

                    _Lang_length = static_cast<size_t>(_Len);
                }

                constexpr size_t _Buf_size = 132; // at most 128-byte language + 4-byte LCID
                byte_t _Buf[_Buf_size];
                if (!_Mystream.read_exactly(_Buf, _Lang_length + 4)) {
                    return false;
                }

                _Language = ::mjx::to_unicode_string(byte_string_view{_Buf, _Lang_length});
                _Lcid     = _Load_integer<uint32_t>(_Buf + _Lang_length);
                return true;
            }

            bool _Get_message_count(size_t& _Count) noexcept {
                constexpr size_t _Buf_size = sizeof(uint32_t);
                byte_t _Buf[_Buf_size];
                if (!_Mystream.read_exactly(_Buf, _Buf_size)) {
                    return false;
                }

                _Count = _Load_integer<uint32_t>(_Buf);
                return true;
            }

            bool _Load_lookup_table(const size_t _Count, _Umc_lookup_table& _Table) {
                constexpr size_t _Bytes_per_entry = sizeof(_Umc_lookup_table::_Table_entry);
                const size_t _Buf_size            = _Count * _Bytes_per_entry;
                unique_smart_array<byte_t> _Buf   = ::mjx::make_unique_smart_array<byte_t>(_Buf_size);
                if (!_Mystream.read_exactly(_Buf.get(), _Buf_size)) {
                    return false;
                }

                _Table._Resize(_Count); // reserve space for all entries
                for (size_t _Off = 0; _Off < _Buf_size; _Off += _Bytes_per_entry) {
                    _Table._Append_entry(*reinterpret_cast<_Umc_lookup_table::_Table_entry*>(_Buf.get() + _Off));
                }

                return true;
            }

            bool _Load_blob(_Umc_lookup_table& _Table, _Umc_blob& _Blob) {
                size_t _Blob_size = 0;
                for (size_t _Idx = 0; _Idx < _Table._Size(); ++_Idx) { // calculate blob size
                    _Blob_size += _Table._At(_Idx)->_Length;
                }

                _Blob._Resize(_Blob_size);
                return _Mystream.read_exactly(_Blob._Data(), _Blob_size);
            }

        private:
            static constexpr size_t _Signature_size             = 4;
            static constexpr byte_t _Signature[_Signature_size] = {'U', 'M', 'C', '\0'};

            file_stream& _Mystream;
        };

        class _Message_catalog {
        public:
            unicode_string _Language;
            uint32_t _Lcid;
            _Umc_lookup_table _Table;
            _Umc_blob _Blob;

            explicit _Message_catalog(const path& _Target) : _Language(), _Lcid(0), _Table(), _Blob() {
                if (!_Load_from_file(_Target)) { // failed to load the catalog, erase any loaded data
                    _Erase_data();
                }
            }

            ~_Message_catalog() noexcept {}

            _Message_catalog()                                   = delete;
            _Message_catalog(const _Message_catalog&)            = delete;
            _Message_catalog& operator=(const _Message_catalog&) = delete;

            bool _Valid() const noexcept {
                // Note: _Table and _Blob are not considered invalid when they are empty,
                //       as the catalog might be empty.
                return !_Language.empty() && (_Lcid > 0 && _Lcid <= 0x7FFF'FFFF);
            }

        private:
            bool _Load_from_file(const path& _Target) {
                if (_Target.extension() != L".umc") { // invalid extension, break
                    return false;
                }

                file _File(_Target, file_access::read, file_share::read);
                file_stream _Stream(_File);
                if (!_Stream.is_open()) { // invalid stream, break
                    return false;
                }

                _Catalog_loader _Loader(_Stream);
                if (!_Loader._Verify_signature()) { // signature not recognized, break
                    return false;
                }

                if (!_Loader._Get_language_and_lcid(_Language, _Lcid)) { // failed to load language and LCID, break
                    return false;
                }

                size_t _Count;
                if (!_Loader._Get_message_count(_Count)) { // failed to get the number of messages, break
                    return false;
                }

                if (_Count > 0) { // some messages declared, try to load them
                    if (!_Loader._Load_lookup_table(_Count, _Table) || !_Loader._Load_blob(_Table, _Blob)) {
                        return false;
                    }
                }

                return true; // catalog loaded successfully
            }

            void _Erase_data() noexcept {
                _Language.clear();
                _Language.shrink_to_fit();
                _Lcid = 0;
                _Table._Destroy();
                _Blob._Destroy();
            }
        };
    } // namespace umls_impl
} // namespace mjx

#endif // _UMLS_IMPL_CATALOG_HPP_
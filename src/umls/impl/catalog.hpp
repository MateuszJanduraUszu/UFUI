// catalog.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_IMPL_CATALOG_HPP_
#define _UMLS_IMPL_CATALOG_HPP_
#include <mjmem/object_allocator.hpp>
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

        class _Umc_blob {
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

            bool _Fetch_message(unicode_string& _Str, const size_t _Off, const size_t _Size) noexcept {
                if (_Off + _Size > _Mysize) { // message exceedes the blob, break
                    return false;
                }

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

        class _Umc_lookup_table {
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

        class _Message_catalog {
        public:
            unicode_string _Language;
            uint32_t _Lcid;
            _Umc_lookup_table _Table;
            _Umc_blob _Blob;

            bool _Valid() const noexcept {
                // Note: _Table and _Blob are not considered invalid when they are empty,
                //       as the catalog might be empty.
                return !_Language.empty() && (_Lcid > 0 && _Lcid <= 0x7FFF'FFFF);
            }
        };
    } // namespace umls_impl
} // namespace mjx

#endif // _UMLS_IMPL_CATALOG_HPP_
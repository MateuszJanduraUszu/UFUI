// translator.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_IMPL_TRANSLATOR_HPP_
#define _UMLS_IMPL_TRANSLATOR_HPP_
#include <mjfs/file_stream.hpp>
#include <mjfs/path.hpp>
#include <mjmem/smart_pointer.hpp>
#include <mjstr/conversion.hpp>
#include <umls/translator.hpp>

namespace mjx {
    namespace umls_impl {
        inline constexpr size_t _Uts_signature_size                 = 4;
        inline constexpr byte_t _Uts_signature[_Uts_signature_size] = {'U', 'T', 'S', '\0'};

#pragma pack(push)
#pragma pack(2) // align to 2-byte boundaries to maintain a total structure size of 14 bytes
        struct _Uts_static_data {
            byte_t _Signature[_Uts_signature_size];
            uint32_t _Default_lcid;
            uint32_t _Preferred_lcid;
            uint16_t _Catalog_count;
        };
#pragma pack(pop)

        inline bool _Verify_uts_signature(const _Uts_static_data& _Data) noexcept {
            // compare the stored signature with the original
            using _Traits = char_traits<byte_t>;
            return _Traits::eq(_Data._Signature, _Uts_signature, _Uts_signature_size);
        }

        template <class _Integer>
        _Integer _Load_integer(const byte_t* const) noexcept; // defined in <impl/catalog.hpp>

        class _Uts_file_loader { // manages loading process of a UTS file
        public:
            explicit _Uts_file_loader(file_stream& _Stream) noexcept : _Mystream(_Stream) {}

            ~_Uts_file_loader() noexcept {}

            _Uts_file_loader()                                   = delete;
            _Uts_file_loader(const _Uts_file_loader&)            = delete;
            _Uts_file_loader& operator=(const _Uts_file_loader&) = delete;

            bool _Load_static_data(_Uts_static_data& _Data) noexcept {
                // load fixed-size static data from the file
                static_assert(sizeof(_Uts_static_data) == 14, "_Uts_static_data's size must be 14 bytes");
                return _Mystream.read_exactly(reinterpret_cast<byte_t*>(&_Data), sizeof(_Uts_static_data));
            }

            bool _Load_catalogs(translator_catalogs& _Catalogs, const size_t _Count) {
                // load _Count fixed-size catalogs from the file
                constexpr size_t _Catalog_name_size = 64;
                constexpr size_t _Bytes_per_catalog = _Catalog_name_size + sizeof(uint32_t);
                const size_t _Buf_size              = _Count * _Bytes_per_catalog;
                unique_smart_array<byte_t> _Buf     = ::mjx::make_unique_smart_array<byte_t>(_Buf_size);
                if (!_Mystream.read_exactly(_Buf.get(), _Buf_size)) { // corrupted data or something went wrong
                    return false;
                }

                _Catalogs.reserve(_Count); // preallocate memory for the catalogs
                for (size_t _Off = 0; _Off < _Buf_size; _Off += _Bytes_per_catalog) {
                    _Catalogs.push_back(translator_catalog{
                        ::mjx::to_unicode_string(_Buf.get() + _Off),
                        _Load_integer<uint32_t>(_Buf.get() + _Off + _Catalog_name_size)
                    });
                }

                return true;
            }

        private:
            file_stream& _Mystream;
        };

        inline const path& _Get_settings_file_path() {
            // concatenate the current path with the settings file name to obtain its absolute path
            static const path& _Path = ::mjx::current_path() / L"settings.uts";
            return _Path;
        }

        class _Translator_init_lcids_iterator { // forward iterator for translator initialization LCIDs
        public:
            using value_type        = uint32_t;
            using difference_type   = ptrdiff_t;
            using pointer           = const value_type*;
            using reference         = const value_type&;
            using iterator_category = ::std::forward_iterator_tag;

            explicit _Translator_init_lcids_iterator(const translator_settings& _Settings) noexcept
                : _Myelems{0}, _Mycount(0), _Myoff(0) {
                _Init(_Settings);
            }

            ~_Translator_init_lcids_iterator() noexcept {}

            _Translator_init_lcids_iterator()                                                  = delete;
            _Translator_init_lcids_iterator(const _Translator_init_lcids_iterator&)            = delete;
            _Translator_init_lcids_iterator& operator=(const _Translator_init_lcids_iterator&) = delete;

            reference operator*() const noexcept {
                return _Myelems[_Myoff];
            }

            _Translator_init_lcids_iterator& operator++() noexcept {
                ++_Myoff;
                return *this;
            }

            pointer begin() const noexcept {
                return _Myelems;
            }

            pointer end() const noexcept {
                return _Myelems + _Mycount;
            }

        private:
            static constexpr size_t _Max_elem_count = 4;
            
            void _Insert_lcid(const uint32_t _Lcid) noexcept {
                // insert _Lcid to _Myelems, assumes that it is unique
                _Myelems[_Mycount++] = _Lcid;
            }

            void _Insert_lcid_if_unique(const uint32_t _Lcid) noexcept {
                // insert _Lcid to _Myelems if it is not already present, otherwise ignore it
                for (size_t _Idx = 0; _Idx < _Mycount; ++_Idx) {
                    if (_Myelems[_Idx] == _Lcid) { // same LCID found, break
                        return;
                    }
                }

                _Insert_lcid(_Lcid); // the given LCID is unique, insert it
            }

            void _Init(const translator_settings& _Settings) noexcept {
                // Note: The order of LCIDs is fixed and defined as follows: user-preferred, user-default,
                //       system-preferred, and system-default. To prevent multiple attempts to load the same
                //       catalog, the LCIDs are filtered to retain only unique ones.
                _Insert_lcid(_Settings.preferred_lcid()); // always unique
                _Insert_lcid_if_unique(_Settings.default_lcid());
                _Insert_lcid_if_unique(::mjx::system_preferred_lcid());
                _Insert_lcid_if_unique(::mjx::system_default_lcid());
            }

            uint32_t _Myelems[_Max_elem_count];
            size_t _Mycount;
            size_t _Myoff;
        };
    } // namespace umls_impl
} // namespace mjx

#endif // _UMLS_IMPL_TRANSLATOR_HPP_
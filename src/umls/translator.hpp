// translator.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_TRANSLATOR_HPP_
#define _UMLS_TRANSLATOR_HPP_
#include <atomic>
#include <cstdint>
#include <mjmem/object_allocator.hpp>
#include <mjstr/string.hpp>
#include <mjstr/string_view.hpp>
#include <mjsync/srwlock.hpp>
#include <type_traits>
#include <umls/api.hpp>
#include <umls/catalog.hpp>
#include <vector>

namespace mjx {
    _UMLS_API uint32_t system_default_lcid() noexcept;
    _UMLS_API uint32_t system_preferred_lcid() noexcept;

    struct translator_catalog {
        unicode_string name;
        uint32_t lcid;
    };

    using translator_catalogs = ::std::vector<translator_catalog, object_allocator<translator_catalog>>;

    class _UMLS_API translator_settings { // stores settings used by the translator
    public:
        translator_settings();
        ~translator_settings() noexcept;

        translator_settings(const translator_settings&)            = delete;
        translator_settings& operator=(const translator_settings&) = delete;

        // returns the path to the directory where the catalogs are installed
        static const path& catalogs_directory();

        // returns the default LCID
        uint32_t default_lcid() const noexcept;

        // returns or changes the preferred LCID
        uint32_t preferred_lcid() const noexcept;
        void preferred_lcid(const uint32_t _New_lcid) noexcept;
    
        // returns the installed catalogs
        const translator_catalogs& installed_catalogs() const noexcept;

        // checks if a catalog is installed
        bool is_catalog_installed(const unicode_string_view _Catalog) const noexcept;
    
        // discards all changes that have been made
        void discard_changes() noexcept;

    private:
        class _Locale {
        public:
            uint32_t _Default_lcid                  = 0;
            uint32_t _Originally_preferred_lcid     = 0;
            ::std::atomic<uint32_t> _Preferred_lcid = 0;

            // checks if any changes have been made
            bool _Changed() const noexcept;

            // reverts all changes that have been made
            void _Revert_changes() noexcept;

            // resets the locale
            void _Reset() noexcept;
        };

        // loads the settings from the file
        void _Load();

        // saves the settings to the file
        void _Save() const noexcept;

        _Locale _Myloc;
#pragma warning(suppress : 4251) // C4251: translator_catalogs needs to have dll-interface
        translator_catalogs _Mycats;
    };

    class _UMLS_API translator { // manages multi-language support
    public:
        ~translator() noexcept;

        translator(const translator&)            = delete;
        translator& operator=(const translator&) = delete;

        // returns the global instance of the translator
        static translator& global() noexcept;

        // returns the associated settings
        translator_settings& settings() noexcept;
        const translator_settings& settings() const noexcept;

        // returns or changes the fallback message
        const unicode_string& fallback_message() const noexcept;
        void fallback_message(const unicode_string_view _New_message);

        // returns the current catalog
        const message_catalog& catalog() const noexcept;

        // loads a catalog
        bool use_catalog(const unicode_string_view _Catalog);

    private:
        translator() noexcept;

        // returns a catalog name associated with the given LCID
        unicode_string_view _Find_catalog_name_by_lcid(const uint32_t _Lcid) const noexcept;

        // initializes the translator
        void _Init();

        mutable shared_lock _Mylock;
        translator_settings _Myset;
        message_catalog _Mycat;
        unicode_string _Myfbmsg;
    };

    template <class... _Types>
    inline unicode_string get_message(const utf8_string_view _Id, _Types&&... _Args) {
        translator& _Tr                 = translator::global();
        const message_catalog& _Catalog = _Tr.catalog();
        if (!_Catalog.is_open()) { // no catalog is open, return the fallback message
            return _Tr.fallback_message();
        }

        const auto& [_Message, _Retrieved] = _Catalog.get_message(
            _Id, ::mjx::make_format_args(::std::forward<_Types>(_Args)...));
        return _Retrieved ? _Message : _Tr.fallback_message();
    }
} // namespace mjx

#endif // _UMLS_TRANSLATOR_HPP_
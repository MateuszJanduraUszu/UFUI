// translator.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <umls/impl/tinywin.hpp>
#include <umls/impl/translator.hpp>
#include <umls/translator.hpp>

namespace mjx {
    uint32_t system_default_lcid() noexcept {
        return ::GetSystemDefaultLCID();
    }

    uint32_t system_preferred_lcid() noexcept {
        return ::GetUserDefaultLCID();
    }

    translator_settings::translator_settings() : _Myloc(), _Mycats() {
        _Load();
    }

    translator_settings::~translator_settings() noexcept {
        if (_Myloc._Changed()) { // something has been changed, save it
            _Save();
        }
    }

    bool translator_settings::_Locale::_Changed() const noexcept {
        // Note: Only the preferred LCID can be changed at runtime, so we must compare
        //       the current preferred LCID with the original one.
        return _Preferred_lcid.load(::std::memory_order_relaxed) != _Originally_preferred_lcid;
    }

    void translator_settings::_Locale::_Revert_changes() noexcept {
        _Preferred_lcid.store(_Originally_preferred_lcid, ::std::memory_order_relaxed);
    }

    void translator_settings::_Locale::_Reset() noexcept {
        _Default_lcid              = 0;
        _Originally_preferred_lcid = 0;
        _Preferred_lcid.store(0, ::std::memory_order_relaxed);
    }

    void translator_settings::_Load() {
        // load the translator settings from the 'settings.uts' file
        file _File(umls_impl::_Get_settings_file_path(), file_access::read, file_share::read);
        file_stream _Stream(_File);
        if (_Stream.is_open()) { // valid stream, try to load the settings
            umls_impl::_Uts_file_loader _Loader(_Stream);
            umls_impl::_Uts_static_data _Data;
            if (!_Loader._Load_static_data(_Data) || !umls_impl::_Verify_uts_signature(_Data)) {
                return;
            }

            _Myloc._Default_lcid              = _Data._Default_lcid;
            _Myloc._Originally_preferred_lcid = _Data._Preferred_lcid;
            _Myloc._Preferred_lcid.store(_Data._Preferred_lcid, ::std::memory_order_relaxed);
            if (!_Loader._Load_catalogs(_Mycats, _Data._Catalog_count)) { // something went wrong, reset the locale
                _Myloc._Reset();
            }
        }
    }

    void translator_settings::_Save() const noexcept {
        // save the translator settings to the 'settings.uts' file
        file _File(umls_impl::_Get_settings_file_path(), file_access::write);
        file_stream _Stream(_File);
        if (_Stream.is_open()) { // valid stream, try to save the settings
            constexpr file_stream::pos_type _Preferred_lcid_offset = 8;
            if (_Stream.seek(_Preferred_lcid_offset)) {
                const uint32_t _Lcid = _Myloc._Preferred_lcid.load(::std::memory_order_relaxed);
                _Stream.write(reinterpret_cast<const byte_t*>(&_Lcid), sizeof(uint32_t));
            }
        }
    }

    const path& translator_settings::catalogs_directory() {
        // concatenate the current directory path with a 'locale' directory
        static const path& _Path = ::mjx::current_path() / L"locale";
        return _Path;
    }

    uint32_t translator_settings::default_lcid() const noexcept {
        return _Myloc._Default_lcid;
    }

    uint32_t translator_settings::preferred_lcid() const noexcept {
        return _Myloc._Preferred_lcid.load(::std::memory_order_relaxed);
    }

    void translator_settings::preferred_lcid(const uint32_t _New_lcid) noexcept {
        _Myloc._Preferred_lcid.store(_New_lcid, ::std::memory_order_relaxed);
    }

    const translator_catalogs& translator_settings::installed_catalogs() const noexcept {
        return _Mycats;
    }

    bool translator_settings::is_catalog_installed(const unicode_string_view _Catalog) const noexcept {
        for (const translator_catalog& _Cat : _Mycats) {
            if (_Cat.name == _Catalog) { // searched catalog found, break
                return true;
            }
        }

        return false; // not found
    }

    void translator_settings::discard_changes() noexcept {
        if (_Myloc._Changed()) { // something has been changed, revert it
            _Myloc._Revert_changes();
        }
    }

    translator::translator() noexcept : _Mylock(), _Myset(), _Mycat(), _Myfbmsg(L"???") {
        // invoke _Init() within a try-catch block to preserve the noexcept specification of the constructor
        try {
            _Init();
        } catch (...) {
            // ignore the thrown exception
        }
    }

    translator::~translator() noexcept {}

    unicode_string_view translator::_Find_catalog_name_by_lcid(const uint32_t _Lcid) const noexcept {
        for (const translator_catalog& _Catalog : _Myset.installed_catalogs()) {
            if (_Catalog.lcid == _Lcid) { // searched catalog found, break
                return _Catalog.name;
            }
        }

        return unicode_string_view{}; // not found
    }

    void translator::_Init() {
        // load a catalog based on user preferrence
        umls_impl::_Translator_init_lcids_iterator _Iter(_Myset);
        for (const uint32_t _Lcid : _Iter) {
            const unicode_string_view _Catalog = _Find_catalog_name_by_lcid(_Lcid);
            if (!_Catalog.empty() && use_catalog(_Catalog)) { // loaded a catalog, break
                break;
            }
        }
    }

    translator& translator::global() noexcept {
        static translator _Tr;
        return _Tr;
    }

    translator_settings& translator::settings() noexcept {
        return _Myset;
    }

    const translator_settings& translator::settings() const noexcept {
        return _Myset;
    }

    const unicode_string& translator::fallback_message() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return _Myfbmsg;
    }

    void translator::fallback_message(const unicode_string_view _New_message) {
        lock_guard _Guard(_Mylock);
        if (_Myfbmsg != _New_message) {
            _Myfbmsg = _New_message;
        }
    }

    const message_catalog& translator::catalog() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return _Mycat;
    }

    bool translator::use_catalog(const unicode_string_view _Catalog) {
        lock_guard _Guard(_Mylock);
        if (_Mycat.is_open()) { // some catalog is already open, close it
            _Mycat.close();
        }

        return _Mycat.open(_Myset.catalogs_directory() / _Catalog);
    }
} // namespace mjx
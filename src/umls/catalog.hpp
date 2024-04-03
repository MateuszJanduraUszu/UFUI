// catalog.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_CATALOG_HPP_
#define _UMLS_CATALOG_HPP_
#include <mjfs/path.hpp>
#include <mjmem/smart_pointer.hpp>
#include <mjstr/string.hpp>
#include <mjstr/string_view.hpp>
#include <umls/api.hpp>
#include <umls/format.hpp>

namespace mjx {
    namespace umls_impl {
        class _Message_catalog;
    } // namespace umls_impl

    class _UMLS_API message_catalog {
    public:
        message_catalog() noexcept;
        message_catalog(message_catalog&& _Other) noexcept;
        ~message_catalog() noexcept;

        explicit message_catalog(const path& _Target);

        message_catalog& operator=(message_catalog&& _Other) noexcept;

        message_catalog(const message_catalog&)            = delete;
        message_catalog& operator=(const message_catalog&) = delete;

        // checks if the catalog is open
        bool is_open() const noexcept;

        // closes the catalog
        void close() noexcept;

        // opens a catalog
        bool open(const path& _Target);

        // returns the language name associated with the catalog
        const unicode_string& language() const noexcept;

        // returns the LCID associated with the catalog
        uint32_t lcid() const noexcept;

        // checks whether the catalog has a message
        bool has_message(const utf8_string_view _Id) const noexcept;

        // retrieves a message from the catalog
        unicode_string get_message(const utf8_string_view _Id, const format_args& _Args = format_args{}) const;

    private:
        unique_smart_ptr<umls_impl::_Message_catalog> _Myimpl;
    };
} // namespace mjx

#endif // _UMLS_CATALOG_HPP_
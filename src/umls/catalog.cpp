// catalog.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <type_traits>
#include <umls/catalog.hpp>
#include <umls/impl/catalog.hpp>
#include <umls/impl/utils.hpp>

namespace mjx {
    message_catalog::message_catalog() noexcept : _Myimpl(nullptr) {}

    message_catalog::message_catalog(message_catalog&& _Other) noexcept
        : _Myimpl(_Other._Myimpl.release()) {}

    message_catalog::message_catalog(const path& _Target)
        : _Myimpl(::mjx::create_object<umls_impl::_Message_catalog>(_Target)) {}

    message_catalog::~message_catalog() noexcept {
        close();
    }

    message_catalog& message_catalog::operator=(message_catalog&& _Other) noexcept {
        if (this != ::std::addressof(_Other)) {
            _Myimpl.reset(_Other._Myimpl.release());
        }

        return *this;
    }

    bool message_catalog::is_open() const noexcept {
        return _Myimpl != nullptr && _Myimpl->_Valid();
    }

    void message_catalog::close() noexcept {
        if (_Myimpl) {
            _Myimpl.reset();
        }
    }

    bool message_catalog::open(const path& _Target) {
        if (is_open()) { // some catalog is already open, break
            return false;
        }

        _Myimpl.reset(::mjx::create_object<umls_impl::_Message_catalog>(_Target));
        return _Myimpl->_Valid();
    }

    const unicode_string& message_catalog::language() const noexcept {
#ifdef _DEBUG
        _INTERNAL_ASSERT(is_open(), "attempt to use invalid catalog");
#endif // _DEBUG
        return _Myimpl->_Language;
    }

    uint32_t message_catalog::lcid() const noexcept {
        // Note: Unlike _Myimpl->_Language, which is passed by constant reference,
        //       _Myimpl->_Lcid is passed by value, hence we don't need to validate _Myimpl here.
        return is_open() ? _Myimpl->_Lcid : 0;
    }

    bool message_catalog::has_message(const utf8_string_view _Id) const noexcept {
        if (!is_open()) { // invalid catalog, break
            return false;
        }

        return _Myimpl->_Table._Find_message(umls_impl::_Hash_message_id(_Id)) != nullptr;
    }

    message_catalog::message_retrieval_result message_catalog::get_message(
        const utf8_string_view _Id, const format_args& _Args) const {
        if (!is_open()) { // invalid catalog, break
            return message_retrieval_result{unicode_string{}, false};
        }
        
        const auto* const _Entry = _Myimpl->_Table._Find_message(umls_impl::_Hash_message_id(_Id));
        if (!_Entry) { // message not found, break
            return message_retrieval_result{unicode_string{}, false};
        }

#ifdef _M_X64
        const size_t _Len = static_cast<size_t>(_Entry->_Length);
        const size_t _Off = _Entry->_Offset;
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
        const size_t _Len = _Entry->_Length;
        const size_t _Off = static_cast<size_t>(_Entry->_Offset);
#endif // _M_X64
        unicode_string _Msg;
        if (!_Myimpl->_Blob._Fetch_message(_Msg, _Off, _Len)) { // failed to fetch the message, break
            return message_retrieval_result{unicode_string{}, false};
        }

        if (::mjx::is_formattable(_Msg)) { // formattable message, try to format it
            // Note: For a message to be formattable, it must contain at least one format specifier
            //       and be non-empty. An empty formatted message typically indicates that something
            //       went wrong during formatting. By checking for an empty message, we can detect
            //       such issue and appropriately set the message_retrieval_result::retrieved member.
            _Msg                  = ::mjx::format_string(_Msg, _Args);
            const bool _Not_empty = !_Msg.empty();
            return message_retrieval_result{::std::move(_Msg), _Not_empty};
        } else { // not formattable message, leave it as is
            return message_retrieval_result{::std::move(_Msg), true};
        }
    }
} // namespace mjx
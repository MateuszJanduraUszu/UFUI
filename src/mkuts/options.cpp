// options.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <mjfs/directory.hpp>
#include <mjfs/status.hpp>
#include <mkuts/logger.hpp>
#include <mkuts/options.hpp>

namespace mjx {
    program_options::program_options() noexcept
        : catalogs(), output_dir(), default_lcid(0), preferred_lcid(0) {}

    program_options::~program_options() noexcept {}

    program_options& program_options::global() noexcept {
        static program_options _Options;
        return _Options;
    }

    bool _Is_catalog_included(const path& _Path) noexcept {
        for (const path& _Catalog : program_options::global().catalogs) {
            if (_Catalog == _Path) { // searched catalog found, break
                return true;
            }
        }

        return false; // not found
    }

    path _Absolute_path(const path& _Path) {
        // concatenate _Path with the current directory if it is relative, otherwise return it as is
        return _Path.is_absolute() ? _Path : ::mjx::current_path() / _Path;
    }

    void _Check_lcids() noexcept {
        // check if the LCIDs contain non-zero values
        const program_options& _Options = program_options::global();
        if (_Options.default_lcid == 0) { // check the default LCID
            rtlog(L"Warning: The default LCID has not been specified.");
        }

        if (_Options.preferred_lcid == 0) { // check the preferred LCID
            rtlog(L"Warning: The preferred LCID has not been specified.");
        }
    }

    void _Options_parser::_Parse_catalog(const unicode_string_view _Value) {
        vector<path>& _Catalogs = program_options::global().catalogs;
        path _Path              = _Absolute_path(_Value);
        if (_Is_catalog_included(_Path)) { // already specified
            rtlog(L"Warning: The catalog '%s' specified more than once, ignored.", _Value.data());
            return;
        }

        if (!::mjx::exists(_Path)) { // specified non-existent file
            rtlog(L"Warning: The catalog '%s' does not exist, ignored.", _Value.data());
            return;
        }

        if (_Path.extension() != L".umc") { // specified not recognized file
            rtlog(L"Warning: The catalog '%s' has an invalid extension, ignored.", _Value.data());
            return;
        }

        _Catalogs.push_back(::std::move(_Path));
    }
    
    void _Options_parser::_Parse_catalog_directory(const unicode_string_view _Value) {
        vector<path>& _Catalogs = program_options::global().catalogs;
        const path& _Path       = _Absolute_path(_Value);
        if (!::mjx::exists(_Path)) { // specified non-existent directory
            rtlog(L"Warning: The directory '%s' does not exist, ignored.", _Value.data());
            return;
        }

        if (!::mjx::is_directory(_Path)) { // specified non-directory
            rtlog(L"Warning: The directory '%s' is not a directory, ignored.", _Value.data());
            return;
        }

        for (const directory_entry& _Entry : directory_iterator(_Path)) {
            const path& _File = _Entry.absolute_path();
            if (_File.extension() == L".umc") { // catalog found
                if (!_Is_catalog_included(_File)) { // catalog not included, include it
                    _Catalogs.push_back(_File);
                } else { // catalog already specified
                    rtlog(L"Warning: The catalog '%s' already specified, ignored.", _File.c_str());
                }
            }
        }
    }

    void _Options_parser::_Parse_output_directory(const unicode_string_view _Value) {
        path& _Outdir = program_options::global().output_dir;
        if (_Outdir.empty()) { // set the output directory
            path _Path = _Absolute_path(_Value);
            if (::mjx::exists(_Path)) { // specified existing directory, validate it
                if (!::mjx::is_directory(_Path)) { // specified non-directory, break
                    rtlog(L"Warning: The output directory '%s' is not a directory, ignored.", _Value.data());
                    return;
                }
            } else { // specified non-existent directory, try to create it
                if (!::mjx::create_directory(_Path)) { // failed to create the directory, break
                    rtlog(L"Warning: Failed to create the output directory '%s', ignored.", _Value.data());
                    return;
                }
            }

            _Outdir = ::std::move(_Path);
        } else { // the output directory already specified
            rtlog(L"Warning: Output directory specified more than once, ignored.");
        }
    }

    void _Options_parser::_Parse_lcid(const unicode_string_view _Value, uint32_t& _Lcid) noexcept {
        constexpr uint32_t _Max    = 0x7FFF'FFFF; // max LCID value
        const wchar_t* _First      = _Value.data();
        const wchar_t* const _Last = _First + _Value.size();
        uint32_t _Val              = 0;
        for (; _First != _Last; ++_First) {
            if (*_First < L'0' || *_First > L'9') { // must consist only of digits
                rtlog(L"Warning: The LCID '%s' must consist only of digits, ignored.", _Value.data());
                return;
            }

            _Val = _Val * 10 + static_cast<uint32_t>(*_First - L'0');
            if (_Val > _Max) { // value too large, break
                rtlog(L"Warning: The LCID '%s' is too large.", _Value.data());
                return;
            }
        }

        _Lcid = _Val;
    }

    void parse_program_args(int _Count, wchar_t** _Args) {
        program_options& _Options = program_options::global();
        unicode_string_view _Arg;
        size_t _Eq_pos;
        for (; _Count > 0; --_Count, ++_Args) {
            _Arg    = *_Args;
            _Eq_pos = _Arg.find(L'=');
            if (_Eq_pos == unicode_string_view::npos) { // unrecognized argument
                rtlog(L"Warning: Unrecognized option '%s', ignored.", _Arg.data());
                continue;
            }

            if (_Eq_pos == 0 || _Eq_pos == _Arg.size() - 1) { // invalid option
                rtlog(L"Warning: Invalid option '%s', ignored.", _Arg.data());
                continue;
            }

            const unicode_string_view _Option = _Arg.substr(0, _Eq_pos);
            const unicode_string_view _Value  = _Arg.substr(_Eq_pos + 1);
            if (_Option == L"--catalog") { // include a catalog
                _Options_parser::_Parse_catalog(_Value);
            } else if (_Option == L"--catalog-dir") { // include catalogs from a directory
                _Options_parser::_Parse_catalog_directory(_Value);
            } else if (_Option == L"--output-dir") { // set the output directory
                _Options_parser::_Parse_output_directory(_Value);
            } else if (_Option == L"--default-lcid") { // set the default LCID
                _Options_parser::_Parse_lcid(_Value, _Options.default_lcid);
            } else if (_Option == L"--preferred-lcid") { // set the preferred LCID
                _Options_parser::_Parse_lcid(_Value, _Options.preferred_lcid);
            } else {
                rtlog(L"Warning: Unrecognized option '%s', ignored.", _Arg.data());
            }
        }

        if (_Options.output_dir.empty()) { // set the default output directory
            _Options.output_dir = ::mjx::current_path();
        }

        _Check_lcids(); // check if all LCIDs are specified
    }
} // namespace mjx
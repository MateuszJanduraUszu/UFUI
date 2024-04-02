// api.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UMLS_API_HPP_
#define _UMLS_API_HPP_

#ifdef UMLS_EXPORTS
#define _UMLS_API __declspec(dllexport)
#else // ^^^ UMLS_EXPORTS ^^^ / vvv !UMLS_EXPORTS vvv
#define _UMLS_API __declspec(dllimport)
#endif // UMLS_EXPORTS
#endif // _UMLS_API_HPP_
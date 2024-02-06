// api.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _ULS_API_HPP_
#define _ULS_API_HPP_

#ifdef ULS_EXPORTS
#define _ULS_API __declspec(dllexport)
#else // ^^^ ULS_EXPORTS ^^^ / vvv !ULS_EXPORTS vvv
#define _ULS_API __declspec(dllimport)
#endif // ULS_EXPORTS
#endif // _ULS_API_HPP_
// api.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _URE_API_HPP_
#define _URE_API_HPP_

#ifdef URE_EXPORTS
#define _URE_API __declspec(dllexport)
#else // ^^^ URE_EXPORTS ^^^ / vvv !URE_EXPORTS vvv
#define _URE_API __declspec(dllimport)
#endif // URE_EXPORTS
#endif // _URE_API_HPP_
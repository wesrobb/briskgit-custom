/*
 * dtwindow.h - header file for D-Type's platform independent window support
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_WINDOW_H
#define DTYPE_WINDOW_H


#ifndef DEF_WIN32
#if (defined(_WIN32) || defined(__WIN32__))
#define DEF_WIN32
#endif
#endif

#ifndef DEF_LINUX
#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__CYGWIN__))
#define DEF_LINUX
#endif
#endif

#ifndef DEF_APPLE
#if (defined(__APPLE__))
#define DEF_APPLE
#endif
#endif


#if (defined(DEF_WINDOW_FORCE_VOID))

#include "dtwindow/void/DTWindowPlatform.h"

#elif (defined(DEF_WIN32) || defined(DEF_WINDOW_FORCE_WIN32))

#include "dtwindow/win/DTWindowPlatform.h"

#elif (defined(DEF_LINUX) || defined(DEF_WINDOW_FORCE_X11))

#include "dtwindow/unix/DTWindowPlatform.h"

#elif (defined(DEF_APPLE) || defined(DEF_WINDOW_FORCE_OSX))

#include "dtwindow/osx/DTWindowPlatform.h"

#else

#error "error (dtwindow.h): non-recognized or non-supported platform"

#endif


#endif /* DTYPE_WINDOW_H */

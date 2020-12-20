/*
 * dtype.h - main header file for all D-Type engines
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_ENGINE_H
#define DTYPE_ENGINE_H


/* --- Configuration Settings --- */

//#define DEF_DTBASE_HUGE_AS_64BIT
//#define DEF_DTBASE_FORCE_FIXEDINT_STD     /* Enable standard C/C++ exact-width integer types */
//#define DEF_DTBASE_FORCE_FIXEDINT_MSC     /* Enable non-standard exact-width integer types in MSC prior to 2010 */
//#define DEF_DTBASE_FORCE_FIXEDINT_64BIT   /* Use D-Type's exact-width integer types on 64-bit architectures */
//#define DEF_DTBASE_FORCE_FIXEDINT_DEFAULT /* Use D-Type's exact-width integer types */

#define DEF_STDLIB              /* Include <stdlib.h> */
#define DEF_WINGDI              /* Enable D-Type's support for Windows HDC (on Windows only) */
#define DEF_STDIO_FILE          /* Enable D-Type's support for standard FILE* type */
#define DEF_FLOAT_IEEE754       /* Allows DF_ROUND and DF_FLOOR to use fast float-to-int conversion */
#define DEF_MACROS_FLOAT_BASIC  /* Enable D-Type's basic floating point macros (DF_ROUND, DF_FLOOR) */
#define DEF_MACROS_FLOAT_OTHER  /* Enable D-Type's other floating point macros (DF_RAD, DF_DEG, DF_ISEQUAL) */
#define DEF_MACROS_CONVENIENCE  /* Enable D-Type's convenience macros (DF_MIN, DF_MAX) */


/* --- Required Includes --- */

#include "dtype/dtbase.h"       /* D-Type's base types and definitions, required by all other D-Type engines */
#include "dtype/dthead.h"       /* D-Type's common header, required by all other D-Type engines */


/* --- D-Type Engines --- */

#include "dtype/dtypestd-lo.h"  /* D-Type Standard Engine (Low-Level API) header */
#include "dtype/dtypestd-hi.h"  /* D-Type Standard Engine (High-Level API) header */
#include "dtype/dtypeaux.h"     /* D-Type Auxiliary Extension header */
#include "dtype/dtypelay.h"     /* D-Type Text Layout Extension header */
#include "dtype/dtypepwr.h"     /* D-Type Power Engine header */
#include "dtype/dtypetxt-lo.h"  /* D-Type Text Engine (Low-Level API) header */
#include "dtype/dtypetxt-hi.h"  /* D-Type Text Engine (High-Level API) header */


#endif /* DTYPE_ENGINE_H */

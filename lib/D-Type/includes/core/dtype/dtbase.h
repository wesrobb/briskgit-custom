/*
 *
 * Copyright (C) D-Type Solutions - All Rights Reserved
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 *
 * This file is part of D-Type Engine and may only be used, modified
 * and distributed under the terms of the applicable license agreement
 * For conditions of distribution and use, see license.txt
 *
 * ANY VIOLATIONS OF THE ABOVE LICENSE AGREEMENT WILL BE PROSECUTED TO
 * THE MAXIMUM EXTENT POSSIBLE UNDER THE LAW. NO VENDOR, DISTRIBUTOR,
 * DEALER, RETAILER, SALES PERSON OR OTHER PERSON IS AUTHORIZED TO MODIFY
 * THIS AGREEMENT OR TO MAKE ANY WARRANTY, REPRESENTATION OR PROMISE WHICH
 * IS DIFFERENT THAN, OR IN ADDITION TO, THIS AGREEMENT.
 *
 */


#ifndef DTYPE_DTBASE_H
#define DTYPE_DTBASE_H


#ifndef DEF_WIN32
#if (defined(_WIN32) || defined(__WIN32__))
	#define DEF_WIN32
#endif
#endif

#if defined(__LP64__) || defined(__64BIT__) || defined(_LP64) || (__WORDSIZE == 64)
	#define DEF_DTBASE_ARCH_MODEL_64BIT
#else
	#define DEF_DTBASE_ARCH_MODEL_DEFAULT
#endif

/*
 * Important Notes:
 *
 * DT_SBYTE and DT_UBYTE must be 1 byte long
 * DT_SWORD and DT_UWORD must be 2 bytes long
 * DT_SLONG and DT_ULONG must be 4 bytes long
 *
 * Contact D-Type Solutions if your compiler does not provide any data types that satisfy the above requirements.
 *
 */

#ifdef DEF_DTBASE_FORCE_FIXEDINT_STD

	/* Standard exact-width integer types (using 2's complement for negative values) */
	#include <stdint.h>

	typedef int8_t DT_SBYTE;
	typedef int16_t DT_SWORD;
	typedef int32_t DT_SLONG;
	typedef uint8_t DT_UBYTE;
	typedef uint16_t DT_UWORD;
	typedef uint32_t DT_ULONG;
	typedef uint64_t DT_BIT64;
#ifdef DEF_DTBASE_HUGE_AS_64BIT
	typedef int64_t DT_SHUGE;
	typedef uint64_t DT_UHUGE;
#else
	typedef int32_t DT_SHUGE;
	typedef uint32_t DT_UHUGE;
#endif

#else

	#if (defined(DEF_DTBASE_FORCE_FIXEDINT_MSC))

		/* Non-standard exact-width integer types in MSC prior to 2010 (_MSC_VER 1600) */

		#define DT_DTBASE_SZ1 __int8
		#define DT_DTBASE_SZ2 __int16
		#define DT_DTBASE_SZ4 __int32
		#define DT_DTBASE_SZ8 __int64

	#elif (defined(DEF_DTBASE_FORCE_FIXEDINT_64BIT) || defined(DEF_DTBASE_ARCH_MODEL_64BIT))

		/* 64-bit (e.g. gcc on OS X) */

		#define DT_DTBASE_SZ1 char
		#define DT_DTBASE_SZ2 short int
		#define DT_DTBASE_SZ4 int
		#define DT_DTBASE_SZ8 long

	#elif (defined(DEF_DTBASE_FORCE_FIXEDINT_DEFAULT) || defined(DEF_DTBASE_ARCH_MODEL_DEFAULT))

		/* All 32-bit platforms (Windows, Linux, OS X), 64-bit Windows */

		#define DT_DTBASE_SZ1 char
		#define DT_DTBASE_SZ2 short int
		#define DT_DTBASE_SZ4 long
		//#ifdef DEF_COMPILER_MSVC
		#if defined(_MSC_VER) && _MSC_VER < 1600
			#define DT_DTBASE_SZ8 __int64
		#else
			#define DT_DTBASE_SZ8 long long
		#endif

	#else

		#error "Basic data types not defined!"

	#endif

	typedef signed DT_DTBASE_SZ1 DT_SBYTE;
	typedef signed DT_DTBASE_SZ2 DT_SWORD;
	typedef signed DT_DTBASE_SZ4 DT_SLONG;
	typedef unsigned DT_DTBASE_SZ1 DT_UBYTE;
	typedef unsigned DT_DTBASE_SZ2 DT_UWORD;
	typedef unsigned DT_DTBASE_SZ4 DT_ULONG;
	typedef unsigned DT_DTBASE_SZ8 DT_BIT64;
#ifdef DEF_DTBASE_HUGE_AS_64BIT
	typedef signed DT_DTBASE_SZ8 DT_SHUGE;
	typedef unsigned DT_DTBASE_SZ8 DT_UHUGE;
#else
	typedef signed DT_DTBASE_SZ4 DT_SHUGE;
	typedef unsigned DT_DTBASE_SZ4 DT_UHUGE;
#endif

#endif


/* Compile-time check to make sure that D-Type's predefined int types have the right size on all supported platforms (1, 2, 4 and 8 bytes) */

#define DEF_DTBASE_COMPILE_ASSERT_01(bool_expr) typedef char check_size_of_dtype_base_types_01[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_02(bool_expr) typedef char check_size_of_dtype_base_types_02[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_03(bool_expr) typedef char check_size_of_dtype_base_types_03[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_04(bool_expr) typedef char check_size_of_dtype_base_types_04[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_05(bool_expr) typedef char check_size_of_dtype_base_types_05[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_06(bool_expr) typedef char check_size_of_dtype_base_types_06[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_07(bool_expr) typedef char check_size_of_dtype_base_types_07[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_08(bool_expr) typedef char check_size_of_dtype_base_types_08[2 * ((bool_expr) != 0) - 1]
#define DEF_DTBASE_COMPILE_ASSERT_09(bool_expr) typedef char check_size_of_dtype_base_types_09[2 * ((bool_expr) != 0) - 1]

#pragma message("--- Performing compile-time checks ---")

DEF_DTBASE_COMPILE_ASSERT_01(sizeof(DT_SBYTE) == 1);
DEF_DTBASE_COMPILE_ASSERT_02(sizeof(DT_UBYTE) == 1);
DEF_DTBASE_COMPILE_ASSERT_03(sizeof(DT_SWORD) == 2);
DEF_DTBASE_COMPILE_ASSERT_04(sizeof(DT_UWORD) == 2);
DEF_DTBASE_COMPILE_ASSERT_05(sizeof(DT_SLONG) == 4);
DEF_DTBASE_COMPILE_ASSERT_06(sizeof(DT_ULONG) == 4);
#ifdef DEF_DTBASE_HUGE_AS_64BIT
DEF_DTBASE_COMPILE_ASSERT_07(sizeof(DT_SHUGE) == 8);
DEF_DTBASE_COMPILE_ASSERT_08(sizeof(DT_UHUGE) == 8);
#pragma message("--- DT_SHUGE/DT_UHUGE is 64 bits long ---")
#else
DEF_DTBASE_COMPILE_ASSERT_07(sizeof(DT_SHUGE) == 4);
DEF_DTBASE_COMPILE_ASSERT_08(sizeof(DT_UHUGE) == 4);
#endif

#pragma message("--- Compile-time checks are done ---")

/* If we reach this point, everything is OK */

typedef DT_SBYTE DT_ID_SBYTE;
typedef DT_UBYTE DT_ID_UBYTE;
typedef DT_SWORD DT_ID_SWORD;
typedef DT_UWORD DT_ID_UWORD;
typedef DT_SLONG DT_ID_SLONG;
typedef DT_ULONG DT_ID_ULONG;


#define DV_SIZE_1 1
#define DV_SIZE_2 2
#define DV_SIZE_3 3
#define DV_SIZE_4 4


typedef double DT_FLOAT;


/* Endianness */

#if defined(DEF_CPUTYPE_INTEL)

	#define DV_ENDIANLOC 0
	#pragma message("--- Endianness: LITTLE ENDIAN (0) ---")

#elif defined(__i386__) || defined(__i386) || defined(__X86__) || defined(__x86_64)

	#define DV_ENDIANLOC 0
	#pragma message("--- Endianness detected: LITTLE ENDIAN (0) ---")

#elif defined(__POWERPC__) || defined(__ppc__)

	#define DV_ENDIANLOC 1
	#pragma message("--- Endianness detected: BIG ENDIAN (1) ---")

#else

	#pragma message("--- Endianness not detected --- ")

#endif


/* Improved float-point support (not always portable) */

#if (defined(DEF_FLOAT_IEEE754))

DEF_DTBASE_COMPILE_ASSERT_09(sizeof(DT_FLOAT) == 8);

typedef union { DT_FLOAT f; DT_SLONG i[2]; } DT_CAST_FLOAT_TO_SLONG;

#define DV_FLOAT_IEEE754_TRICK 6755399441055744.0

#ifdef DV_ENDIANLOC
	#define DV_FLOAT_IEEE754_ENDIANLOC DV_ENDIANLOC
	#pragma message("--- IEEE754 floating-point enabled (endianness calculated at compile-time) ---")
#else
	const DT_CAST_FLOAT_TO_SLONG DTypeF2I = {-(33.0 + DV_FLOAT_IEEE754_TRICK)};
	#define DV_FLOAT_IEEE754_ENDIANLOC (DTypeF2I.i[1] == 33) /* calculate endianness at run-time and not compile-time */
	#pragma message("--- IEEE754 floating-point enabled (endianness calculated at run-time) ---")
#endif

inline DT_SLONG DF_F2I_SLONG(DT_FLOAT f)
{
	volatile DT_CAST_FLOAT_TO_SLONG f2i = {f + DV_FLOAT_IEEE754_TRICK}; /* f2i.f = f + DV_FLOAT_IEEE754_TRICK; */
	return f2i.i[DV_FLOAT_IEEE754_ENDIANLOC];
}


#ifdef DEF_DTBASE_HUGE_AS_64BIT
typedef union { DT_FLOAT f; DT_SHUGE i; } DT_CAST_FLOAT_TO_SHUGE;
inline DT_SHUGE DF_F2I_SHUGE(DT_FLOAT f)
{
	volatile DT_CAST_FLOAT_TO_SHUGE f2i = {f + DV_FLOAT_IEEE754_TRICK}; /* f2i.f = f + DV_FLOAT_IEEE754_TRICK; */
	f2i.i <<= 13; f2i.i >>= 13;
	return f2i.i;
}
#endif

#endif


/* Support for Windows HDC */

#if (defined(DEF_WIN32) && defined(DEF_WINGDI))

	#ifdef DEF_DISABLE_HDC
		typedef DT_SLONG DT_HDC;
	#else
		#include <windows.h>
		typedef HDC DT_HDC;
		#pragma message("--- WINGDI enabled ---")
	#endif

#else

	typedef DT_SLONG DT_HDC;

#endif


/* Calling Convention */

#ifdef DEF_WIN32
	#define DTYPE_APICALL __stdcall
	#define DTYPE_USRCALL __cdecl
#else
	#define DTYPE_APICALL
	#define DTYPE_USRCALL
#endif


/* Linkage Specification */

#define DT_EXTERN extern "C"


#endif /* DTYPE_DTBASE_H */


#pragma once
#ifndef SHMEM_SHMEM_H
#define SHMEM_SHMEM_H

#if shmem_DLL
// .dll (or .so) interface.
#if _MSC_VER
# ifdef shmem_EXPORTS
#  define SHMEM_API		__declspec( dllexport )
# else
#  define SHMEM_API		__declspec( dllimport )
# endif
#else // _WIN32
# if defined __SUNPRO_C  || defined __SUNPRO_CC
#  define SHMEM_API __global
# elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#  define SHMEM_API __attribute__ ((visibility("default")))
# else
#  define SHMEM_API
# endif
#endif
#else
#define SHMEM_API	extern
#endif

#include <shmem/config.h>

SHMEM_API void foo(void);

#endif

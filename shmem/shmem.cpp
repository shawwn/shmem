
#include "shmem.h"

#include <stdio.h>

SHMEM_API void foo(void)
{
	printf("This is shmem version %s\n", SHMEM_VERSION_STRING);
}

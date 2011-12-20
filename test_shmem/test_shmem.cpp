
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "shmem/shmem.h"

int main(int argc, char* argv[])
{
	shmem_t* shm = shmem_create_or_open( "test_shmem", 8 );
	if ( !shm )
	{
		fprintf( stderr, "test_shmem: failed to open shared memory.\n" );
		exit( -1 );
	}

	char* str = (char*)shmem_ptr( shm );
	strcpy( str, "hello" );

	{
		shmem_t* shm2 = shmem_create_or_open( "test_shmem", 8 );
		if ( shmem_was_created( shm2 ) )
		{
			fprintf( stderr, "test_shmem: shmem_was_created() failed.\n" );
			exit( -1 );
		}

		char* str2 = (char*)shmem_ptr( shm );
		printf( "%s\n", str2 );

		shmem_close( shm2 );
	}

	shmem_close( shm );
	return 0;
}

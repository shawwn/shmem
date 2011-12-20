#ifndef SHMEM_H
#define SHMEM_H

// std C headers.
#include <cstddef>

//===================================================================================
// named shared memory.
//===================================================================================
#include <shmem/config.h>

struct shmem; typedef struct shmem shmem_t;

shmem_t*	shmem_create( const char* name, size_t size );
shmem_t*	shmem_open( const char* name, size_t size );
shmem_t*	shmem_create_or_open( const char* name, size_t size );
void		shmem_close( shmem_t* shm );

void*		shmem_ptr( shmem_t* shm );

const char*	shmem_get_name( shmem_t* shm );
size_t		shmem_get_size( shmem_t* shm );
bool		shmem_was_created( shmem_t* shm );



#endif

#define _CRT_SECURE_NO_WARNINGS

#include "shmem.h"

// std C headers.
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>

// platform-specific headers.
#if _MSC_VER
# define ON_WINDOWS		1
# include <windows.h>
#else
# define ON_WINDOWS		0
# include <unistd.h>
# include <fcntl.h>
# include <sys/mman.h>
#endif

// standard types.
#if ON_WINDOWS
# include "windows/stdint.h"
#else
# include <stdint.h>
#endif

// constants.
static const char*		SHMEM_MAGIC( "SMEM" );
static const size_t		SHMEM_HEADER_SIZE( 64 );

// forward declarations.
struct shmem_header_t;
static shmem_header_t*	shmem_get_header( shmem_t* shm );
static shmem*			shmem_connect( const char* name, size_t size );
static bool				shmem_platform_startup( shmem_t* shm, uint32_t size );
static void				shmem_platform_shutdown( shmem_t* shm );
static void				strfree( char* str );
static char*			strclone( const char* str );
static char*			strjoin( const char* strA, const char* strB );

//===================================================================================
// named shared memory.
//===================================================================================

typedef struct shmem
{
	char*		name;
	uint8_t*	mapped;
#if ON_WINDOWS
	HANDLE		handle;
#else
	int			fd;
#endif
	bool		created;
} shmem_t;

struct shmem_header_t
{
	char		magic[4]; // "SMEM"
	char		pad[4];
	uint64_t	size;
};

//---------------------------------------------------------------------------
shmem_t*	shmem_create_or_open( const char* name, size_t size )
{
	return shmem_connect( name, size );
}

//---------------------------------------------------------------------------
void	shmem_close( shmem_t* shm )
{
	if ( !shm )
		return;

	shmem_platform_shutdown( shm );
	strfree( shm->name );
	free( shm );
}

//---------------------------------------------------------------------------
void*	shmem_ptr( shmem_t* shm )
{
	assert( shm );
	if ( !shm )
		return NULL;

	assert( shm->mapped );
	if ( !shm->mapped )
		return NULL;

	return (void*)( shm->mapped + SHMEM_HEADER_SIZE );
}

//---------------------------------------------------------------------------
const char*	shmem_get_name( shmem_t* shm )
{
	assert( shm );
	if ( !shm )
		return "";

	return shm->name;
}

//---------------------------------------------------------------------------
bool	shmem_was_created( shmem_t* shm )
{
	assert( shm );
	if ( !shm )
		return false;

	return shm->created;
}

//===================================================================================
// named shared memory - private methods.
//===================================================================================

//---------------------------------------------------------------------------
shmem_header_t*	shmem_get_header( shmem_t* shm )
{
	assert( shm );
	if ( !shm )
		return NULL;

	assert( shm->mapped );
	if ( !shm->mapped )
		return NULL;

	return (shmem_header_t*)shm->mapped;
}

//---------------------------------------------------------------------------
shmem_t*	shmem_connect( const char* name, size_t size )
{
	shmem_t* shm = (shmem_t*)calloc( 1, sizeof( shmem_t ) );
	shm->name = strclone( name );

	uint32_t total_size = (uint32_t)( SHMEM_HEADER_SIZE + size );

	if ( shmem_platform_startup( shm, total_size ) )
	{
		assert( shm->mapped != NULL );
		if ( shm->mapped != NULL )
		{
			shmem_header_t* header = shmem_get_header( shm );

			bool success = true;
			if ( shm->created )
			{
				// if we created the shared memory, then initialize the header.
				strncpy( header->magic, SHMEM_MAGIC, 4 );
				header->size = size;
			}
			else
			{
				// if we opened the shared memory, then make sure it's valid.
				if ( strncmp( header->magic, SHMEM_MAGIC, 4 ) )
				{
					success = false;
					fprintf( stderr, "shmem(\"%s\", %d) failed: invalid header\n",
						name,
						(uint32_t)size );
				}
				else if ( header->size != size )
				{
					success = false;
					fprintf( stderr, "shmem(\"%s\", %d) failed: size mismatch (found %d)\n",
						name,
						(uint32_t)size,
						(uint32_t)header->size );
				}
			}
			if ( success )
				return shm;
		}
	}

	shmem_close( shm );
	return NULL;
}

//---------------------------------------------------------------------------
static bool			shmem_platform_startup( shmem_t* shm, uint32_t size )
{
	assert( shm && (shm->mapped == NULL) );

	bool success = false;

#if ON_WINDOWS
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Windows
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	assert( shm->handle == NULL );
	if ( shm->handle == NULL )
	{
		char* fullname = strjoin( "Global\\", shm->name );

		shm->created = false;
		shm->handle = OpenFileMappingA( FILE_MAP_ALL_ACCESS, FALSE, fullname );
		if ( shm->handle == NULL )
		{
			shm->created = true;
			shm->handle = CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, fullname );
		}
		if ( shm->handle != NULL )
		{
			shm->mapped = (uint8_t*)MapViewOfFile( shm->handle, FILE_MAP_ALL_ACCESS, 0, 0, size );
		}

		if ( shm->handle && shm->mapped )
		{
			success = true;
		}
		else
		{
			success = false;
			shmem_platform_shutdown( shm );
		}

		strfree( fullname );
	}
#else
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Posix
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	{
	}
#endif

	return success;
}

//---------------------------------------------------------------------------
void	shmem_platform_shutdown( shmem_t* shm )
{
	if ( !shm )
		return;

	shm->created = false;

#if ON_WINDOWS
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Windows
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	{
		if ( shm->mapped )
		{
			UnmapViewOfFile( shm->mapped );
			shm->mapped = NULL;
		}
		if ( shm->handle )
		{
			CloseHandle( shm->handle );
			shm->handle = NULL;
		}
	}
#else
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Posix
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	{
	}
#endif
}

//---------------------------------------------------------------------------
void	strfree( char* str )
{
	free( str );
}

//---------------------------------------------------------------------------
char*	strclone( const char* str )
{
	size_t len = strlen( str );
	char* ret = (char*)malloc( len + 1 );
	strcpy( ret, str);
	return ret;
}

//---------------------------------------------------------------------------
char*	strjoin( const char* strA, const char* strB )
{
	size_t lenA = strlen( strA );
	size_t lenB = strlen( strB );
	char* ret = (char*)malloc( lenA + lenB + 1 );
	strcpy( ret, strA );
	strcat( ret, strB );
	return ret;
}

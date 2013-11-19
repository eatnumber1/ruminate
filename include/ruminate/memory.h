/** Typed memory allocator
 *
 * @file include/ruminate/memory.h
 */

G_BEGIN_DECLS

void * RUMINATE_EXPORT r_mem_malloc_fn( RType *, GError ** ) RUMINATE_NOEXCEPT;

// This relies on _r_mem_malloc for correct memory management.
#define r_mem_malloc(type, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu\"") \
	({ \
		GError **_err = (error); \
		void *_ret = NULL; \
		RType *_rt = ruminate_get_type((type *) NULL, _err); \
		if( _rt != NULL ) _ret = _r_mem_malloc_sized(_rt, sizeof(type), _err); \
		_ret; \
	}) \
	_Pragma("clang diagnostic pop")

void * RUMINATE_EXPORT _r_mem_malloc_sized( RType *, size_t, GError ** ) RUMINATE_NOEXCEPT;
void * RUMINATE_EXPORT r_mem_malloc_sized_fn( RType *, size_t, GError ** ) RUMINATE_NOEXCEPT;

// This relies on _r_mem_malloc_sized for correct memory management.
#define r_mem_malloc_sized(type, size, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu\"") \
	({ \
		GError **_err = (error); \
		void *_ret = NULL; \
		RType *_rt = ruminate_get_type((type *) NULL, _err); \
		if( _rt != NULL ) _ret = _r_mem_malloc_sized(_rt, size, _err); \
		_ret; \
	}) \
	_Pragma("clang diagnostic pop")

void * RUMINATE_EXPORT r_mem_calloc_fn( RType *, size_t, GError ** ) RUMINATE_NOEXCEPT;

// This relies on _r_mem_calloc for correct memory management.
#define r_mem_calloc(type, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu\"") \
	({ \
		RType *_rt = ruminate_get_type((type *) NULL, error); \
		void *_ret = NULL; \
		if( _rt != NULL ) { \
			_ret = _r_mem_calloc_sized(_rt, sizeof(type), error); \
		} \
		_ret; \
	}) \
	_Pragma("clang diagnostic pop")

void * RUMINATE_EXPORT _r_mem_calloc_sized( RType *, size_t, size_t, GError ** ) RUMINATE_NOEXCEPT;
void * RUMINATE_EXPORT r_mem_calloc_sized_fn( RType *, size_t, size_t, GError ** ) RUMINATE_NOEXCEPT;

// This relies on _r_mem_calloc_sized for correct memory management.
#define r_mem_calloc_sized(type, size, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu\"") \
	({ \
		GError **_err = (error); \
		void *_ret = NULL; \
		RType *_rt = ruminate_get_type((type *) NULL, _err); \
		if( _rt != NULL ) _ret = _r_mem_calloc_sized(_rt, size, _err); \
		_ret; \
	}) \
	_Pragma("clang diagnostic pop")

void * RUMINATE_EXPORT r_mem_realloc( void *, size_t ) RUMINATE_NOEXCEPT;

size_t RUMINATE_EXPORT r_mem_size( void * ) RUMINATE_NOEXCEPT;

void * RUMINATE_EXPORT r_mem_ref( void * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_mem_unref( void * ) RUMINATE_NOEXCEPT;

RType * RUMINATE_EXPORT r_mem_type( void * ) RUMINATE_NOEXCEPT;

G_END_DECLS

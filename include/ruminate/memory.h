/** Typed reference counted memory allocator
 *
 * These functions provide facilities for allocating dynamic memory which you
 * can retrieve the type of via a call to r_mem_type(). This allows you to
 * retrieve the real type of e.x. a `void *`.
 *
 * This memory is also reference counted via calls to r_mem_ref() and
 * r_mem_unref().
 *
 * @file include/ruminate/memory.h
 */

G_BEGIN_DECLS

/** Allocate typed memory.
 *
 * This is the function version of r_mem_malloc()
 *
 * @see r_mem_malloc
 * @return a pointer to dynamically allocated memory
 */
void * RUMINATE_EXPORT r_mem_malloc_fn(
	RType *type /** [in] an RType representing the type of the allocated memory */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Allocate typed memory.
 *
 * This macro allocates memory of size `sizeof(type)` and with type \a type.
 *
 * This memory must be freed via a call to r_mem_unref().
 *
 * @param[in] type the type of the memory to allocate
 * @param[out] error see errors.h
 * @return a pointer to dynamically allocated memory
 */
// This relies on _r_mem_malloc_sized for correct memory management.
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

/// @private
void * RUMINATE_EXPORT _r_mem_malloc_sized( RType *, size_t, GError ** ) RUMINATE_NOEXCEPT;

/** Allocate typed memory with size.
 *
 * This is the function version of r_mem_malloc_sized().
 *
 * @see r_mem_malloc_sized
 * @return a pointer to dynamically allocated memory
 */
void * RUMINATE_EXPORT r_mem_malloc_sized_fn(
	RType *type /** [in] an RType representing the type of the allocated memory */,
	size_t size /** [in] size the size of the memory to allocate. This must be
				         at least as large as the type represented by \a type */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Allocate typed memory with size.
 *
 * This macro allocates memory of size \a size and with type \a type.
 *
 * This memory must be freed via a call to r_mem_unref().
 *
 * @param[in] type the type of the memory to allocate
 * @param[in] size the size of the memory to allocate
 *            This must be at least as large as the type represented by \a type
 * @param[out] error see errors.h
 * @return a pointer to dynamically allocated memory
 */
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

/** Allocate zero'ed typed memory.
 *
 * This is the function version of r_mem_calloc()
 *
 * @see r_mem_calloc
 * @return a pointer to dynamically allocated memory
 */
void * RUMINATE_EXPORT r_mem_calloc_fn(
	RType *type /** [in] an RType representing the type of the allocated memory */,
	size_t nmemb /** [in] the number of \a type sized elements to allocate memory for */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Allocate zero'ed typed memory.
 *
 * This macro allocates enough memory for \a nmemb instances of \a type sized
 * elements and sets the allocated memory to zero.
 *
 * @param[in] type the type of the memory to allocate
 * @param[in] nmemb the number of \a type sized members to allocate.
 * @param[out] error see errors.h
 * @return a pointer to dynamically allocated memory
 */
// This relies on _r_mem_calloc_sized for correct memory management.
#define r_mem_calloc(type, nmemb, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu\"") \
	({ \
		RType *_rt = ruminate_get_type((type *) NULL, error); \
		void *_ret = NULL; \
		if( _rt != NULL ) { \
			_ret = _r_mem_calloc_sized(_rt, sizeof(type), nmemb, error); \
		} \
		_ret; \
	}) \
	_Pragma("clang diagnostic pop")

/// @private
void * RUMINATE_EXPORT _r_mem_calloc_sized( RType *, size_t, size_t, GError ** ) RUMINATE_NOEXCEPT;

/** Allocate zero'ed typed memory, with size.
 *
 * This is the function version of r_mem_calloc_sized()
 *
 * @see r_mem_calloc_sized
 * @return a pointer to dynamically allocated memory
 */
void * RUMINATE_EXPORT r_mem_calloc_sized_fn(
	RType *type /** [in] an RType representing the type of the allocated memory */,
	size_t size /** [in] size the size of the memory to allocate. This must be
				         at least as large as the type represented by \a type */,
	size_t nmemb /** [in] the number of \a type sized elements to allocate memory for */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Allocate zero'ed typed memory, with size.
 *
 * This macro allocates enough memory for \a nmemb instances of \a size sized
 * elements and sets the allocated memory to zero.
 *
 * @param[in] type the type of the memory to allocate
 * @param[in] size the size of the memory to allocate.
 *            This must be at least as large as the type represented by \a type
 * @param[in] nmemb the number of \a type sized members to allocate.
 * @param[out] error see errors.h
 * @return a pointer to dynamically allocated memory
 */
// This relies on _r_mem_calloc_sized for correct memory management.
#define r_mem_calloc_sized(type, size, nmemb, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu\"") \
	({ \
		GError **_err = (error); \
		void *_ret = NULL; \
		RType *_rt = ruminate_get_type((type *) NULL, _err); \
		if( _rt != NULL ) _ret = _r_mem_calloc_sized(_rt, size, nmemb, _err); \
		_ret; \
	}) \
	_Pragma("clang diagnostic pop")

/** Retrieve the allocated size of typed memory.
 *
 * @return the allocated size of \a mem
 */
size_t RUMINATE_EXPORT r_mem_size(
	void *mem /** [in] the typed memory to retrieve the allocated size of */
) RUMINATE_NOEXCEPT;

/** Increment the reference count of typed memory.
 *
 * @return \a mem
 */
void * RUMINATE_EXPORT r_mem_ref(
	void *mem /** [in] the memory to increase the reference count of */
) RUMINATE_NOEXCEPT;

/** Decrease the reference count of typed memory.
 *
 * This function frees \a mem if the reference count reaches zero.
 */
void RUMINATE_EXPORT r_mem_unref(
	void *mem /** [in] the memory to decrease the reference count of */
) RUMINATE_NOEXCEPT;

/** Retrieve the type of typed memory.
 *
 * @return an RType representing the type of \a mem.
 */
RType * RUMINATE_EXPORT r_mem_type(
	void *mem /** [in] the memory to retrieve the type of */
) RUMINATE_NOEXCEPT;

G_END_DECLS

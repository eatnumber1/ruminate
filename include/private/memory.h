// An RMemory preserves the validity of a value by allowing you to hold a
// reference-counted list of g_malloc'ed pointers to values.
typedef struct RMemory {
	struct RMemory *next;
	void *ptr;
	gint refcnt;
} RMemory;

RMemory *r_memory_new( void *, GError ** ) RUMINATE_NOEXCEPT;
RMemory *r_memory_new( void *, RMemory *, GError ** ) RUMINATE_NOEXCEPT;

RMemory *r_memory_ref( RMemory * ) RUMINATE_NOEXCEPT;
void r_memory_unref( RMemory * ) RUMINATE_NOEXCEPT;

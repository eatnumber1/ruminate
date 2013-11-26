typedef struct RMemory {
	RType *type;
	gint refcnt;
	size_t size;
	char data[];
} RMemory;

RMemory *r_memory_new( RType *, size_t ) RUMINATE_NOEXCEPT;

void r_memory_unref( RMemory * ) RUMINATE_NOEXCEPT;
RMemory *r_memory_ref( RMemory * ) RUMINATE_NOEXCEPT;

RMemory *r_memory_from_opaque( void * ) RUMINATE_NOEXCEPT;
void *r_memory_to_opaque( RMemory * ) RUMINATE_NOEXCEPT;

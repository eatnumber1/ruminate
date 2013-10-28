struct RType {
	RTypeId id;
	gint refcnt;
	RString *name;
	RMemory *ptr;
	void *cur;
	RuminateBackend::TypePrx type;
	RuminateBackend::TypeId type_id;
};

bool r_type_init( RType *, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_destroy( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_alloc( RuminateBackend::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_type_free( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_new( RuminateBackend::TypePrx &, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_delete( RType * ) RUMINATE_NOEXCEPT;

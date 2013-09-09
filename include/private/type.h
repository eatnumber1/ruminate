struct RType {
	RTypeId id;
	gint refcnt;
	RString *name;
	RMemory *ptr;
	void *cur;
	Ruminate::TypePrx type;
	Ruminate::TypeId type_id;
};

bool r_type_init( RType *, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_destroy( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_type_free( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_new( Ruminate::TypePrx &, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_delete( RType * ) RUMINATE_NOEXCEPT;

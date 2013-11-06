struct RType {
	RTypeId id;
	gint refcnt;
	RString *name;
	RuminateBackend::TypePrx type;
	RuminateBackend::TypeId type_id;
	struct {
		size_t value;
		bool initialized;
	} size;
};

bool r_type_init( RType *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_destroy( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_alloc( RuminateBackend::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_type_free( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_new( RuminateBackend::TypePrx &, GError ** ) RUMINATE_NOEXCEPT;
void r_type_delete( RType * ) RUMINATE_NOEXCEPT;

struct RTypedefType {
	RType parent;
};

bool r_typedef_type_init( RTypedefType *, GError ** ) RUMINATE_NOEXCEPT;
void r_typedef_type_destroy( RTypedefType * ) RUMINATE_NOEXCEPT;

RTypedefType *r_typedef_type_alloc( RuminateBackend::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_typedef_type_free( RTypedefType * ) RUMINATE_NOEXCEPT;

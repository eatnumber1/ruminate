struct RPointerType {
	RType parent;
};

bool r_pointer_type_init( RPointerType *, GError ** ) RUMINATE_NOEXCEPT;
void r_pointer_type_destroy( RPointerType * ) RUMINATE_NOEXCEPT;

RPointerType *r_pointer_type_alloc( RuminateBackend::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_pointer_type_free( RPointerType * ) RUMINATE_NOEXCEPT;

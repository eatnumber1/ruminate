struct RPointerType {
	RType parent;
};

bool r_pointer_type_init( RPointerType *, GError ** ) noexcept;
void r_pointer_type_destroy( RPointerType * ) noexcept;

RPointerType *r_pointer_type_alloc( Ruminate::TypeId, GError ** ) noexcept;
void r_pointer_type_free( RPointerType * ) noexcept;

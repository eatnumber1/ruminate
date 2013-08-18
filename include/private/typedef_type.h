struct RTypedefType {
	RType parent;
};

bool r_typedef_type_init( RTypedefType *, GError ** ) noexcept;
void r_typedef_type_destroy( RTypedefType * ) noexcept;

RTypedefType *r_typedef_type_alloc( Ruminate::TypeId, GError ** ) noexcept;
void r_typedef_type_free( RTypedefType * ) noexcept;

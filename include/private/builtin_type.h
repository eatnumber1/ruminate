struct RBuiltinType {
	RType parent;
	RBuiltinTypeId id;
};

bool r_builtin_type_init( RBuiltinType *, GError ** ) noexcept;
void r_builtin_type_destroy( RBuiltinType * ) noexcept;

RBuiltinType *r_builtin_type_alloc( Ruminate::TypeId, GError ** ) noexcept;
void r_builtin_type_free( RBuiltinType * ) noexcept;

struct RBuiltinType {
	RType parent;
	RBuiltinTypeId id;
};

bool r_builtin_type_init( RBuiltinType *, GError ** ) RUMINATE_NOEXCEPT;
void r_builtin_type_destroy( RBuiltinType * ) RUMINATE_NOEXCEPT;

RBuiltinType *r_builtin_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_builtin_type_free( RBuiltinType * ) RUMINATE_NOEXCEPT;

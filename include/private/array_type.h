struct RArrayType {
	RType parent;
	struct {
		RuminateBackend::TypeMemberList value;
		bool valid;
	} members;
};

bool r_array_type_init( RArrayType *, GError ** ) RUMINATE_NOEXCEPT;
void r_array_type_destroy( RArrayType * ) RUMINATE_NOEXCEPT;

RArrayType *r_array_type_alloc( RuminateBackend::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_array_type_free( RArrayType * ) RUMINATE_NOEXCEPT;

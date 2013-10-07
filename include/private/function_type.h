struct RFunctionType {
	RRecordType parent;
	RString *name;
};

bool r_function_type_init( RFunctionType *, GError ** ) RUMINATE_NOEXCEPT;
void r_function_type_destroy( RFunctionType * ) RUMINATE_NOEXCEPT;

RFunctionType *r_function_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_function_type_free( RFunctionType * ) RUMINATE_NOEXCEPT;

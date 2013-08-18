struct RFunctionType {
	RRecordType parent;
};

bool r_function_type_init( RFunctionType *, GError ** ) noexcept;
void r_function_type_destroy( RFunctionType * ) noexcept;

RFunctionType *r_function_type_alloc( Ruminate::TypeId, GError ** ) noexcept;
void r_function_type_free( RFunctionType * ) noexcept;

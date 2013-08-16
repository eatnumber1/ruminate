struct RType {
	RTypeId id;
	gint refcnt;
	RString *name;
	Ruminate::TypePrx type;
	Ruminate::TypeId type_id;
};

bool r_type_init( RType *, Ruminate::TypePrx &, GError ** ) noexcept;
void r_type_destroy( RType * ) noexcept;

RType *r_type_alloc( Ruminate::TypeId, GError ** ) noexcept;
void r_type_free( RType * ) noexcept;

RType *r_type_new( Ruminate::TypePrx &, GError ** ) noexcept;
void r_type_delete( RType * ) noexcept;

#ifndef _TYPE_CPP_
extern template gxx_call_proto(Ruminate::TypeId);
extern template gxx_call_proto(Ruminate::TypePrx);
#endif

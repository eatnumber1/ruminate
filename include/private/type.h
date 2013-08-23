struct RType {
	RTypeId id;
	gint refcnt;
	RString *name;
	void *mem;
	Ruminate::TypePrx type;
	Ruminate::TypeId type_id;
};

typedef void (*MemFreeFunction)( void * );

bool r_type_init( RType *, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_destroy( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_type_free( RType * ) RUMINATE_NOEXCEPT;

RType *r_type_new( Ruminate::TypePrx &, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_delete( RType * ) RUMINATE_NOEXCEPT;

#if 0
#ifndef _TYPE_CPP_
extern template gxx_call_proto(Ruminate::TypeId);
extern template gxx_call_proto(Ruminate::TypePrx);
#endif
#endif

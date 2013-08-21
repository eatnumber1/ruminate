struct RRecordType {
	RTagType parent;
	RRecordTypeId id;
	bool members_init;
	Ruminate::TypeMemberList members;
};

bool r_record_type_init( RRecordType *, GError ** ) RUMINATE_NOEXCEPT;
void r_record_type_destroy( RRecordType * ) RUMINATE_NOEXCEPT;

RRecordType *r_record_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_record_type_free( RRecordType * ) RUMINATE_NOEXCEPT;

#if 0
#ifndef _RECORD_TYPE_CPP_
extern template gxx_call_proto(Ruminate::TypeMemberList);
#endif
#endif

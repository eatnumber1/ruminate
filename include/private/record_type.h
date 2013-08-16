struct RRecordType {
	RTagType parent;
	RRecordTypeId id;
	bool members_init;
	Ruminate::TypeMemberList members;
};

bool r_record_type_init( RRecordType *, GError ** ) noexcept;
void r_record_type_destroy( RRecordType * ) noexcept;

RRecordType *r_record_type_alloc( Ruminate::TypeId, GError ** ) noexcept;
void r_record_type_free( RRecordType * ) noexcept;

#ifndef _RECORD_TYPE_CPP_
extern template gxx_call_proto(Ruminate::TypeMemberList);
#endif

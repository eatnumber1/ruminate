struct RRecordMember {
	RTypeMember parent;
	RRecordMemberId id;
	RString *name;
};

void r_record_member_destroy( RRecordMember * ) RUMINATE_NOEXCEPT;
bool r_record_member_init( RRecordMember *, GError ** ) RUMINATE_NOEXCEPT;

RRecordMember *r_record_member_alloc( GError ** ) RUMINATE_NOEXCEPT;
void r_record_member_free( RRecordMember * ) RUMINATE_NOEXCEPT;

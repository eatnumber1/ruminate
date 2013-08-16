struct RRecordMember {
	RRecordMemberId id;
	gint refcnt;
	RString *name;
	Ruminate::TypeMemberPrx member;
};

void r_record_member_destroy( RRecordMember * ) noexcept;
bool r_record_member_init( RRecordMember *, Ruminate::TypeMemberPrx &, GError ** ) noexcept;
RRecordMember *r_record_member_new( Ruminate::TypeMemberPrx &, GError ** ) noexcept;

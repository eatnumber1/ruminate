struct RTypeMember {
	RTypeMemberId id;
	gint refcnt;
	RValue mem;
	Ruminate::TypeMemberPrx member;
};

void r_type_member_destroy( RTypeMember * ) RUMINATE_NOEXCEPT;
bool r_type_member_init( RTypeMember *tm, RValue rv, GError **error ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_alloc( RTypeMemberId, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_free( RTypeMember * ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_new( Ruminate::TypeMemberPrx &, RTypeMemberId id, RValue, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_delete( RTypeMember * ) RUMINATE_NOEXCEPT;

bool _r_type_member_offset( Ruminate::TypeMemberPrx &, off_t *, GError ** ) RUMINATE_NOEXCEPT;

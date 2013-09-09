struct RTypeMember {
	RTypeMemberId id;
	gint refcnt;
	RMemory *ptr;
	void *cur;
	Ruminate::TypeMemberPrx member;
};

void r_type_member_destroy( RTypeMember * ) RUMINATE_NOEXCEPT;
bool r_type_member_init( RTypeMember *, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_alloc( RTypeMemberId, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_free( RTypeMember * ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_new( Ruminate::TypeMemberPrx &, RTypeMemberId id, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_delete( RTypeMember * ) RUMINATE_NOEXCEPT;

bool _r_type_member_offset( Ruminate::TypeMemberPrx &, off_t *, GError ** ) RUMINATE_NOEXCEPT;

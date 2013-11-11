struct RTypeMember {
	RTypeMemberId id;
	gint refcnt;
	RuminateBackend::TypeMemberPrx member;
	RType *type;
};

void r_type_member_destroy( RTypeMember * ) RUMINATE_NOEXCEPT;
bool r_type_member_init( RTypeMember *, RType *, GError ** ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_alloc( RType *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_free( RTypeMember * ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_new( RuminateBackend::TypeMemberPrx &, RType *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_delete( RTypeMember * ) RUMINATE_NOEXCEPT;

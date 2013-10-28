struct RTypeMember {
	RTypeMemberId id;
	gint refcnt;
	RMemory *ptr;
	void *cur;
	RuminateBackend::TypeMemberPrx member;
	RType *type;
};

void r_type_member_destroy( RTypeMember * ) RUMINATE_NOEXCEPT;
bool r_type_member_init( RTypeMember *, RType *, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_alloc( RTypeMemberId, RType *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_free( RTypeMember * ) RUMINATE_NOEXCEPT;

RTypeMember *r_type_member_new( RuminateBackend::TypeMemberPrx &, RType *, RMemory *, void *, GError ** ) RUMINATE_NOEXCEPT;
void r_type_member_delete( RTypeMember * ) RUMINATE_NOEXCEPT;

bool _r_type_member_offset( RuminateBackend::TypeMemberPrx &, off_t *, GError ** ) RUMINATE_NOEXCEPT;

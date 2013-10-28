struct RAggregateMember {
	RTypeMember parent;
	RAggregateMemberId id;
	RString *name;
};

void r_aggregate_member_destroy( RAggregateMember * ) RUMINATE_NOEXCEPT;
bool r_aggregate_member_init( RAggregateMember *, RType *, GError ** ) RUMINATE_NOEXCEPT;

RAggregateMember *r_aggregate_member_alloc( RType *, GError ** ) RUMINATE_NOEXCEPT;
void r_aggregate_member_free( RAggregateMember * ) RUMINATE_NOEXCEPT;

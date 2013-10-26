struct RAggregateType {
	RType parent;
	RAggregateTypeId id;
	bool members_init;
	Ruminate::TypeMemberList members;
};

bool r_aggregate_type_init( RAggregateType *, GError ** ) RUMINATE_NOEXCEPT;
void r_aggregate_type_destroy( RAggregateType * ) RUMINATE_NOEXCEPT;

RAggregateType *r_aggregate_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_aggregate_type_free( RAggregateType * ) RUMINATE_NOEXCEPT;

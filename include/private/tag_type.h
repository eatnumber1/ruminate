struct RTagType {
	RAggregateType parent;
	RTagTypeId id;
	RString *name;
};

bool r_tag_type_init( RTagType *, GError ** ) RUMINATE_NOEXCEPT;
void r_tag_type_destroy( RTagType * ) RUMINATE_NOEXCEPT;

RTagType *r_tag_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT;
void r_tag_type_free( RTagType * ) RUMINATE_NOEXCEPT;

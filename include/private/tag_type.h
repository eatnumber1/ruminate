struct RTagType {
	RType parent;
	RTagTypeId id;
	RString *name;
};

bool r_tag_type_init( RTagType *, GError ** ) noexcept;
void r_tag_type_destroy( RTagType * ) noexcept;

RTagType *r_tag_type_alloc( Ruminate::TypeId, GError ** ) noexcept;
void r_tag_type_free( RTagType * ) noexcept;

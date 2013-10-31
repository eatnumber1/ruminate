enum REnumMemberValueType {
	ENUM_MEMBER_VALUE_TYPE_SIGNED,
	ENUM_MEMBER_VALUE_TYPE_UNSIGNED
};

struct REnumMember {
	RAggregateMember parent;
	struct {
		enum REnumMemberValueType type;
		union {
			uintmax_t unsignd;
			intmax_t signd;
		};
		bool initialized;
	} value;
};

void r_enum_member_destroy( REnumMember * ) RUMINATE_NOEXCEPT;
bool r_enum_member_init( REnumMember *, RType *, GError ** ) RUMINATE_NOEXCEPT;

REnumMember *r_enum_member_alloc( RType *, GError ** ) RUMINATE_NOEXCEPT;
void r_enum_member_free( REnumMember * ) RUMINATE_NOEXCEPT;

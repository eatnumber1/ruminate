#ifndef _RUMINATE_TYPE_MEMBER_H_
#define _RUMINATE_TYPE_MEMBER_H_

typedef enum RTypeMemberId {
	R_TYPE_MEMBER_RECORD,
	R_TYPE_MEMBER_ARRAY
} RTypeMemberId;

typedef struct RTypeMember RTypeMember;

G_BEGIN_DECLS

RTypeMemberId RUMINATE_EXPORT r_type_member_id( RTypeMember *, GError ** ) RUMINATE_NOEXCEPT;
RType * RUMINATE_EXPORT r_type_member_type( RTypeMember *, GError ** ) RUMINATE_NOEXCEPT;
off_t RUMINATE_EXPORT r_type_member_offset( RTypeMember *, GError ** ) RUMINATE_NOEXCEPT;

RTypeMember * RUMINATE_EXPORT r_type_member_ref( RTypeMember * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_type_member_unref( RTypeMember * ) RUMINATE_NOEXCEPT;

G_END_DECLS

#endif

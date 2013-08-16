#ifndef _RUMINATE_TYPE_MEMBER_H_
#define _RUMINATE_TYPE_MEMBER_H_

typedef enum RTypeMemberIdentifier {
	R_TYPE_MEMBER_INVALID = 0,
	R_TYPE_MEMBER_UNSPEC,
	R_TYPE_MEMBER_BIT_FIELD
} RTypeMemberIdentifier;

typedef struct RTypeMember RTypeMember;
typedef struct RBitFieldMember RBitFieldMember;

void r_type_member_ref( RTypeMember * );
void r_type_member_unref( RTypeMember * );

RTypeMemberIdentifier r_type_member_id( RTypeMember *, GError ** );
RBitFieldMember *r_type_member_as_bit_field_member( RTypeMember *, GError ** );

#endif

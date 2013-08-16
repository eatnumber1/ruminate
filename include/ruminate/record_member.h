#ifndef _RUMINATE_RECORD_MEMBER_H_
#define _RUMINATE_RECORD_MEMBER_H_

typedef enum RRecordMemberId {
	R_RECORD_MEMBER_BITFIELD,
	R_RECORD_MEMBER_OTHER
} RRecordMemberId;

typedef struct RRecordMember RRecordMember;

G_BEGIN_DECLS

RRecordMemberId RUMINATE_EXPORT r_record_member_id( RRecordMember *, GError ** ) RUMINATE_NOEXCEPT;
const RString * RUMINATE_EXPORT r_record_member_name( RRecordMember *, GError ** ) RUMINATE_NOEXCEPT;

RRecordMember * RUMINATE_EXPORT r_record_member_ref( RRecordMember * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_record_member_unref( RRecordMember * ) RUMINATE_NOEXCEPT;

G_END_DECLS

#endif

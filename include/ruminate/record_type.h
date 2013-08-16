typedef enum RRecordTypeId {
	R_RECORD_TYPE_STRUCTURE,
	R_RECORD_TYPE_UNION
} RRecordTypeId;

typedef struct RRecordType RRecordType;

G_BEGIN_DECLS

RRecordTypeId RUMINATE_EXPORT r_record_type_id( RRecordType *, GError ** ) RUMINATE_NOEXCEPT;

gsize RUMINATE_EXPORT r_record_type_nmembers( RRecordType *, GError ** ) RUMINATE_NOEXCEPT;
RRecordMember * RUMINATE_EXPORT r_record_type_member_at( RRecordType *, size_t, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS

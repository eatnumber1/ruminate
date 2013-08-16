typedef enum RTagTypeId {
	R_TAG_TYPE_RECORD,
	R_TAG_TYPE_ENUM
} RTagTypeId;

typedef struct RTagType RTagType;

G_BEGIN_DECLS

RTagTypeId RUMINATE_EXPORT r_tag_type_id( RTagType *, GError ** ) RUMINATE_NOEXCEPT;
const RString * RUMINATE_EXPORT r_tag_type_name( RTagType *, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS

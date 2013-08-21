typedef enum RTypeId {
	R_TYPE_BUILTIN,
	R_TYPE_TAG,
	R_TYPE_TYPEDEF,
	R_TYPE_POINTER,
	R_TYPE_UNKNOWN
} RTypeId;

typedef struct RType RType;

G_BEGIN_DECLS

RTypeId RUMINATE_EXPORT r_type_id( RType *, GError ** ) RUMINATE_NOEXCEPT;
RString * RUMINATE_EXPORT r_type_name( RType *, GError ** ) RUMINATE_NOEXCEPT;

RType * RUMINATE_EXPORT r_type_ref( RType * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_type_unref( RType * ) RUMINATE_NOEXCEPT;

RType * RUMINATE_EXPORT r_type_pointer( RType *, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS

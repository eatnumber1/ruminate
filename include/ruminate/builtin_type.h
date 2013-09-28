typedef enum RBuiltinTypeId {
	R_BUILTIN_TYPE_INT,
	R_BUILTIN_TYPE_LONG,
	R_BUILTIN_TYPE_DOUBLE,
	R_BUILTIN_TYPE_SHORT,
	R_BUILTIN_TYPE_CHAR,
	R_BUILTIN_TYPE_VOID,
	R_BUILTIN_TYPE_BOOL
} RBuiltinTypeId;

typedef struct RBuiltinType RBuiltinType;

G_BEGIN_DECLS

RBuiltinTypeId RUMINATE_EXPORT r_builtin_type_id( RBuiltinType *, GError ** ) RUMINATE_NOEXCEPT;

bool RUMINATE_EXPORT r_builtin_type_is_signed( RBuiltinType *, GError ** ) RUMINATE_NOEXCEPT;
bool RUMINATE_EXPORT r_builtin_type_is_unsigned( RBuiltinType *, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS

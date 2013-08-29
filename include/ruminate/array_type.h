typedef struct RArrayType RArrayType;

G_BEGIN_DECLS

size_t RUMINATE_EXPORT r_array_type_size( RArrayType *, GError ** ) RUMINATE_NOEXCEPT;
RType * RUMINATE_EXPORT r_array_type_member_type( RArrayType *, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS

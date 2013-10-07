typedef struct RFunctionType RFunctionType;

G_BEGIN_DECLS

RType * RUMINATE_EXPORT r_function_type_return_type( RFunctionType *, GError ** ) RUMINATE_NOEXCEPT;
RString * RUMINATE_EXPORT r_function_type_name( RFunctionType *, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS

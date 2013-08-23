typedef void (*RPointerDeleter)( void * );

G_BEGIN_DECLS

void r_ptr_set_deleter( void *, RPointerDeleter ) RUMINATE_NOEXCEPT;
void *r_ptr_ref( void * ) RUMINATE_NOEXCEPT;
void r_ptr_unref( void * ) RUMINATE_NOEXCEPT;

G_END_DECLS

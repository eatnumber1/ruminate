G_BEGIN_DECLS

bool RUMINATE_EXPORT rumination_destroy( GError ** ) RUMINATE_NOEXCEPT;
bool RUMINATE_EXPORT rumination_init( int *, char *[], GError ** ) RUMINATE_NOEXCEPT;

bool RUMINATE_EXPORT rumination_begin_get_type_by_variable_name( const char *, GError ** ) RUMINATE_NOEXCEPT;
RType * RUMINATE_EXPORT rumination_end_get_type_by_variable_name( void *, GError ** ) RUMINATE_NOEXCEPT;

RFrameList * RUMINATE_EXPORT rumination_backtrace( GError ** ) RUMINATE_NOEXCEPT;

__attribute__((noinline))
void RUMINATE_EXPORT rumination_hit_breakpoint() RUMINATE_NOEXCEPT;

G_END_DECLS

#define rumination_get_type(expr, err) ({ \
	__typeof__(expr) *_expr = g_malloc(sizeof(__typeof__(expr))); \
	r_ptr_set_deleter(_expr, g_free); \
	*_expr = (expr); \
	RType *ret; \
	if( !rumination_begin_get_type_by_variable_name("_expr", err) ) { \
		ret = NULL; \
	} else { \
		rumination_hit_breakpoint(); \
		ret = rumination_end_get_type_by_variable_name(_expr, err); \
	} \
	ret; \
})

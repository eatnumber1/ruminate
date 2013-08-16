typedef struct RString {
	GString *str;

	/* private */
	gint refcnt;
} RString;

G_BEGIN_DECLS

RString * RUMINATE_EXPORT r_string_ref( RString * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_string_unref( RString * ) RUMINATE_NOEXCEPT;

G_END_DECLS

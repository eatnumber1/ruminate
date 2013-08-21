struct RString {
	GString *gstr;
	gint refcnt;
};

RString *r_string_new( const char *, gsize ) RUMINATE_NOEXCEPT;
RString *r_string_new( const char * ) RUMINATE_NOEXCEPT;

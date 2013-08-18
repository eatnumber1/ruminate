struct RString {
	GString *gstr;
	gint refcnt;
};

RString *r_string_new( const char *, gsize ) noexcept;
RString *r_string_new( const char * ) noexcept;

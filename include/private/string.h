struct RString {
	GString *gstr;
	gint refcnt;
	struct {
		GQuark value;
		bool initialized;
	} quark;
};

RString *r_string_new( const char *, gsize ) RUMINATE_NOEXCEPT;
RString *r_string_new( const char * ) RUMINATE_NOEXCEPT;
RString *r_string_new( const std::string & ) RUMINATE_NOEXCEPT;

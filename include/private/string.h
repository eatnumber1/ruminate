struct RString {
	GString *gstr;
	gint refcnt;
	struct {
		GQuark value;
		bool initialized;
	} quark;
};

RString *r_string_new_cxx( const std::string & ) RUMINATE_NOEXCEPT;

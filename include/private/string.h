struct RString {
	GString *gstr;
	struct {
		GQuark value;
		bool initialized;
	} quark;
	gint refcnt;
};

RString *r_string_new_cxx( const std::string & ) RUMINATE_NOEXCEPT;

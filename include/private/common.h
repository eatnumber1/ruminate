// TODO: When C++11 support is available, switch to using this.
#if 0
bool gxx_call( const std::function<void()> &func, GError **error ) noexcept;

template <typename R> bool gxx_call( const std::function<R()> &func, R *out, GError **error ) noexcept {
	return gxx_call([out, &func](){ *out = func(); }, error);
}

#define gxx_call_proto(type) bool gxx_call<type>( const std::function<type()> &, type *, GError ** ) noexcept

#ifndef _COMMON_CPP_
extern template gxx_call_proto(std::string);
extern template gxx_call_proto(bool);
extern template gxx_call_proto(size_t);
#endif
#endif

#define gxx_call(body, error) ({ \
	bool ret = true; \
	try { \
		body; \
	} catch( Ice::Exception &ex ) { \
		/* If we throw here, meh. */ \
		std::ostringstream ss; \
		ss << ex; \
		g_set_error( \
			error, \
			RUMINATE_ERROR, \
			RUMINATE_ERROR_ICE, \
			"%s", \
			ss.str().c_str() \
		); \
		ret = false; \
	} catch( std::exception &ex ) { \
		g_set_error( \
			error, \
			RUMINATE_ERROR, \
			RUMINATE_ERROR_STDLIB, \
			"%s", \
			ex.what() \
		); \
		ret = false; \
	} catch(...) { \
		g_set_error_literal( \
			error, \
			RUMINATE_ERROR, \
			RUMINATE_ERROR_UNSPEC, \
			"Unspecified error" \
		); \
		ret = false; \
	} \
	ret; \
})

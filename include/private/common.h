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

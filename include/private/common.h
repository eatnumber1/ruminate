#define gxx_call(body, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu\"") \
	({ \
		bool _ret = true; \
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
			_ret = false; \
		} catch( std::exception &ex ) { \
			g_set_error( \
				error, \
				RUMINATE_ERROR, \
				RUMINATE_ERROR_STDLIB, \
				"%s", \
				ex.what() \
			); \
			_ret = false; \
		} catch(...) { \
			g_set_error_literal( \
				error, \
				RUMINATE_ERROR, \
				RUMINATE_ERROR_UNSPEC, \
				"Unspecified error" \
			); \
			_ret = false; \
		} \
		_ret; \
	}) \
	_Pragma("clang diagnostic pop")

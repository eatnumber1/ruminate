#if JANSSON_MAJOR_VERSION <= 2 && JANSSON_MINOR_VERSION < 4
#define json_boolean(val) ((val) ? json_true() : json_false())
#endif

struct JsonState {
	GData *handlers; // { GQuark -> JsonSerializerCallback * }
	gint refcnt;
	JsonFlags flags;
};

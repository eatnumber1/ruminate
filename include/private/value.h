// This is passed by value, so keep it small.
typedef struct RValue {
	void *top;
	void *cur;
} RValue;

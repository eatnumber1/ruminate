#define RUMINATE_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define RUMINATE_NOEXCEPT noexcept(true)
#else
#define RUMINATE_NOEXCEPT
#endif

__attribute__((deprecated("unimplemented")))
void unimplemented( GError ** ) RUMINATE_NOEXCEPT;
#define RUMINATE_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define RUMINATE_NOEXCEPT throw()
#else
#define RUMINATE_NOEXCEPT
#endif

#define RUMINATE_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))

/// @private
__attribute__((deprecated("unimplemented")))
void unimplemented( GError ** ) RUMINATE_NOEXCEPT;

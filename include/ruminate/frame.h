typedef struct RFrameList RFrameList;
typedef struct RFrame RFrame;

G_BEGIN_DECLS

size_t RUMINATE_EXPORT r_frame_list_size( RFrameList *, GError ** ) RUMINATE_NOEXCEPT;
RFrame * RUMINATE_EXPORT r_frame_list_at( RFrameList *, size_t, GError ** ) RUMINATE_NOEXCEPT;

void RUMINATE_EXPORT r_frame_list_ref( RFrameList * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_frame_list_unref( RFrameList * ) RUMINATE_NOEXCEPT;

void RUMINATE_EXPORT r_frame_ref( RFrame * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_frame_unref( RFrame * ) RUMINATE_NOEXCEPT;

// TODO: These should be returning RStrings
const char * RUMINATE_EXPORT r_frame_function_name( RFrame *, GError ** ) RUMINATE_NOEXCEPT;
const char * RUMINATE_EXPORT r_frame_module_name( RFrame *, GError ** ) RUMINATE_NOEXCEPT;
const char * RUMINATE_EXPORT r_frame_compile_unit_name( RFrame *, GError ** ) RUMINATE_NOEXCEPT;
RType * RUMINATE_EXPORT r_frame_function_type( RFrame *, GError ** ) RUMINATE_NOEXCEPT;
uint32_t RUMINATE_EXPORT r_frame_line( RFrame *, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS

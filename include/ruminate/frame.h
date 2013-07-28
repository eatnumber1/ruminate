#ifndef _RUMINATE_FRAME_H_
#define _RUMINATE_FRAME_H_

typedef struct RFrameList RFrameList;
typedef struct RFrame RFrame;

G_BEGIN_DECLS

size_t r_frame_list_size( RFrameList * );
RFrame *r_frame_list_at( RFrameList *, size_t );

void r_frame_list_ref( RFrameList * );
void r_frame_list_unref( RFrameList * );

void r_frame_ref( RFrame * );
void r_frame_unref( RFrame * );

const char *r_frame_function_name( RFrame * );
const char *r_frame_module_name( RFrame * );
const char *r_frame_compile_unit_name( RFrame * );
RType *r_frame_function_type( RFrame *, GError ** );
uint32_t r_frame_line( RFrame * );

G_END_DECLS

#endif

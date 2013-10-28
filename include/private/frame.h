struct RFrameList {
	RFrame **frames;
	size_t nframes;
	gint refcnt;
};

struct RFrame {
	RType *functionType;
	RuminateBackend::Frame frame;
	gint refcnt;
	RString *function_name;
	RString *module_name;
	RString *compile_unit_name;
};

RFrameList *r_frame_list_new( RuminateBackend::FrameList &, GError ** ) RUMINATE_NOEXCEPT;
RFrame *r_frame_new( RuminateBackend::Frame &, GError ** ) RUMINATE_NOEXCEPT;

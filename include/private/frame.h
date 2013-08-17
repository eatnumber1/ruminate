struct RFrameList {
	RFrame **frames;
	size_t nframes;
	gint refcnt;
};

struct RFrame {
	RType *functionType;
	Ruminate::Frame frame;
	gint refcnt;
};

RFrameList *r_frame_list_new( Ruminate::FrameList &&, GError ** ) noexcept;
RFrame *r_frame_new( Ruminate::Frame &&, GError ** ) noexcept;
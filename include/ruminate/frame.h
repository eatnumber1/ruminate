/** An opaque struct representing a call stack.
 *
 * A call stack is a list of one or more RFrame instances.
 *
 * @struct RFrameList
 * @see RFrame
 */
typedef struct RFrameList RFrameList;

/** An opaque struct representing a call stack frame.
 *
 * @struct RFrame
 */
typedef struct RFrame RFrame;

G_BEGIN_DECLS

/** Get the number of elements in this frame list.
 *
 * @return the number of elements in this frame list.
 * @memberof RFrameList
 */
size_t RUMINATE_EXPORT r_frame_list_size(
	RFrameList *list /** [in] the frame list to get the size of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get an RFrame from this RFrameList
 *
 * @return the RFrame at index \a index
 * @memberof RFrameList
 */
RFrame * RUMINATE_EXPORT r_frame_list_at(
	RFrameList *list /** [in] the frame list to get an element from */,
	size_t index /** [in] the index of the RFrame to get */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Increment the reference count on this RFrameList
 *
 * @memberof RFrameList
 */
void RUMINATE_EXPORT r_frame_list_ref(
	RFrameList *list /** [in] the frame list to increment the reference count of */
) RUMINATE_NOEXCEPT;

/** Decrement the reference count of this RFrameList.
 *
 * The RFrameList will be freed if it's reference count drops to zero.
 * 
 * @memberof RFrameList
 */
void RUMINATE_EXPORT r_frame_list_unref(
	RFrameList *list /** [in] the frame list to decrement the reference count of */
) RUMINATE_NOEXCEPT;

/** Increment the reference count of this RFrame.
 *
 * @memberof RFrame
 */
void RUMINATE_EXPORT r_frame_ref(
	RFrame *frame /** [in] the frame to increment the reference count of */
) RUMINATE_NOEXCEPT;

/** Decrement the reference count of this RFrame.
 *
 * The RFrame will be freed if it's reference count drops to zero.
 *
 * @memberof RFrame
 */
void RUMINATE_EXPORT r_frame_unref(
	RFrame *frame /** [in] the frame to decrement the reference count of */
) RUMINATE_NOEXCEPT;

/** Get the name of the function this RFrame represents.
 * 
 * @memberof RFrame
 * @return a RString containing the name of this function.
 */
RString * RUMINATE_EXPORT r_frame_function_name(
	RFrame *frame /** [in] the frame to get the name of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the name of the module which contains this frame.
 *
 * This is usually the name of the executable or library.
 *
 * @memberof RFrame
 * @return a RString containing the name of this module.
 */
RString * RUMINATE_EXPORT r_frame_module_name(
	RFrame *frame /** [in] the frame to get the module name of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the name of the compile unit which contains this frame.
 *
 * This is usually the name of the file which defined this function.
 *
 * @memberof RFrame
 * @return a RString containing the name of this compile unit.
 */
RString * RUMINATE_EXPORT r_frame_compile_unit_name(
	RFrame *frame /** [in] the frame to get the compile unit name of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the type of this function.
 *
 * @memberof RFrame
 * @return an RType representing the type of this function.
 */
RType * RUMINATE_EXPORT r_frame_function_type(
	RFrame *frame /** [in] the frame to get the type of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the line number that this frame is at.
 *
 * @memberof RFrame
 * @return the line number that this frame is at
 */
uintmax_t RUMINATE_EXPORT r_frame_line(
	RFrame *frame /** [in] the frame to get the line number of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS

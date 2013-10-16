/** An opaque struct representing a string.
 *
 * An RString is a reference counted array of characters.
 *
 * @struct RString
 */
typedef struct RString RString;

G_BEGIN_DECLS

/** Increase the reference count on this RString
 *
 * @return \a string
 * @memberof RString
 */
RString * RUMINATE_EXPORT r_string_ref(
	RString *string /** [in] the string to increase the reference count of */
) RUMINATE_NOEXCEPT;

/** Decrease the reference count on this RString
 *
 * If the reference count of this RString drops to zero, the string will be
 * freed.
 *
 * @memberof RString
 */
void RUMINATE_EXPORT r_string_unref(
	RString *string /** [in] the string to decrease the reference count of */
) RUMINATE_NOEXCEPT;

/** Get the C-style string (array of characters) backing this RString.
 *
 * The behavior is undefined if this array is modified.
 *
 * @return the array of characters backing this RString
 * @memberof RString
 */
const char * RUMINATE_EXPORT r_string_bytes(
	RString *string /** [in] the string to get the array of characters of */
) RUMINATE_NOEXCEPT;

/** Get the length of this RString
 *
 * @return the length of this RString
 * @memberof RString
 */
size_t RUMINATE_EXPORT r_string_length(
	RString *string /** [in] the string to get the length of */
) RUMINATE_NOEXCEPT;

G_END_DECLS

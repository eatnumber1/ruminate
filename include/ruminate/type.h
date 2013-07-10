#ifndef _RUMINATE_TYPE_H_
#define _RUMINATE_TYPE_H_

#include <glib.h>

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
#define STATIC_ASSERT(...) static_assert(__VA_ARGS__)
#else
#define STATIC_ASSERT(...) _Static_assert(__VA_ARGS__)
#endif

// Shamelessly copied & modified from lldb-enumerations.h
typedef enum TypeIdentifier {
	TYPE_CLASS_INVALID           = (0u),
	TYPE_CLASS_ARRAY             = (1u << 0),
	TYPE_CLASS_BLOCKPOINTER      = (1u << 1),
	TYPE_CLASS_BUILTIN           = (1u << 2),
	TYPE_CLASS_CLASS             = (1u << 3),
	TYPE_CLASS_COMPLEXFLOAT      = (1u << 4),
	TYPE_CLASS_COMPLEXINTEGER    = (1u << 5),
	TYPE_CLASS_ENUMERATION       = (1u << 6),
	TYPE_CLASS_FUNCTION          = (1u << 7),
	TYPE_CLASS_MEMBERPOINTER     = (1u << 8),
	TYPE_CLASS_OBJCOBJECT        = (1u << 9),
	TYPE_CLASS_OBJCINTERFACE     = (1u << 10),
	TYPE_CLASS_OBJCOBJECTPOINTER = (1u << 11),
	TYPE_CLASS_POINTER           = (1u << 12),
	TYPE_CLASS_REFERENCE         = (1u << 13),
	TYPE_CLASS_STRUCT            = (1u << 14),
	TYPE_CLASS_TYPEDEF           = (1u << 15),
	TYPE_CLASS_UNION             = (1u << 16),
	TYPE_CLASS_VECTOR            = (1u << 17),
	// Define the last type class as the MSBit of a 32 bit value
	TYPE_CLASS_OTHER             = (1u << 31),
	// Define a mask that can be used for any type when finding types
	TYPE_CLASS_ANY               = (0xffffffffu)
} TypeIdentifier;

typedef enum BasicTypeIdentifier {
	BASIC_TYPE_INVALID = 0,
	BASIC_TYPE_VOID = 1,
	BASIC_TYPE_CHAR,
	BASIC_TYPE_SIGNEDCHAR,
	BASIC_TYPE_UNSIGNEDCHAR,
	BASIC_TYPE_WCHAR,
	BASIC_TYPE_SIGNEDWCHAR,
	BASIC_TYPE_UNSIGNEDWCHAR,
	BASIC_TYPE_CHAR16,
	BASIC_TYPE_CHAR32,
	BASIC_TYPE_SHORT,
	BASIC_TYPE_UNSIGNEDSHORT,
	BASIC_TYPE_INT,
	BASIC_TYPE_UNSIGNEDINT,
	BASIC_TYPE_LONG,
	BASIC_TYPE_UNSIGNEDLONG,
	BASIC_TYPE_LONGLONG,
	BASIC_TYPE_UNSIGNEDLONGLONG,
	BASIC_TYPE_INT128,
	BASIC_TYPE_UNSIGNEDINT128,
	BASIC_TYPE_BOOL,
	BASIC_TYPE_HALF,
	BASIC_TYPE_FLOAT,
	BASIC_TYPE_DOUBLE,
	BASIC_TYPE_LONGDOUBLE,
	BASIC_TYPE_FLOATCOMPLEX,
	BASIC_TYPE_DOUBLECOMPLEX,
	BASIC_TYPE_LONGDOUBLECOMPLEX,
	BASIC_TYPE_OBJCID,
	BASIC_TYPE_OBJCCLASS,
	BASIC_TYPE_OBJCSEL,
	BASIC_TYPE_NULLPTR,
	BASIC_TYPE_OTHER
} BasicTypeIdentifier;

typedef struct Type {
	const char *name;
	size_t size;
	TypeIdentifier id;
	void *priv;
} Type;

typedef struct StructType {
	Type type;
	size_t nfields;
} StructType;

typedef struct StructMember {
	Type *type;
	const char *name;
	off_t offset;
	void *priv;
} StructMember;

typedef struct BasicType {
	Type type;
	BasicTypeIdentifier id;
} BasicType;

G_BEGIN_DECLS

void type_unref( Type * );
void type_ref( Type * );

void struct_member_ref( StructMember * );
void struct_member_unref( StructMember * );

BasicType *type_as_basic( Type *, GError ** );
StructType *type_as_struct( Type *, GError ** );
StructMember *struct_type_field_at_index( StructType *, size_t, GError ** );

G_END_DECLS

#endif

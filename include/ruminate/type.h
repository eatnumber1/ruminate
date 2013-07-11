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
	TYPE_CLASS_PRIMITIVE         = (1u << 2),
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

typedef enum PrimitiveTypeIdentifier {
	PRIMITIVE_TYPE_INVALID = 0,
	PRIMITIVE_TYPE_VOID = 1,
	PRIMITIVE_TYPE_CHAR,
	PRIMITIVE_TYPE_SIGNEDCHAR,
	PRIMITIVE_TYPE_UNSIGNEDCHAR,
	PRIMITIVE_TYPE_WCHAR,
	PRIMITIVE_TYPE_SIGNEDWCHAR,
	PRIMITIVE_TYPE_UNSIGNEDWCHAR,
	PRIMITIVE_TYPE_CHAR16,
	PRIMITIVE_TYPE_CHAR32,
	PRIMITIVE_TYPE_SHORT,
	PRIMITIVE_TYPE_UNSIGNEDSHORT,
	PRIMITIVE_TYPE_INT,
	PRIMITIVE_TYPE_UNSIGNEDINT,
	PRIMITIVE_TYPE_LONG,
	PRIMITIVE_TYPE_UNSIGNEDLONG,
	PRIMITIVE_TYPE_LONGLONG,
	PRIMITIVE_TYPE_UNSIGNEDLONGLONG,
	PRIMITIVE_TYPE_INT128,
	PRIMITIVE_TYPE_UNSIGNEDINT128,
	PRIMITIVE_TYPE_BOOL,
	PRIMITIVE_TYPE_HALF,
	PRIMITIVE_TYPE_FLOAT,
	PRIMITIVE_TYPE_DOUBLE,
	PRIMITIVE_TYPE_LONGDOUBLE,
	PRIMITIVE_TYPE_FLOATCOMPLEX,
	PRIMITIVE_TYPE_DOUBLECOMPLEX,
	PRIMITIVE_TYPE_LONGDOUBLECOMPLEX,
	PRIMITIVE_TYPE_OBJCID,
	PRIMITIVE_TYPE_OBJCCLASS,
	PRIMITIVE_TYPE_OBJCSEL,
	PRIMITIVE_TYPE_NULLPTR,
	PRIMITIVE_TYPE_OTHER
} PrimitiveTypeIdentifier;

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

typedef struct PrimitiveType {
	Type type;
	PrimitiveTypeIdentifier id;
} PrimitiveType;

G_BEGIN_DECLS

void type_unref( Type * );
void type_ref( Type * );

void struct_member_ref( StructMember * );
void struct_member_unref( StructMember * );

PrimitiveType *type_as_primitive( Type *, GError ** );
StructType *type_as_struct( Type *, GError ** );
Type *type_as_canonical( Type *, GError ** );

Type *type_pointee( Type *, GError ** );
StructMember *struct_type_field_at_index( StructType *, size_t, GError ** );

G_END_DECLS

#endif

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
typedef enum RTypeIdentifier {
	R_TYPE_CLASS_INVALID           = (0u),
	R_TYPE_CLASS_ARRAY             = (1u << 0),
	R_TYPE_CLASS_BLOCKPOINTER      = (1u << 1),
	R_TYPE_CLASS_PRIMITIVE         = (1u << 2),
	R_TYPE_CLASS_CLASS             = (1u << 3),
	R_TYPE_CLASS_COMPLEXFLOAT      = (1u << 4),
	R_TYPE_CLASS_COMPLEXINTEGER    = (1u << 5),
	R_TYPE_CLASS_ENUMERATION       = (1u << 6),
	R_TYPE_CLASS_FUNCTION          = (1u << 7),
	R_TYPE_CLASS_MEMBERPOINTER     = (1u << 8),
	R_TYPE_CLASS_OBJCOBJECT        = (1u << 9),
	R_TYPE_CLASS_OBJCINTERFACE     = (1u << 10),
	R_TYPE_CLASS_OBJCOBJECTPOINTER = (1u << 11),
	R_TYPE_CLASS_POINTER           = (1u << 12),
	R_TYPE_CLASS_REFERENCE         = (1u << 13),
	R_TYPE_CLASS_STRUCT            = (1u << 14),
	R_TYPE_CLASS_TYPEDEF           = (1u << 15),
	R_TYPE_CLASS_UNION             = (1u << 16),
	R_TYPE_CLASS_VECTOR            = (1u << 17),
	// Define the last type class as the MSBit of a 32 bit value
	R_TYPE_CLASS_OTHER             = (1u << 31),
	// Define a mask that can be used for any type when finding types
	R_TYPE_CLASS_ANY               = (0xffffffffu)
} RTypeIdentifier;

typedef enum RPrimitiveTypeIdentifier {
	R_PRIMITIVE_TYPE_INVALID = 0,
	R_PRIMITIVE_TYPE_VOID = 1,
	R_PRIMITIVE_TYPE_CHAR,
	R_PRIMITIVE_TYPE_SIGNEDCHAR,
	R_PRIMITIVE_TYPE_UNSIGNEDCHAR,
	R_PRIMITIVE_TYPE_WCHAR,
	R_PRIMITIVE_TYPE_SIGNEDWCHAR,
	R_PRIMITIVE_TYPE_UNSIGNEDWCHAR,
	R_PRIMITIVE_TYPE_CHAR16,
	R_PRIMITIVE_TYPE_CHAR32,
	R_PRIMITIVE_TYPE_SHORT,
	R_PRIMITIVE_TYPE_UNSIGNEDSHORT,
	R_PRIMITIVE_TYPE_INT,
	R_PRIMITIVE_TYPE_UNSIGNEDINT,
	R_PRIMITIVE_TYPE_LONG,
	R_PRIMITIVE_TYPE_UNSIGNEDLONG,
	R_PRIMITIVE_TYPE_LONGLONG,
	R_PRIMITIVE_TYPE_UNSIGNEDLONGLONG,
	R_PRIMITIVE_TYPE_INT128,
	R_PRIMITIVE_TYPE_UNSIGNEDINT128,
	R_PRIMITIVE_TYPE_BOOL,
	R_PRIMITIVE_TYPE_HALF,
	R_PRIMITIVE_TYPE_FLOAT,
	R_PRIMITIVE_TYPE_DOUBLE,
	R_PRIMITIVE_TYPE_LONGDOUBLE,
	R_PRIMITIVE_TYPE_FLOATCOMPLEX,
	R_PRIMITIVE_TYPE_DOUBLECOMPLEX,
	R_PRIMITIVE_TYPE_LONGDOUBLECOMPLEX,
	R_PRIMITIVE_TYPE_OBJCID,
	R_PRIMITIVE_TYPE_OBJCCLASS,
	R_PRIMITIVE_TYPE_OBJCSEL,
	R_PRIMITIVE_TYPE_NULLPTR,
	R_PRIMITIVE_TYPE_OTHER
} RPrimitiveTypeIdentifier;

#ifndef _RUMINATE_TYPE_CPP_
typedef void RTypePrivate;
typedef void RStructTypePrivate;
typedef void RStructMemberPrivate;
typedef void RFunctionTypePrivate;
#endif

typedef struct RType {
	const char *name;
	size_t size;
	RTypeIdentifier id;
	RTypePrivate *priv;
} RType;

typedef struct RStructType {
	RType type;
	size_t nfields;
	RStructTypePrivate *priv;
} RStructType;

typedef struct RStructMember {
	RType *type;
	const char *name;
	off_t offset;
	RStructMemberPrivate *priv;
} RStructMember;

typedef struct RPrimitiveType {
	RType type;
	RPrimitiveTypeIdentifier id;
} RPrimitiveType;

typedef struct RFunctionType {
	RType type;
	size_t narguments;
	RFunctionTypePrivate *priv;
} RFunctionType;

G_BEGIN_DECLS

void r_type_unref( RType * );
void r_type_ref( RType * );

void r_struct_member_ref( RStructMember * );
void r_struct_member_unref( RStructMember * );

RPrimitiveType *r_type_as_primitive( RType *, GError ** );
RStructType *r_type_as_struct( RType *, GError ** );
RType *r_type_as_canonical( RType *, GError ** );
RFunctionType *r_type_as_function( RType *, GError ** );

RType *r_type_pointee( RType *, GError ** );

RStructMember *r_struct_type_field_at_index( RStructType *, size_t, GError ** );
RType *r_function_argument_type_at_index( RFunctionType *, size_t, GError ** );
RType *r_function_return_type( RFunctionType *, GError ** );

G_END_DECLS

#endif

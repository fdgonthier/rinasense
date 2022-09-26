/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6 */

#ifndef PB_RINA_MESSAGES_CDAP_PB_H_INCLUDED
#define PB_RINA_MESSAGES_CDAP_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
/* Message types. */
typedef enum _rina_messages_opCode_t { 
    rina_messages_opCode_t_M_CONNECT = 0, 
    rina_messages_opCode_t_M_CONNECT_R = 1, 
    rina_messages_opCode_t_M_RELEASE = 2, 
    rina_messages_opCode_t_M_RELEASE_R = 3, 
    rina_messages_opCode_t_M_CREATE = 4, 
    rina_messages_opCode_t_M_CREATE_R = 5, 
    rina_messages_opCode_t_M_DELETE = 6, 
    rina_messages_opCode_t_M_DELETE_R = 7, 
    rina_messages_opCode_t_M_READ = 8, 
    rina_messages_opCode_t_M_READ_R = 9, 
    rina_messages_opCode_t_M_CANCELREAD = 10, 
    rina_messages_opCode_t_M_CANCELREAD_R = 11, 
    rina_messages_opCode_t_M_WRITE = 12, 
    rina_messages_opCode_t_M_WRITE_R = 13, 
    rina_messages_opCode_t_M_START = 14, 
    rina_messages_opCode_t_M_START_R = 15, 
    rina_messages_opCode_t_M_STOP = 16, 
    rina_messages_opCode_t_M_STOP_R = 17 
} rina_messages_opCode_t;

/* Values for the flags field. */
typedef enum _rina_messages_flagValues_t { 
    rina_messages_flagValues_t_F_NO_FLAGS = 0, /* The default value, no flags are set */
    rina_messages_flagValues_t_F_SYNC = 1, /* set on READ/WRITE to request synchronous r/w */
    rina_messages_flagValues_t_F_RD_INCOMPLETE = 2 /* set on all but final reply to an M_READ */
} rina_messages_flagValues_t;

/* Struct definitions */
typedef struct _rina_messages_string_t { /* information to identify an string */
    pb_callback_t value; /* value of the string */
} rina_messages_string_t;

typedef struct _rina_messages_authPolicy_t { 
    bool has_name;
    char name[40]; /* Policy name */
    pb_size_t versions_count;
    char versions[1][40]; /* Supported versions */
    pb_callback_t options; /* Opaque policy-specific versions */
} rina_messages_authPolicy_t;

typedef struct _rina_messages_int_t { /* information to identify an int */
    uint32_t value; /* value of the integer */
} rina_messages_int_t;

typedef PB_BYTES_ARRAY_T(512) rina_messages_objVal_t_byteval_t;
typedef struct _rina_messages_objVal_t { /* value of an object */
    bool has_intval;
    int32_t intval;
    bool has_sintval;
    int32_t sintval;
    bool has_int64val;
    int64_t int64val;
    bool has_sint64val;
    int64_t sint64val;
    bool has_strval;
    char strval[20];
    bool has_byteval;
    rina_messages_objVal_t_byteval_t byteval; /* arbitrary structure or message */
    bool has_floatval;
    uint32_t floatval;
    bool has_doubleval;
    uint64_t doubleval;
    bool has_boolval;
    bool boolval;
} rina_messages_objVal_t;

/* CDAP message field definition, can be used for all messages.
 In this single-message-buffer-type form, if a field is optional in any
 message, it must be "optional" here.  If required in all, it is "mandatory" here.
 See the documentation for the complete field specification of each message type. */
typedef struct _rina_messages_CDAPMessage { 
    bool has_absSyntax;
    int32_t absSyntax; /* Abstract Syntax of messages, see text. */
    rina_messages_opCode_t opCode; /* op Code. */
    bool has_invokeID;
    int32_t invokeID; /* Invoke ID, omitted if no reply desired. */
    bool has_flags;
    rina_messages_flagValues_t flags; /* misc. flags */
    bool has_objClass;
    char objClass[40]; /* Name of the object class of objName */
    bool has_objName;
    char objName[40]; /* Object name, unique in its class */
    bool has_objInst;
    int64_t objInst; /* Unique object instance */
    bool has_objValue;
    rina_messages_objVal_t objValue; /* value of object in read/write/etc. */
    bool has_result;
    int32_t result; /* result of operation, 0 == success */
    bool has_scope;
    int32_t scope; /* scope of READ/WRITE operation */
    pb_callback_t filter; /* filter script */
    bool has_authPolicy;
    rina_messages_authPolicy_t authPolicy; /* Authentication policy information */
    bool has_destAEInst;
    char destAEInst[20]; /* Destination AE Instance name */
    bool has_destAEName;
    char destAEName[20]; /* Destination AE name */
    bool has_destApInst;
    char destApInst[20]; /* Destination Application Instance name */
    bool has_destApName;
    char destApName[20]; /* Destination Application name */
    bool has_srcAEInst;
    char srcAEInst[20]; /* Source AE Instance name */
    bool has_srcAEName;
    char srcAEName[20]; /* Source AE name */
    bool has_srcApInst;
    char srcApInst[20]; /* Source Application Instance name */
    bool has_srcApName;
    char srcApName[20]; /* Source Application name */
    bool has_resultReason;
    char resultReason[20]; /* further explanation of result */
    bool has_version;
    int64_t version; /* For application use - RIB/class version. */
} rina_messages_CDAPMessage;


/* Helper constants for enums */
#define _rina_messages_opCode_t_MIN rina_messages_opCode_t_M_CONNECT
#define _rina_messages_opCode_t_MAX rina_messages_opCode_t_M_STOP_R
#define _rina_messages_opCode_t_ARRAYSIZE ((rina_messages_opCode_t)(rina_messages_opCode_t_M_STOP_R+1))

#define _rina_messages_flagValues_t_MIN rina_messages_flagValues_t_F_NO_FLAGS
#define _rina_messages_flagValues_t_MAX rina_messages_flagValues_t_F_RD_INCOMPLETE
#define _rina_messages_flagValues_t_ARRAYSIZE ((rina_messages_flagValues_t)(rina_messages_flagValues_t_F_RD_INCOMPLETE+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define rina_messages_objVal_t_init_default      {false, 0, false, 0, false, 0, false, 0, false, "", false, {0, {0}}, false, 0, false, 0, false, 0}
#define rina_messages_authPolicy_t_init_default  {false, "", 0, {""}, {{NULL}, NULL}}
#define rina_messages_CDAPMessage_init_default   {false, 0, _rina_messages_opCode_t_MIN, false, 0, false, _rina_messages_flagValues_t_MIN, false, "", false, "", false, 0, false, rina_messages_objVal_t_init_default, false, 0, false, 0, {{NULL}, NULL}, false, rina_messages_authPolicy_t_init_default, false, "", false, "", false, "", false, "", false, "", false, "", false, "", false, "", false, "", false, 0}
#define rina_messages_int_t_init_default         {0}
#define rina_messages_string_t_init_default      {{{NULL}, NULL}}
#define rina_messages_objVal_t_init_zero         {false, 0, false, 0, false, 0, false, 0, false, "", false, {0, {0}}, false, 0, false, 0, false, 0}
#define rina_messages_authPolicy_t_init_zero     {false, "", 0, {""}, {{NULL}, NULL}}
#define rina_messages_CDAPMessage_init_zero      {false, 0, _rina_messages_opCode_t_MIN, false, 0, false, _rina_messages_flagValues_t_MIN, false, "", false, "", false, 0, false, rina_messages_objVal_t_init_zero, false, 0, false, 0, {{NULL}, NULL}, false, rina_messages_authPolicy_t_init_zero, false, "", false, "", false, "", false, "", false, "", false, "", false, "", false, "", false, "", false, 0}
#define rina_messages_int_t_init_zero            {0}
#define rina_messages_string_t_init_zero         {{{NULL}, NULL}}

/* Field tags (for use in manual encoding/decoding) */
#define rina_messages_string_t_value_tag         1
#define rina_messages_authPolicy_t_name_tag      1
#define rina_messages_authPolicy_t_versions_tag  2
#define rina_messages_authPolicy_t_options_tag   3
#define rina_messages_int_t_value_tag            1
#define rina_messages_objVal_t_intval_tag        1
#define rina_messages_objVal_t_sintval_tag       2
#define rina_messages_objVal_t_int64val_tag      3
#define rina_messages_objVal_t_sint64val_tag     4
#define rina_messages_objVal_t_strval_tag        5
#define rina_messages_objVal_t_byteval_tag       6
#define rina_messages_objVal_t_floatval_tag      7
#define rina_messages_objVal_t_doubleval_tag     8
#define rina_messages_objVal_t_boolval_tag       9
#define rina_messages_CDAPMessage_absSyntax_tag  1
#define rina_messages_CDAPMessage_opCode_tag     2
#define rina_messages_CDAPMessage_invokeID_tag   3
#define rina_messages_CDAPMessage_flags_tag      4
#define rina_messages_CDAPMessage_objClass_tag   5
#define rina_messages_CDAPMessage_objName_tag    6
#define rina_messages_CDAPMessage_objInst_tag    7
#define rina_messages_CDAPMessage_objValue_tag   8
#define rina_messages_CDAPMessage_result_tag     9
#define rina_messages_CDAPMessage_scope_tag      10
#define rina_messages_CDAPMessage_filter_tag     11
#define rina_messages_CDAPMessage_authPolicy_tag 18
#define rina_messages_CDAPMessage_destAEInst_tag 19
#define rina_messages_CDAPMessage_destAEName_tag 20
#define rina_messages_CDAPMessage_destApInst_tag 21
#define rina_messages_CDAPMessage_destApName_tag 22
#define rina_messages_CDAPMessage_srcAEInst_tag  23
#define rina_messages_CDAPMessage_srcAEName_tag  24
#define rina_messages_CDAPMessage_srcApInst_tag  25
#define rina_messages_CDAPMessage_srcApName_tag  26
#define rina_messages_CDAPMessage_resultReason_tag 27
#define rina_messages_CDAPMessage_version_tag    28

/* Struct field encoding specification for nanopb */
#define rina_messages_objVal_t_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, INT32,    intval,            1) \
X(a, STATIC,   OPTIONAL, SINT32,   sintval,           2) \
X(a, STATIC,   OPTIONAL, INT64,    int64val,          3) \
X(a, STATIC,   OPTIONAL, SINT64,   sint64val,         4) \
X(a, STATIC,   OPTIONAL, STRING,   strval,            5) \
X(a, STATIC,   OPTIONAL, BYTES,    byteval,           6) \
X(a, STATIC,   OPTIONAL, FIXED32,  floatval,          7) \
X(a, STATIC,   OPTIONAL, FIXED64,  doubleval,         8) \
X(a, STATIC,   OPTIONAL, BOOL,     boolval,           9)
#define rina_messages_objVal_t_CALLBACK NULL
#define rina_messages_objVal_t_DEFAULT NULL

#define rina_messages_authPolicy_t_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, STRING,   name,              1) \
X(a, STATIC,   REPEATED, STRING,   versions,          2) \
X(a, CALLBACK, OPTIONAL, BYTES,    options,           3)
#define rina_messages_authPolicy_t_CALLBACK pb_default_field_callback
#define rina_messages_authPolicy_t_DEFAULT NULL

#define rina_messages_CDAPMessage_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, INT32,    absSyntax,         1) \
X(a, STATIC,   REQUIRED, UENUM,    opCode,            2) \
X(a, STATIC,   OPTIONAL, INT32,    invokeID,          3) \
X(a, STATIC,   OPTIONAL, UENUM,    flags,             4) \
X(a, STATIC,   OPTIONAL, STRING,   objClass,          5) \
X(a, STATIC,   OPTIONAL, STRING,   objName,           6) \
X(a, STATIC,   OPTIONAL, INT64,    objInst,           7) \
X(a, STATIC,   OPTIONAL, MESSAGE,  objValue,          8) \
X(a, STATIC,   OPTIONAL, INT32,    result,            9) \
X(a, STATIC,   OPTIONAL, INT32,    scope,            10) \
X(a, CALLBACK, OPTIONAL, BYTES,    filter,           11) \
X(a, STATIC,   OPTIONAL, MESSAGE,  authPolicy,       18) \
X(a, STATIC,   OPTIONAL, STRING,   destAEInst,       19) \
X(a, STATIC,   OPTIONAL, STRING,   destAEName,       20) \
X(a, STATIC,   OPTIONAL, STRING,   destApInst,       21) \
X(a, STATIC,   OPTIONAL, STRING,   destApName,       22) \
X(a, STATIC,   OPTIONAL, STRING,   srcAEInst,        23) \
X(a, STATIC,   OPTIONAL, STRING,   srcAEName,        24) \
X(a, STATIC,   OPTIONAL, STRING,   srcApInst,        25) \
X(a, STATIC,   OPTIONAL, STRING,   srcApName,        26) \
X(a, STATIC,   OPTIONAL, STRING,   resultReason,     27) \
X(a, STATIC,   OPTIONAL, INT64,    version,          28)
#define rina_messages_CDAPMessage_CALLBACK pb_default_field_callback
#define rina_messages_CDAPMessage_DEFAULT (const pb_byte_t*)"\x48\x00\x00"
#define rina_messages_CDAPMessage_objValue_MSGTYPE rina_messages_objVal_t
#define rina_messages_CDAPMessage_authPolicy_MSGTYPE rina_messages_authPolicy_t

#define rina_messages_int_t_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT32,   value,             1)
#define rina_messages_int_t_CALLBACK NULL
#define rina_messages_int_t_DEFAULT NULL

#define rina_messages_string_t_FIELDLIST(X, a) \
X(a, CALLBACK, REQUIRED, STRING,   value,             1)
#define rina_messages_string_t_CALLBACK pb_default_field_callback
#define rina_messages_string_t_DEFAULT NULL

extern const pb_msgdesc_t rina_messages_objVal_t_msg;
extern const pb_msgdesc_t rina_messages_authPolicy_t_msg;
extern const pb_msgdesc_t rina_messages_CDAPMessage_msg;
extern const pb_msgdesc_t rina_messages_int_t_msg;
extern const pb_msgdesc_t rina_messages_string_t_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define rina_messages_objVal_t_fields &rina_messages_objVal_t_msg
#define rina_messages_authPolicy_t_fields &rina_messages_authPolicy_t_msg
#define rina_messages_CDAPMessage_fields &rina_messages_CDAPMessage_msg
#define rina_messages_int_t_fields &rina_messages_int_t_msg
#define rina_messages_string_t_fields &rina_messages_string_t_msg

/* Maximum encoded size of messages (where known) */
/* rina_messages_authPolicy_t_size depends on runtime parameters */
/* rina_messages_CDAPMessage_size depends on runtime parameters */
/* rina_messages_string_t_size depends on runtime parameters */
#define rina_messages_int_t_size                 6
#define rina_messages_objVal_t_size              591

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

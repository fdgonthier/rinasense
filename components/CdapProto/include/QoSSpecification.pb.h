/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6 */

#ifndef PB_RINA_MESSAGES_QOSSPECIFICATION_PB_H_INCLUDED
#define PB_RINA_MESSAGES_QOSSPECIFICATION_PB_H_INCLUDED
#include <pb.h>
#include "CommonMessages.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _rina_messages_qosSpecification_t { /* The QoS parameters requested by an application for a certain flow */
    bool has_name;
    char name[20]; /* The name of the QoS cube, if known */
    bool has_qosid;
    uint32_t qosid; /* The if of the QoS cube, if known (-1 = not known) */
    bool has_averageBandwidth;
    uint64_t averageBandwidth; /* in bytes/s, a value of 0 indicates 'don't care' */
    bool has_averageSDUBandwidth;
    uint64_t averageSDUBandwidth; /* in bytes/s, a value of 0 indicates 'don't care' */
    bool has_peakBandwidthDuration;
    uint32_t peakBandwidthDuration; /* in ms, a value of 0 indicates 'don't care' */
    bool has_peakSDUBandwidthDuration;
    uint32_t peakSDUBandwidthDuration; /* in ms, a value of 0 indicates 'don't care' */
    bool has_undetectedBitErrorRate;
    double undetectedBitErrorRate; /* a value of 0 indicates 'don`t care' */
    bool has_partialDelivery;
    bool partialDelivery; /* indicates if partial delivery of SDUs is allowed or not */
    bool has_order;
    bool order; /* indicates if SDUs have to be delivered in order */
    bool has_maxAllowableGapSdu;
    int32_t maxAllowableGapSdu; /* indicates the maximum gap allowed in SDUs, a gap of N SDUs is considered the same as all SDUs delivered. A value of -1 indicates 'Any' */
    bool has_delay;
    uint32_t delay; /* in milliseconds, indicates the maximum delay allowed in this flow. A value of 0 indicates don't care */
    bool has_jitter;
    uint32_t jitter; /* in milliseconds, indicates indicates the maximum jitter allowed in this flow. A value of 0 indicates don't care */
    pb_callback_t extraParameters; /* the extra parameters specified by the application process that requested this flow */
    bool has_msg_boundaries;
    bool msg_boundaries; /* preserve message boundaries if true, don't do it otherwise */
    bool has_loss;
    uint32_t loss; /* the max loss probability (1/10000) for this connection */
} rina_messages_qosSpecification_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define rina_messages_qosSpecification_t_init_default {false, "", false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, {{NULL}, NULL}, false, 0, false, 0}
#define rina_messages_qosSpecification_t_init_zero {false, "", false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, {{NULL}, NULL}, false, 0, false, 0}

/* Field tags (for use in manual encoding/decoding) */
#define rina_messages_qosSpecification_t_name_tag 1
#define rina_messages_qosSpecification_t_qosid_tag 2
#define rina_messages_qosSpecification_t_averageBandwidth_tag 3
#define rina_messages_qosSpecification_t_averageSDUBandwidth_tag 4
#define rina_messages_qosSpecification_t_peakBandwidthDuration_tag 5
#define rina_messages_qosSpecification_t_peakSDUBandwidthDuration_tag 6
#define rina_messages_qosSpecification_t_undetectedBitErrorRate_tag 7
#define rina_messages_qosSpecification_t_partialDelivery_tag 8
#define rina_messages_qosSpecification_t_order_tag 9
#define rina_messages_qosSpecification_t_maxAllowableGapSdu_tag 10
#define rina_messages_qosSpecification_t_delay_tag 11
#define rina_messages_qosSpecification_t_jitter_tag 12
#define rina_messages_qosSpecification_t_extraParameters_tag 13
#define rina_messages_qosSpecification_t_msg_boundaries_tag 14
#define rina_messages_qosSpecification_t_loss_tag 15

/* Struct field encoding specification for nanopb */
#define rina_messages_qosSpecification_t_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, STRING,   name,              1) \
X(a, STATIC,   OPTIONAL, UINT32,   qosid,             2) \
X(a, STATIC,   OPTIONAL, UINT64,   averageBandwidth,   3) \
X(a, STATIC,   OPTIONAL, UINT64,   averageSDUBandwidth,   4) \
X(a, STATIC,   OPTIONAL, UINT32,   peakBandwidthDuration,   5) \
X(a, STATIC,   OPTIONAL, UINT32,   peakSDUBandwidthDuration,   6) \
X(a, STATIC,   OPTIONAL, DOUBLE,   undetectedBitErrorRate,   7) \
X(a, STATIC,   OPTIONAL, BOOL,     partialDelivery,   8) \
X(a, STATIC,   OPTIONAL, BOOL,     order,             9) \
X(a, STATIC,   OPTIONAL, INT32,    maxAllowableGapSdu,  10) \
X(a, STATIC,   OPTIONAL, UINT32,   delay,            11) \
X(a, STATIC,   OPTIONAL, UINT32,   jitter,           12) \
X(a, CALLBACK, REPEATED, MESSAGE,  extraParameters,  13) \
X(a, STATIC,   OPTIONAL, BOOL,     msg_boundaries,   14) \
X(a, STATIC,   OPTIONAL, UINT32,   loss,             15)
#define rina_messages_qosSpecification_t_CALLBACK pb_default_field_callback
#define rina_messages_qosSpecification_t_DEFAULT NULL
#define rina_messages_qosSpecification_t_extraParameters_MSGTYPE rina_messages_property_t

extern const pb_msgdesc_t rina_messages_qosSpecification_t_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define rina_messages_qosSpecification_t_fields &rina_messages_qosSpecification_t_msg

/* Maximum encoded size of messages (where known) */
/* rina_messages_qosSpecification_t_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

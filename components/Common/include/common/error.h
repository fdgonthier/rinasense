#ifndef _COMMON_ERROR_H_INCLUDED
#define _COMMON_ERROR_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include "portability/port.h"

#include "error_defs.h"
#include "rinasense_errors.h"

typedef struct xERR_INFO {
    /* Somewhat arbitrary size for the filename */
    stringbuf_t pcFile[0xFF];

    /* Line number for the error */
    uint32_t unLine;

    /* Error code attached to this error info bloack */
    rsErr_t unErrCode;

    /* Human-readable error message attached to the error. */
    stringbuf_t pcErrMsg[50];

    /* Pointer to the next error info structure in the chain. */
    struct xERR_INFO *pxNext;

} rsErrInfo_t;

#define _MK_CAT(cat) (cat << 24)

#define _MK_ERR(cat, err) ((rsErr_t){ .c = cat | err })

#define ERR_CAT(x) (x.c >> 24)

#define ERR_CODE(x) (x.c & 0x000000FF)

/* Clear all the error messages in the current thread. */
void vErrorClear();

/* Clear all the error messages that may be attached to the current
 * thread, replacing with a new one. */
rsErr_t xErrorSet(string_t pcFile, uint32_t unLine, rsErr_t errNo, string_t pcMsg);

rsErr_t xErrorSetf(string_t pcFile, uint32_t unLine, rsErr_t errNo, const string_t pcFmt, ...);

rsErr_t xErrorSetErrno(string_t pcFile, uint32_t unLine, int _errno);

/* Add to the error currently saved for the current thread, attaching
 * the previously saved error to the new one. */
rsErr_t xErrorPush(string_t pcFile, uint32_t unLine, rsErr_t errNo, string_t pcMsg);

void vErrorLog(const string_t pcTag, const string_t pcTitle);

rsErrInfo_t *xErrorGet();

/*  */
string_t xErrorGetMsg();

static inline bool_t _err_chk_mem(rsErr_t xErr)
{
    if (!xErr.c) return false;
    if ((xErr.c & ERR_OOM.c) == ERR_OOM.c)
        return true;
    else {
        fprintf(stderr, "ERR_CHK_MEM unexpected error: 0x%08x!\n", xErr.c);
        abort();
    }
}

/* Returns true if a function returning 'rsMemErr_t' returns a
 * memory error. */
#define ERR_CHK_MEM(x)                          \
    _err_chk_mem(x)

/* Returns true in case of error */
#define ERR_CHK(x) \
    (x.c)

#define ERR_IS(x, t) \
    (ERR_CHK(x) && xErrorGet() != NULL && xErrorGet()->unErrCode.c == t.c)

/* Fail an assertion in case an allocation fails. Use this in
 * situations where it would be impossible or difficult to recover,
 * like during initialization process. */
#define ERR_MEM_ASSERT(x) \
    RsAssert(!((x) & ERR_ENOMEM))

#define SUCCESS (rsErr_t){ .c = 0 }

/* Generic failure, use this when you know an error is already set */
#define FAIL ERR_ERR

/* Set any error but evalutes to NULL */
#define ERR_SET(x)                              \
    (!x.c ? SUCCESS : xErrorSet(__FILE__, __LINE__, x, xErrorGetMsg(x)))

#define ERR_SETF(x, ...)                                            \
    (!x.c ? SUCCESS : xErrorSetf(__FILE__, __LINE__, x, xErrorGetMsg(x), __VA_ARGS__))

#define ERR_SET_MSG(x, m)                       \
    (!x.c ? SUCCESS : xErrorSet(__FILE__, __LINE__, x, m))

#define ERR_SET_MSGF(x, msg, ...)                   \
    (!x.c ? SUCCESS : xErrorSetf(__FILE__, __LINE__, x, __VA_ARGS__))

#define ERR_SET_MSGF_NULL(x, msg, ...)                              \
    (!x.c ? NULL : (ERR_CHK(xErrorSetf(__FILE__, __LINE__, x, NULL)) ? NULL : NULL))

/* Set any error but evaluates to NULL */
#define ERR_SET_NULL(x)                                             \
    (!x.c ? NULL : (ERR_CHK(xErrorSet(__FILE__, __LINE__, x, xErrorGetMsg(x))) ? NULL : NULL))

/* Set an out-of-memory error */
#define ERR_SET_OOM                                         \
    xErrorSet(__FILE__, __LINE__, ERR_OOM, "Out of memory")

/* Set and out-of-memory error but evaluates to NULL */
#define ERR_SET_OOM_NULL                                                \
    ERR_CHK(xErrorSet(__FILE__, __LINE__, ERR_OOM, "Out of memory")) ? NULL : NULL;

#define ERR_SET_ERRNO                           \
    xErrorSetErrno(__FILE__, __LINE__, errno);

/* Pthread returns an error code directly */
#define ERR_SET_PTHREAD(x) \
    ERR_SET(_MK_ERR(ERR_PTHREAD.c, (x << 16)))

#endif /* _COMMON_ERROR_H_INCLUDED */

#ifndef COMMON_RINA_NAME_H
#define COMMON_RINA_NAME_H

#include "defs.h"
#include "portability/port.h"

typedef struct xName_info
{
	string_t pcProcessName;  		/*> Process Name*/
	string_t pcProcessInstance;		/*> Process Instance*/
	string_t pcEntityName;			/*> Entity Name*/
	string_t pcEntityInstance;		/*> Entity Instance*/

} name_t;

name_t *pxRstrNameDup(const name_t *pxSrc);

void vRstrNameDestroy(name_t *pxName);

bool_t xRstringDup(const string_t *pxSrc, string_t **pxDst);

name_t * xRinaNameCreate( void );

bool_t xRinaNameFromString(const string_t pcString, name_t * xName);

void xRinaNameFree(name_t * xName);

bool_t xRINAStringDup(const string_t * pcSrc, string_t ** pcDst);

name_t *xRINAstringToName(const string_t *pxInput);

#endif // COMMON_RINA_NAME_H

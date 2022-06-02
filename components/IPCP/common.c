#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "common.h"
#include "rina_name.h"

/*name_t *xRinaNameCreate(void);

char *xRINAstrdup(const char *s);

name_t *xRinaNameCreate(void)
{
        name_t *result;

        result = pvPortMalloc(sizeof(*result));
        if (!result)
                return NULL;
        memset(result, 0, sizeof(*result));

        return result;
}
*/

void memcheck(void){
	// perform  free memory check
	int blockSize = 16;
	int i = 1;
        static int size = 0;
	printf("Checking memory with blocksize %d char ...\n", blockSize);
	while (true) {
		char *p = (char *) malloc(i * blockSize);
		if (p == NULL){
			break;
		}
		free(p);
		++i;
	}

	return dst;
}

name_t *xRINAstringToName(const string_t *pxInput)
{
	name_t *pxName;

	string_t *tmp1 = NULL;
	string_t *tmp_pn = NULL;
	string_t *tmp_pi = NULL;
	string_t *tmp_en = NULL;
	string_t *tmp_ei = NULL;

        ESP_LOGE(TAG_RINA, "pxInput: %s", *pxInput);

	if (pxInput)
	{
		string_t *tmp2;

		xRINAStringDup(pxInput, &tmp1);
		if (!tmp1)
		{
			return NULL;
		}
		tmp2 = tmp1;

		tmp_pn = strsep(&tmp2, DELIMITER);
		tmp_pi = strsep(&tmp2, DELIMITER);
		tmp_en = strsep(&tmp2, DELIMITER);
		tmp_ei = strsep(&tmp2, DELIMITER);
	}

        ESP_LOGE(TAG_RINA, "tmp_pn: %s", *tmp_pn);
	pxName = xRinaNameCreate( );
	if (!pxName)
	{
		if (tmp1)
			vPortFree(tmp1);
		return NULL;
	}

	if (!xRINANameInitFrom(pxName, tmp_pn, tmp_pi, tmp_en, tmp_ei))
	{
		xRinaNameFree(pxName);
		if (tmp1)
			vPortFree(tmp1);
		return NULL;
	}

	if (tmp1)
		vPortFree(tmp1);

	return pxName;
}
#endif
void memcheck(void)
{
        // perform free memory check
        int blockSize = 16;
        int i = 1;
        static int size = 0;
        printf("Checking memory with blocksize %d char ...\n", blockSize);
        while (true)
        {
                char *p = (char *)malloc(i * blockSize);
                if (p == NULL)
                {
                        break;
                }
                free(p);
                ++i;
        }
        printf("Ok for %d char\n", (i - 1) * blockSize);
        if (size != (i - 1) * blockSize)
                printf("There is a possible memory leak because the last memory size was %d and now is %d\n", size, (i - 1) * blockSize);
        size = (i - 1) * blockSize;
}

static int invoke_id = 1;
int get_next_invoke_id(void)
{
    return (invoke_id % INT_MAX == 0) ? (invoke_id = 1) : invoke_id++;
}



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "configRINA.h"
#include "IPCP.h"
#include "RINA_API.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define TAG_APP "[PING-PERF]"
#define PING_SIZE (32)
#define NUMBER_OF_PACKETS (1000)
#define INTERVAL (1)
#define DIF "mobile.DIF"
#define SERVER "sensor1"
#define CLIENT "ping"

void app_main(void)
{

    nvs_flash_init();
    /*
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);*/

    RINA_IPCPInit();

    portId_t xAppPortId;
    struct rinaFlowSpec_t *xFlowSpec = pvPortMalloc(sizeof(*xFlowSpec));
    uint8_t Flags = 1;

    size_t uxTxBytes = 0;
    int i = 0;

    int RTT[NUMBER_OF_PACKETS];
    int count = 0;

    int64_t time_start = 0,
            time_end = 0;

    int time_delta = 0;
    int time_init = 0;

    float pps, bps;
    double time_inter;

    char *data;

    size_t size_ping = 32;
    char bufferTx[size_ping];

    memset(bufferTx, 'x', size_ping); // 32bytes  (size_t)(PING_SIZE)

    vTaskDelay(2000);

    ESP_LOGD(TAG_APP, "----------- Requesting a Flow ----- ");

    xAppPortId = RINA_flow_alloc(DIF, CLIENT, SERVER, xFlowSpec, Flags);

    ESP_LOGD(TAG_APP, "Flow Port id: %d ", xAppPortId);

    if (xAppPortId != -1)
    {
        ESP_LOGI(TAG_APP, "-----------------------------------------------\n ");
        ESP_LOGI(TAG_APP, "Client connecting to %s, port %d \n: ", SERVER, xAppPortId);
        ESP_LOGI(TAG_APP, "-----------------------------------------------\n ");

        time_start = esp_timer_get_time();
        ESP_LOGI(TAG_APP, "  Interval          Transfer           Bandwidth");
        for (i = 0; i < 1; i++)
        {

            while (time_delta < INTERVAL * 1000000)
            {

                uxTxBytes = RINA_flow_write(xAppPortId, (void *)bufferTx, strlen(bufferTx));

                if (uxTxBytes == 0)
                {
                    ESP_LOGE(TAG_APP, "Error to send Data");
                    break;
                }

                time_end = esp_timer_get_time();
                time_delta = time_end - time_start;
                count++;
                vTaskDelay(1 / portTICK_RATE_MS);
            }

            ESP_LOGI(TAG_APP, "Time start = %lld", time_start);

            time_inter = (float)time_delta / 1000000;

            ESP_LOGI(TAG_APP, "Time inter = %f", time_inter);
            ESP_LOGI(TAG_APP, "Time delta= %d", time_delta);
            ESP_LOGI(TAG_APP, "# packets sended= %d", count);
            pps = count * strlen(bufferTx);
            bps = pps * 8 * 1000000 / time_delta;
            bps = bps / 1000000;

            ESP_LOGI(TAG_APP, "  %.1f-%.1f sec         %3.3f MBytes        %3.3f  Mbits/sec",
                     (float)time_init / 1000000,
                     ((float)time_delta + (float)time_init) / 1000000, pps / 10000000, bps);
            time_init = time_delta;
            time_start = time_end;
            time_delta = 0;
        }
    }

    // ESP_LOGI(TAG_APP, "  %f MBytes", pps / 1000000);
    // ESP_LOGI(TAG_APP, "  0.0-%f sec", time_inter);

    if (RINA_flow_close(xAppPortId))
    {
        ESP_LOGI(TAG_APP, "Flow Deallocated");
    }
    else
    {
        ESP_LOGI(TAG_APP, "It was not possible to deallocated the flow");
    }
}


/**
 * ESP-IDF
 */

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

/**
 * FreeRTOS
 */

#include "freertos/FreeRTOS.h"

/**
 * Custom Library
 */

#include "nvs_handler.h"

static const char *TAG_NVS = "NVS_Handler";

nvs_handle my_handle;

void nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
}
/*

*/

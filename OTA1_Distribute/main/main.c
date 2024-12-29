#include <stdio.h>
#include "sys_config.h"
#include "freertos/FreeRTOS.h"
#include "nvs_handler.h"
#include "mesh_handler.h"
#include "mesh_send.h"
#include "wifi_handler.h"
#include "tcp.h"

#include "driver/gpio.h"

#define LED_GPIO_PIN GPIO_NUM_2 // Thường là GPIO2 trên ESP32

extern uint8_t flag_get_done;

void led_on()
{
    gpio_reset_pin(LED_GPIO_PIN);                       // Reset chân GPIO về trạng thái mặc định
    gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT); // Đặt GPIO là đầu ra
    gpio_set_level(LED_GPIO_PIN, 1);                    // Đặt mức HIGH
}

void app_main(void)
{
    led_on();
    nvs_init();
    wifi_init_sta();
    xTaskCreate(tcp_getData, "tcp_getData", 1024 * 4, NULL, 5, NULL);
    while (!flag_get_done)
    {
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    mesh_app_start();
}

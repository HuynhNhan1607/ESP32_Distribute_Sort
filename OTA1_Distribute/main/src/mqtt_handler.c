/**
 * C Library
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
/**
 * ESP-IDF
 */

#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "mqtt_client.h"
#include "esp_ota_ops.h"
/**
 * Custom Library
 */

#include "mqtt_handler.h"
#include "mesh_send.h"
#include "sort.h"
static const char *TAG_MQTT = "MQTT_Handler";
static esp_mqtt_client_handle_t s_client = NULL;

extern char ip[50];

void ota_handle()
{
    static esp_ota_handle_t ota_handle;
    static const esp_partition_t *update_partition = NULL;
    update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    ESP_LOGW(TAG_MQTT, "----Swtich to Upgrade----");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    esp_ota_set_boot_partition(update_partition);
    esp_restart();
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = 0;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "Upgrade", 1);
        ESP_LOGI(TAG_MQTT, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG_MQTT, "TOPIC=%.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG_MQTT, "DATA=%.*s", event->data_len, event->data);
        if (strncmp(event->data, "Upgrade", event->data_len) == 0 && event->data_len == strlen("Upgrade"))
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            send_mesh_all_string("Upgrade");
            ota_handle();
        }
        else if (strncmp(event->data, "Run", event->data_len) == 0 && event->data_len == strlen("Run"))
        {
            Receive_Batches();
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG_MQTT, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://test.mosquitto.org:1883"};

    ESP_LOGI(TAG_MQTT, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

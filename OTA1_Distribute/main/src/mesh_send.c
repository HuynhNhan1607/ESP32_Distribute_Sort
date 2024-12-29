/**
 * C Library
 */

#include <stdint.h>
#include <string.h>
/**
 * ESP-IDF
 */

#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mac.h"
#include "cJSON.h"

/**
 * Custom Library
 */

#include "nvs_handler.h"
#include "sys_config.h"
#include "mesh_handler.h"
#include "mesh_send.h"
#include "mqtt_handler.h"
#include "tcp.h"

static const char *TAG_SEND = "MESH_SEND";
static uint8_t tx_buf[TX_SIZE] = {
    0,
};
static uint8_t rx_cmd_buf[RX_COMMAND_SIZE] = {
    0,
};
uint8_t rx_arr_buf[RX_ARRAY_SIZE] = {
    0,
};

extern int totalSize;

extern mesh_addr_t route_table[CONFIG_MESH_ROUTE_TABLE_SIZE];
int route_table_size;

extern char mesh_root_addr[20];

extern nvs_handle my_handle;
extern uint16_t *max_range_extern;

nodeEsp activeNode[30];
int lengthOfActiveNode = 0;

mesh_data_t g_data;

/* Root dùng để gửi Command cho Non Root */
void send_mesh_all_string(char *data_t)
{
    mesh_data_t mesh_send;
    mesh_send.data = tx_buf;
    mesh_send.size = TX_SIZE;
    mesh_send.proto = MESH_PROTO_BIN;
    snprintf((char *)tx_buf, TX_SIZE, data_t);
    if (strlen((char *)tx_buf) >= TX_SIZE)
    {
        ESP_LOGE(TAG_SEND, "Data too large for buffer");
        return;
    }
    mesh_send.size = strlen((char *)tx_buf) + 1;
    ESP_LOGI(TAG_SEND, "Data to send: %s", (char *)mesh_send.data);
    ESP_LOGI(TAG_SEND, "Data size: %d", mesh_send.size);

    esp_mesh_get_routing_table((mesh_addr_t *)&route_table,
                               CONFIG_MESH_ROUTE_TABLE_SIZE * 6, &route_table_size);
    printf("Routing Table Size: %d\n", route_table_size);
    for (int i = 1; i < route_table_size; i++)
    {
        ESP_LOGI(TAG_SEND, "Broadcast: Sending to [%d] " MACSTR, i, MAC2STR(route_table[i].addr));
        esp_err_t err = esp_mesh_send(&route_table[i], &mesh_send, MESH_DATA_P2P, NULL, 0);
        if (ESP_OK != err)
        {
            ESP_LOGE(TAG_SEND, "Send ALL with err code %d %s", err, esp_err_to_name(err));
        }
    }
}

void send_mesh_each_string(char *data_t, mesh_addr_t *addr)
{
    mesh_data_t mesh_send;
    mesh_send.data = tx_buf;
    mesh_send.size = TX_SIZE;
    mesh_send.proto = MESH_PROTO_BIN;
    snprintf((char *)tx_buf, TX_SIZE, data_t);
    mesh_send.size = strlen((char *)tx_buf) + 1;
    esp_err_t err = esp_mesh_send(addr, &mesh_send, MESH_DATA_P2P, NULL, 0);
    if (ESP_OK != err)
    {
        ESP_LOGE(TAG_SEND, "Send EACH with err code %d %s", err, esp_err_to_name(err));
    }
    ESP_LOGI(TAG_SEND, "Sent to " MACSTR, MAC2STR(addr->addr));
}
/* Dùng cho Non root */
void send_int_array_mesh(int *int_array, int array_size)
{
    mesh_data_t mesh_send;
    mesh_send.data = tx_buf;  // Sử dụng buffer đã định nghĩa trước đó
    mesh_send.size = TX_SIZE; // Kích thước tối đa của buffer

    // Chuyển đổi mảng int thành chuỗi để gửi
    int total_size = array_size * sizeof(int);
    if (total_size > TX_SIZE)
    {
        ESP_LOGE(TAG_SEND, "Array size exceeds buffer size.");
        return;
    }

    memcpy(mesh_send.data, int_array, total_size); // Sao chép dữ liệu vào buffer
    mesh_send.size = total_size;                   // Cập nhật kích thước dữ liệu

    esp_err_t err = esp_mesh_send(NULL, &mesh_send, MESH_DATA_P2P, NULL, 0);
    if (ESP_OK != err)
    {
        ESP_LOGE(TAG_SEND, "Send with err code %d %s", err, esp_err_to_name(err));
    }
}

void task_mesh_rx_command(void *pvParameter)
{
    mesh_addr_t from;

    mesh_data_t mesh_rev;
    mesh_rev.data = rx_cmd_buf;
    mesh_rev.size = RX_COMMAND_SIZE;

    int flag = 0;

    ESP_LOGI(TAG_SEND, "Rx_Command_Task Start");

    while (1)
    {
        mesh_rev.size = RX_COMMAND_SIZE;
        esp_err_t err = esp_mesh_recv(&from, &mesh_rev, portMAX_DELAY, &flag, NULL, 0);
        if (err != ESP_OK || mesh_rev.size == 0)
        {
            ESP_LOGE(TAG_SEND, "Failed to receive: %s, size: %d", esp_err_to_name(err), mesh_rev.size);
        }
        else
        {
            if (strcmp((char *)mesh_rev.data, "Upgrade") == 0)
            {
                printf("Strings match!\n");
                ota_handle();
            }
            else if (strcmp((char *)mesh_rev.data, "ADD") == 0)
            {
                send_to_root();
            }

            else
            {
                printf("Strings do not match.\n");
            }
        }
    }
}

void task_mesh_rx_array(void *pvParameter)
{
    mesh_addr_t from;

    mesh_data_t mesh_rev;
    mesh_rev.data = rx_arr_buf;
    mesh_rev.size = RX_ARRAY_SIZE;

    int flag = 0;

    ESP_LOGI(TAG_SEND, "Rx_ARRAY_Task Start");

    while (1)
    {
        mesh_rev.size = RX_ARRAY_SIZE;
        esp_err_t err = esp_mesh_recv(&from, &mesh_rev, portMAX_DELAY, &flag, NULL, 0);
        if (err != ESP_OK || mesh_rev.size == 0)
        {
            ESP_LOGE(TAG_SEND, "Failed to receive: %s, size: %d", esp_err_to_name(err), mesh_rev.size);
        }
        totalSize += mesh_rev.size / sizeof(int);
    }
}
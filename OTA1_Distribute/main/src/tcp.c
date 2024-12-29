#include "sys_config.h"
#include <sys/socket.h>
#include <netdb.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "mesh_handler.h"
#include "mesh_send.h"
#include "sort.h"
#include "esp_task_wdt.h"

#define ELEMENTS_TO_SEND 20

static const char *TAG_TCP = "TCP";

DRAM_ATTR int int_buffer[2560]; // Buffer để lưu trữ dữ liệu kiểu int
uint8_t flag_get_done = 0;
int preIndex = 0;
int output_length = 0;

void initTcpSocket(int *socket_fd)
{
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Create socket
    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_fd < 0)
    {
        ESP_LOGE(TAG_TCP, "Failed to create socket.");
        vTaskDelete(NULL);
        return;
    }
    if (connect(*socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
    {
        ESP_LOGE(TAG_TCP, "Failed to connect to server.");
        close(*socket_fd);
        return;
    }
    ESP_LOGI(TAG_TCP, "Connected to server");
}

void sendRequestToServer(int *socket_fd, const char *request)
{
    // Gửi yêu cầu đến server
    ssize_t bytes_sent = send(*socket_fd, request, strlen(request), 0);
    if (bytes_sent < 0)
    {
        ESP_LOGE(TAG_TCP, "Failed to send request to server.");
    }
    else
    {
        ESP_LOGI(TAG_TCP, "Sent request to server: %s", request);
    }
}

void tcp_getData()
{
    int received_bytes = 0;
    int socket_fd;

    initTcpSocket(&socket_fd);

    sendRequestToServer(&socket_fd, "REQUEST 10240"); // Yêu cầu 100 KB

    int total_received = 0;

    unsigned char temp_buffer[1024]; // Buffer tạm để nhận dữ liệu 1024 byte

    while (1)
    {                                                                          // 100 KB
        received_bytes = recv(socket_fd, temp_buffer, sizeof(temp_buffer), 0); // Nhận từng phần 1024 bytes
        if (received_bytes < 0)
        {
            ESP_LOGE(TAG_TCP, "Error receiving data.");
            close(socket_fd);
            break;
        }
        else if (received_bytes == 0)
        {
            ESP_LOGI(TAG_TCP, "Connection closed by server.");
            break;
        }
        printf(" %d \n", received_bytes);
        for (int i = 0; i < received_bytes; i += 4)
        {
            int number = (temp_buffer[i] |
                          (temp_buffer[i + 1] << 8) |
                          (temp_buffer[i + 2] << 16) |
                          (temp_buffer[i + 3] << 24));
            int_buffer[output_length] = number;
            output_length++;
        }
        total_received += received_bytes;
    }
    printf("Size received: %d -- Index: %d\n", total_received, output_length);
    // bubbleSort(int_buffer, total_received / 4); // Sắp xếp dữ liệu
    quickSort(int_buffer, output_length);
    //  In ra 10 phần tử đầu tiên hoặc ít hơn nếu không đủ
    for (int i = 0; i < 20; i++)
    {
        printf("%d ", int_buffer[i]);
    }
    printf("\n");
    esp_wifi_disconnect();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    flag_get_done = 1;
    close(socket_fd);
    ESP_LOGI(TAG_TCP, "Socket closed.");
    vTaskDelete(NULL);
}

void get_send_array(int *send_array, int *elements_to_send)
{
    if (preIndex < 0 || preIndex >= output_length)
    {
        ESP_LOGE(TAG_TCP, "Invalid preIndex: %d", preIndex);
        return;
    }

    *elements_to_send = (output_length - preIndex < ELEMENTS_TO_SEND) ? (output_length - preIndex) : ELEMENTS_TO_SEND;

    for (int i = 0; i < *elements_to_send; i++)
    {
        send_array[i] = int_buffer[preIndex + i];
    }
    preIndex += *elements_to_send;
    return;
}
void send_to_root()
{
    int elements_to_send;
    int send_array[ELEMENTS_TO_SEND];
    get_send_array(send_array, &elements_to_send);
    send_int_array_mesh(send_array, elements_to_send);
    ESP_LOGI(TAG_TCP, "Sent %d elements starting from index %d", elements_to_send, preIndex - elements_to_send);
}
#include "sort.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mesh_handler.h"
#include "mesh_send.h"
#include "esp_mesh.h"
#include "tcp.h"
#include "esp_log.h"
#include <limits.h>

static const char *TAG_SORT = "TAG_SORT";
extern uint8_t rx_arr_buf[RX_ARRAY_SIZE];
extern int route_table_size;
extern mesh_addr_t route_table[CONFIG_MESH_ROUTE_TABLE_SIZE];

int totalSize = 0;

// size = esp_mesh_get_routing_table
TournamentTree *initTournamentTree(int *initialValues, int size)
{
    TournamentTree *tree = (TournamentTree *)malloc(sizeof(TournamentTree));
    tree->tree = (int *)malloc(size * sizeof(int));
    tree->size = size;
    tree->missingLeaf = (int *)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++)
    {
        tree->tree[i] = initialValues[i];
        tree->missingLeaf[i] = i; // Theo dõi batch tương ứng
    }
    return tree;
}

int popRoot(TournamentTree *tree, int *batchIndexes, int **batches, int *batchSizes)
{
    int minValue = INT_MAX;
    int minIndex = -1;

    // Tìm giá trị nhỏ nhất từ Tournament Tree
    for (int i = 0; i < tree->size; i++)
    {
        if (tree->tree[i] < minValue)
        {
            minValue = tree->tree[i];
            minIndex = i;
        }
    }

    // Lấy giá trị tiếp theo từ batch tương ứng
    int batchIndex = batchIndexes[minIndex];
    if (batchIndex < batchSizes[minIndex] - 1)
    {
        batchIndexes[minIndex]++;
        tree->tree[minIndex] = batches[minIndex][batchIndexes[minIndex]];
    }
    else
    {
        tree->tree[minIndex] = INT_MAX; // Batch đã hết giá trị
    }

    return minValue;
}


int partition(int *arr, int low, int high)
{
    int pivot = arr[high]; // Chọn phần tử cuối làm pivot
    int i = low - 1;       // Chỉ số của phần tử nhỏ hơn pivot

    for (int j = low; j < high; j++)
    {
        // Nếu phần tử lớn hơn pivot (để sắp xếp giảm dần)
        if (arr[j] > pivot)
        {
            i++;
            // Hoán đổi arr[i] và arr[j]
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    // Đưa pivot về vị trí đúng
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return i + 1; // Trả về chỉ số của pivot
}

void quickSortHelper(int *arr, int low, int high)
{
    if (low < high)
    {
        // Chia mảng và lấy chỉ số phân vùng
        int pivotIndex = partition(arr, low, high);

        // Đệ quy sắp xếp mảng con bên trái và bên phải
        quickSortHelper(arr, low, pivotIndex - 1);
        quickSortHelper(arr, pivotIndex + 1, high);
    }
}

void quickSort(int *arr, int n)
{
    quickSortHelper(arr, 0, n - 1);
}
void MergeBatches(int **batches, int *batchSizes, int batchCount, int totalSize)
{
    // Khởi tạo Tournament Tree với giá trị đầu tiên từ mỗi batch
    int *initialValues = (int *)malloc(batchCount * sizeof(int));
    int *batchIndexes = (int *)calloc(batchCount, sizeof(int)); // Mảng theo dõi chỉ số hiện tại của mỗi batch (Kho tao bang 0)

    for (int i = 0; i < batchCount; i++)
    {
        initialValues[i] = batches[i][0];
    }

    TournamentTree *tree = initTournamentTree(initialValues, batchCount);

    // Kết quả hợp nhất
    int *result = (int *)malloc(totalSize * sizeof(int));

    for (int i = 0; i < totalSize; i++)
    {
        result[i] = popRoot(tree, batchIndexes, batches, batchSizes);
    }

    // In kết quả
    printf("Merged result:\n");
    for (int i = 0; i < totalSize; i++)
    {
        printf("%d ", result[i]);
    }
    printf("\n");

    // Giải phóng bộ nhớ
    free(result);
    free(initialValues);
    free(batchIndexes);
    freeTournamentTree(tree);
}
void Receive_Batches()
{
    esp_mesh_get_routing_table((mesh_addr_t *)&route_table,
                               CONFIG_MESH_ROUTE_TABLE_SIZE * 6, &route_table_size);
    int **batches = malloc(route_table_size * sizeof(int *));
    int batchSizes
    if (batches == NULL)
    {
        ESP_LOGE(TAG_SORT, "Memory allocation failed for batches");
        return;
    }
    for (int i = 0; i < route_table_size; i++)
    {
        batches[i] = malloc(20 * sizeof(int));
        if (i == 0)
        {
            int elements_to_send;
            get_send_array(batches[i], &elements_to_send);
            totalSize += elements_to_send;
        }
        else
        {
            send_mesh_each_string("ADD", &route_table[i]);
            vTaskDelay(50 / portTICK_PERIOD_MS);
            memcpy(batches[i], rx_arr_buf, 20 * sizeof(int));
        }
        if (batches[i] == NULL)
        {
            ESP_LOGE(TAG_SORT, "Memory allocation failed for row %d", i);
            // Giải phóng các hàng đã cấp phát trước đó
            for (int j = 0; j < i; j++)
                free(batches[j]);
            free(batches);
            return;
        }
    }

    ESP_LOGI(TAG_SORT, "Printing batches:");
    for (int i = 0; i < route_table_size; i++)
    {
        ESP_LOGI(TAG_SORT, "Row %d:", i);
        for (int j = 0; j < 20; j++) // In từng phần tử của hàng
        {
            printf("%d ", batches[i][j]);
        }
        printf("\n"); // Xuống dòng sau mỗi hàng
    }
    ESP_LOGI(TAG_SORT, "Total batches Size: %d", totalSize);
    totalSize = 0;
    MergeBatches(batches, )
    for (int i = 0; i < route_table_size; i++)
    {
        free(batches[i]);
    }
    free(batches);
}
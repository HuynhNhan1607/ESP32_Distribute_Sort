#pragma once
#include <stdbool.h>
#include "esp_mesh.h"

typedef struct
{
    char *id;
    char *mac;
} nodeEsp;

void send_mesh_all_string(char *data_t);
void send_mesh_each_string(char *data_t, mesh_addr_t *);
void task_mesh_rx_array();
void task_mesh_rx_command();
void send_int_array_mesh(int *int_array, int array_size);
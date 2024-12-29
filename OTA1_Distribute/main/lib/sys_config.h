
// #define WIFI_SSID "A10.14"
// #define WIFI_PASS "MMNT2004"

// #define WIFI_SSID "CEEC_Tenda"
// #define WIFI_PASS "1denmuoi1"

#define WIFI_SSID "S20 FE"
#define WIFI_PASS "25102004"

#define BROKER_HOST "test.mosquitto.org:1883"

#define MESH_ID_DEFINE {0x77, 0x77, 0x77, 0x77, 0x77, 0x78}

#define LED_BUILDING (27)
#define GPIO_OUTPUT_PIN_SEL (1ULL << LED_BUILDING)

#define BUTTON_PIN (0)
#define GPIO_INPUT_PIN_SEL (1ULL << BUTTON)

#define DEBUG 1

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_DISCONNECTED_BIT BIT2

#define CONFIG_MESH_CHANNEL 0

#define SERVER_IP "192.168.248.85" // Replace with server's IP address
#define SERVER_PORT 5002           // Port on which the server is listening

#define MIN(a, b) ((a) < (b) ? (a) : (b))
/* 
    MultiChannel 64 TCP socket client
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"


#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "lwip/api.h"



#include "driver/spi_slave.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/


#define WIFI_SSID "WiFiTest"
#define WIFI_PASS "201202203"

#define PORT 5001


// SPI USE

#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15

#define PIN_NUM_HANDSHAKE   21
#define PIN_NUM_HANDSHAKE_2 22

#define PIN_NUM_OTA 4

#define PARALLEL_LINES 16

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;


static const char *FIRMWARE_UPGRADE_URL = "https://192.168.1.178:8070/mc64_tcpclient.bin";
static const char *version = "version 1.0";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

const int IPV4_GOTIP_BIT = BIT0;
const int IPV6_GOTIP_BIT = BIT1;

static const char *TAG = "MC64";

#define buffSize  1024

DMA_ATTR uint8_t  spi_recvbuf[buffSize] = {0}; 
DMA_ATTR uint8_t  spi_sendbuf[buffSize] = {0};

uint8_t  tcp_recvbuf[buffSize] = {0};
#define HOST_IP_ADDR "192.168.1.100"  //TCP server的IP，对应上位机


int sock;
bool rcvFlag =false;
bool sendFlag =true;


spi_slave_transaction_t t;


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        /* enable ipv6 */
        tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, IPV4_GOTIP_BIT);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, IPV4_GOTIP_BIT);
        xEventGroupClearBits(wifi_event_group, IPV6_GOTIP_BIT);
        break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
        xEventGroupSetBits(wifi_event_group, IPV6_GOTIP_BIT);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP6");

        char *ip6 = ip6addr_ntoa(&event->event_info.got_ip6.ip6_info.ip);
        ESP_LOGI(TAG, "IPv6: %s", ip6);
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    uint8_t protocol;
   // ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_STA,WIFI_PROTOCOL_11B));//bg
    ESP_ERROR_CHECK(esp_wifi_get_protocol(ESP_IF_WIFI_STA,&protocol));
    ESP_LOGI(TAG, "protocol = %d", protocol);

    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void wait_for_ip()
{
    uint32_t bits = IPV4_GOTIP_BIT | IPV6_GOTIP_BIT ;

    ESP_LOGI(TAG, "Waiting for AP connection...");
    xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP");
}


//tcp 接收任务，通过标志位通知主任务
static void tcp_rcv(void *pvParameters){

    esp_err_t ret;

    while(1){
        
        int len = recv(sock, tcp_recvbuf, sizeof(tcp_recvbuf), 0);
        if (len < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }
        else if(len > 0) {
            rcvFlag = true;

        }   
    }
    vTaskDelete(NULL);


}

//主事件循环
static void tcp_client_task(void *pvParameters)
{

    esp_err_t ret;
    memset(&t, 0, sizeof(t));
    t.length = buffSize*8;
    t.tx_buffer = spi_sendbuf;
    t.rx_buffer = spi_recvbuf;

    char addr_str[128];
    int addr_family;
    int ip_protocol;

    int data_size = 0;
    const TickType_t xDelay = 1 / portTICK_PERIOD_MS;
    int j=0;
    unsigned int num =0;

    //配置套接字
    struct sockaddr_in destAddr;
    destAddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);
    //创建套接字
    sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);  
        return; 
    }
    ESP_LOGI(TAG, "Socket created");
    //连接至服务器（上位机）
    int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Successfully connected");  
    // for(int i=0;i<5;i++)
    //     send(sock, version, strlen(version), 0); 

    //建立接收任务并执行
    xTaskCreate(tcp_rcv,"tcp_rcv", 4096, NULL, 5, NULL);

    int num_rcv =1;
    while (1) {
 
        if(rcvFlag ==true){      //处理数据接收，数据由上位机发送，并通过SPI送出

                rcvFlag = false;
              //  printf("send1\r\n");
                memcpy(spi_sendbuf,tcp_recvbuf,buffSize);
                gpio_set_level(PIN_NUM_HANDSHAKE,0);  //下拉
                ret=spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY); //这里ESP32作为从机，等待主机(ARM)的时钟并将TCP的数据发送给ARM
                gpio_set_level(PIN_NUM_HANDSHAKE,1);  //上拉

                memset(tcp_recvbuf,0,buffSize);
                printf("send\r\n");
            }
         if (gpio_get_level(PIN_NUM_HANDSHAKE_2)==0) { //处理SPI的数据上发，当该握手线为低时触发

            printf("rcv\r\n");

            t.length=buffSize*8;
            t.tx_buffer=spi_sendbuf;
            t.rx_buffer=spi_recvbuf;
            gpio_set_level(PIN_NUM_HANDSHAKE,0);   //下拉
            ret=spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY);  //这里ESP32作为从机，等待主机(ARM)的时钟并接收SPI数据
            
            // for(int i=0;i<10;i++){
            //     printf("%x ",spi_recvbuf[i]);
            // }
            // printf("\r\n");
             int err = send(sock, spi_recvbuf, buffSize, 0);   //TCP发送
             if (err < 0) {
             ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
             break;
             }
            gpio_set_level(PIN_NUM_HANDSHAKE,1);  //上拉


            // printf("%d\r\n",num_rcv++);
           

         }
         else{
               vTaskDelay( xDelay );    
         }
    
    }
    if (sock != -1) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }

    

    vTaskDelete(NULL);
}

void spi_pre_transfer_callback()
{
   // int dc=(int)t->user;

  //  gpio_set_level(PIN_NUM_HANDSHAKE, 0);
}

void spi_post_transfer_callback()
{
   // int dc=(int)t->user;
   // gpio_set_level(PIN_NUM_HANDSHAKE, 1);
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}
static bool ota_check(void){

    gpio_config_t io_conf;
    io_conf.intr_type    = GPIO_PIN_INTR_DISABLE;
    io_conf.mode         = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_NUM_OTA);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "ota_check (5 sec)...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    bool ota_is_needed = gpio_get_level(PIN_NUM_OTA);

   gpio_reset_pin(PIN_NUM_OTA);
   return ota_is_needed;
}


static void do_ota(void){

    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "WiFiTest",
            .password = "201202203",
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    wait_for_ip();
    ESP_LOGI(TAG, "Connected to WiFi network! Attempting to connect to server...");   
    esp_http_client_config_t config = {
        .url = FIRMWARE_UPGRADE_URL,
        .cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
    };
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
//Called after a transaction is queued and ready for pickup by master.
void my_post_setup_cb(spi_slave_transaction_t *trans) {
 //   WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_HANDSHAKE));
}

//Called after transaction is sent/received. 
void my_post_trans_cb(spi_slave_transaction_t *trans) {
  //  WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_HANDSHAKE));
}
void app_main()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 1.OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // 2.NVS partition contains data in new format and cannot be recognized by this version of code.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // bool ota_flag = ota_check();
    // if(ota_flag){

    //     do_ota();
    // }
    esp_err_t ret;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
    };


    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=PIN_NUM_CS,
        .queue_size=3,
        .flags=0,
        .post_setup_cb=my_post_setup_cb,
        .post_trans_cb=my_post_trans_cb
    };

    gpio_config_t io_conf={
        .intr_type=GPIO_PIN_INTR_DISABLE ,
        .mode=GPIO_MODE_OUTPUT,
        .pull_up_en=1,
        .pin_bit_mask=(1<<PIN_NUM_HANDSHAKE)
    }; 
    gpio_config(&io_conf);

      gpio_config_t io_conf_2={
        .intr_type=GPIO_PIN_INTR_DISABLE ,
        .mode=GPIO_MODE_INPUT,
        .pull_up_en=1,
        .pin_bit_mask=(1<<PIN_NUM_HANDSHAKE_2)
    }; 
    gpio_config(&io_conf_2);

    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS, GPIO_PULLUP_ONLY);
    gpio_set_level(PIN_NUM_HANDSHAKE,1);  


    memset(&t, 0, sizeof(t));
    t.length = buffSize*8;
    t.tx_buffer = spi_sendbuf;
    t.rx_buffer = spi_recvbuf; 




    ret=spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 1);
        ESP_ERROR_CHECK(ret);

    //xSemaphoreGive( hsSem );
    initialise_wifi();
    wait_for_ip();

    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);


}

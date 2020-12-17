# 脉搏ESP32程序



## 功能

该程序基于TCP，为脉搏采集仪提供数据传输能力。

## 程序主要流程

### 初始化

该程序的初始化主要包括以下步骤

* 系统底层的初始化
* SPI从机的初始化
* GPIO的初始化
* WFI的初始化

程序入口：

```c
app_main（）
```

#### 系统底层的初始化

```c 
nvs_flash_init();
```

#### SPI从机的初始化

```c 
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
ret=spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 1);
```

#### GPIO的初始化

```c 
    gpio_config_t io_conf={
        .intr_type=GPIO_PIN_INTR_DISABLE ,
        .mode=GPIO_MODE_OUTPUT,
        .pull_up_en=1,
        .pin_bit_mask=(1<<PIN_NUM_HANDSHAKE)
    }; 
    gpio_config(&io_conf);
```

#### WIFI的初始化

````C 
    initialise_wifi();
    wait_for_ip();
````

协议栈的具体初始化过程和WIFI配置请自行参考官方例程。

---

### Tcp 客户端任务处理

程序入口：

````c
void tcp_client_task(void *pvParameters)
````

1. 配置socket 并连接至tcp 服务器端 
2. 处理数据接收和发送操作
3. 注意两根握手线的判断逻辑
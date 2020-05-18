#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_system.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <driver/adc.h>
#include "sdkconfig.h"
#include "DHT.h"


//static const char *TAG = "adc1";
static adc1_channel_t channel = ADC1_CHANNEL_6;
static const char *TAG = "DHT";

void adc1_task(void *ignore);

void DHT_task(void *pvParameter)
{
    setDHTgpio(GPIO_NUM_4);
    ESP_LOGI(TAG, "Starting DHT Task\n\n");

    while (1)
    {
        ESP_LOGI(TAG, "=== Reading DHT ===\n");
        int ret = readDHT();

        errorHandler(ret);

        ESP_LOGI(TAG, "Hum: %.1f Tmp: %.1f\n", getHumidity(), getTemperature());

        // -- wait at least 2 sec before reading again ------------
        // The interval of whole process must be beyond 2 seconds !!
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

void app_main()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_log_level_set("*", ESP_LOG_INFO);

    xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);
    xTaskCreate(&adc1_task,"adc1_task", 2048, NULL, 5, NULL);
   
}
void adc1_task(void *ignore){
        ESP_LOGI(TAG,">>ADC");
        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(channel, ADC_ATTEN_6db);
        while(1){
            int value = adc1_get_voltage(channel);
            ESP_LOGI(TAG,"value:%d",value);
            vTaskDelay(2000/portTICK_RATE_MS);
        }
        vTaskDelete(NULL);
}

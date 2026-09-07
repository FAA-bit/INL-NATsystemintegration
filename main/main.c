#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"

#define DHT11_GPIO 4

void app_main(void)
{
    dht11_data_t data;

    dht11_init(DHT11_GPIO);

    while (1)
    {
        esp_err_t result = dht11_read(&data);

        if (result == ESP_OK)
        {
            printf(
                "Temperature: %.1f C | Humidity: %.1f %%\n",
                data.temperature,
                data.humidity
            );
        }
        else
        {
            printf(
                "DHT11 read failed: %s\n",
                esp_err_to_name(result)
            );
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
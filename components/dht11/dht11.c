#include "dht11.h"

#include "driver/gpio.h"
#include "esp_rom_sys.h"

#define DHT11_START_LOW_MS       20
#define DHT11_RESPONSE_TIMEOUT_US 100
#define DHT11_BIT_TIMEOUT_US      100
#define DHT11_BITS                40

static int dht11_gpio = -1;

static esp_err_t wait_for_level(int level, int timeout_us)
{
    while (timeout_us > 0)
    {
        if (gpio_get_level(dht11_gpio) == level)
        {
            return ESP_OK;
        }

        esp_rom_delay_us(1);
        timeout_us--;
    }

    return ESP_ERR_TIMEOUT;
}

esp_err_t dht11_init(int gpio_num)
{
    dht11_gpio = gpio_num;

    gpio_config_t config = {
        .pin_bit_mask = (1ULL << gpio_num),
        .mode = GPIO_MODE_OUTPUT_OD,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    return gpio_config(&config);
}

esp_err_t dht11_read(dht11_data_t *data)
{
    if (data == NULL || dht11_gpio < 0)
    {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t bytes[5] = {0};

    /*
     * Start signal:
     * ESP32 pulls the data line LOW for at least 18 ms.
     */
    gpio_set_level(dht11_gpio, 0);
    esp_rom_delay_us(DHT11_START_LOW_MS * 1000);

    /*
     * Release the line and wait for the DHT11 response.
     */
    gpio_set_level(dht11_gpio, 1);
    esp_rom_delay_us(30);

    gpio_set_direction(dht11_gpio, GPIO_MODE_INPUT);

    /*
     * DHT11 response:
     * LOW for about 80 us
     * HIGH for about 80 us
     */
    if (wait_for_level(0, DHT11_RESPONSE_TIMEOUT_US) != ESP_OK)
    {
        gpio_set_direction(dht11_gpio, GPIO_MODE_OUTPUT_OD);
        return ESP_ERR_TIMEOUT;
    }

    if (wait_for_level(1, DHT11_RESPONSE_TIMEOUT_US) != ESP_OK)
    {
        gpio_set_direction(dht11_gpio, GPIO_MODE_OUTPUT_OD);
        return ESP_ERR_TIMEOUT;
    }

    if (wait_for_level(0, DHT11_RESPONSE_TIMEOUT_US) != ESP_OK)
    {
        gpio_set_direction(dht11_gpio, GPIO_MODE_OUTPUT_OD);
        return ESP_ERR_TIMEOUT;
    }

    /*
     * Read 40 bits = 5 bytes.
     */
    for (int i = 0; i < DHT11_BITS; i++)
    {
        /*
         * Each bit starts with approximately 50 us LOW.
         */
        if (wait_for_level(1, DHT11_BIT_TIMEOUT_US) != ESP_OK)
        {
            gpio_set_direction(dht11_gpio, GPIO_MODE_OUTPUT_OD);
            return ESP_ERR_TIMEOUT;
        }

        /*
         * Measure how long the signal stays HIGH.
         */
        int high_time = 0;

        while (gpio_get_level(dht11_gpio) == 1 &&
               high_time < DHT11_BIT_TIMEOUT_US)
        {
            esp_rom_delay_us(1);
            high_time++;
        }

        /*
         * A short HIGH pulse represents 0.
         * A longer HIGH pulse represents 1.
         */
        int bit = (high_time > 40) ? 1 : 0;

        bytes[i / 8] <<= 1;
        bytes[i / 8] |= bit;
    }

    gpio_set_direction(dht11_gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dht11_gpio, 1);

    /*
     * DHT11 checksum:
     * byte 0 + byte 1 + byte 2 + byte 3
     * must equal byte 4.
     */
    uint8_t checksum =
        bytes[0] + bytes[1] + bytes[2] + bytes[3];

    if (checksum != bytes[4])
    {
        return ESP_ERR_INVALID_CRC;
    }

    data->humidity = bytes[0] + bytes[1] * 0.1f;
    data->temperature = bytes[2] + bytes[3] * 0.1f;

    return ESP_OK;
}

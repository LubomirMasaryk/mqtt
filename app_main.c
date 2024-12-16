#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "button.hpp"
#include "led.hpp"

#define WIFI_SSID "Tenda_CACDA0"
#define WIFI_PASSWORD "lakemilk729"

#define MQTT_BROKER_IP "46.36.41.235"
#define MQTT_PORT 1883

#define MQTT_TOPIC_SEND "/p7cd2vr9m0TdKxfU2oVA/room1"
#define MQTT_TOPIC_RECEIVE "/p7cd2vr9m0TdKxfU2oVA/room2"

static const char *TAG = "MQTT_LED";

static esp_mqtt_client_handle_t mqtt_client;

Led leds[4] = {
    Led(GPIO_NUM_18),
    Led(GPIO_NUM_17),
    Led(GPIO_NUM_16),
    Led(GPIO_NUM_19)
};

Button buttons[4] = {
    Button(GPIO_NUM_12),
    Button(GPIO_NUM_13),
    Button(GPIO_NUM_14),
    Button(GPIO_NUM_15)
};

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            esp_mqtt_client_subscribe(mqtt_client, MQTT_TOPIC_RECEIVE, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT disconnected");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA received");
            char topic[event->topic_len + 1];
            char data[event->data_len + 1];
            strncpy(topic, event->topic, event->topic_len);
            topic[event->topic_len] = '\0';
            strncpy(data, event->data, event->data_len);
            data[event->data_len] = '\0';

            if (strcmp(topic, MQTT_TOPIC_RECEIVE) == 0) {
                int led_index;
                char command[10];
                if (sscanf(data, "%d %s", &led_index, command) == 2 && led_index >= 0 && led_index < 4) {
                    if (strcmp(command, "on") == 0) {
                        leds[led_index].turnOn();
                    } else if (strcmp(command, "off") == 0) {
                        leds[led_index].turnOff();
                    } else if (strcmp(command, "blink") == 0) {
                        leds[led_index].toggle();
                        vTaskDelay(500 / portTICK_PERIOD_MS);
                        leds[led_index].toggle();
                    }
                }
            }
            break;
        default:
            ESP_LOGI(TAG, "Other MQTT event id: %d", event_id);
            break;
    }
}

void button_task(void *param) {
    while (1) {
        for (int i = 0; i < 4; i++) {
            if (buttons[i].getPress()) {
                char message[20];
                snprintf(message, sizeof(message), "%d on", i);
                esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_SEND, message, 0, 0, 0);
                
                while (buttons[i].isPressed()) {
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }

                snprintf(message, sizeof(message), "%d off", i);
                esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_SEND, message, 0, 0, 0);
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    ESP_LOGI(TAG, "Setting up WiFi...");
    // WiFi initialization code here

    ESP_LOGI(TAG, "Setting up MQTT...");
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://" MQTT_BROKER_IP ":1883",
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "Setting up LED and button control...");

    xTaskCreate(button_task, "button_task", 2048, NULL, 5, NULL);
}

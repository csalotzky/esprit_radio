#include <FS.h>
#include "tasks.h"
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"
#include "gui.h"
#include "radio.h"
#include "inputs.h"
#include "SPIFFS.h"


#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23
#define TFT_BL              4   // Display backlight control pin

#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_1            35
#define BUTTON_2            0

#define BATT_MAX            4.00
#define BATT_MIN            3.30
 

//TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Gui gui;
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

Radio radio;

int vref = 1100;

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void BatteryLoop(void* params){
    while(true) {
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        uint8_t percentage;

        if (battery_voltage <= BATT_MIN) percentage = 0;
        else if (battery_voltage >= BATT_MAX) percentage = 100;
        else percentage = (battery_voltage - BATT_MIN)*100.0 / (BATT_MAX - BATT_MIN);

        gui.DisplayBattery(percentage);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESPRIT RADIO | Start");
    Serial.println();

    /* DEEP SLEEP CONFIG */
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_27,1);
  
    /* MOUNT SPIFFS */
    if(!SPIFFS.begin(true)) Serial.println("SPIFFS | An Error has occurred while mounting SPIFFS");  

    /* INIT SEMAPHORES */
    i2cSemaphore = xSemaphoreCreateMutex();
    spiSemaphore = xSemaphoreCreateMutex();

    /* INIT GUI */
    gui.InitScreen();

    /* BATTERY TASK */
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);
    xTaskCreate(BatteryLoop,"Battery voltage read",2048,NULL,2,NULL);

    /* INIT RADIO */
    radio.GuiRef = &gui;
    radio.InitTuner();

    /* INIT BUTTONS */
    vTaskDelay(100);
    InputsInit(&gui, &radio);
    pinMode(GPIO_NUM_27,INPUT_PULLDOWN);
}

void loop() {}
// C Header file having Standard Input Output Functions
#include <stdio.h>

// Header files for Free RTOS - Real Time Operating Systems
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// SRA's custom header file including additional functions
#include "sra_board.h"

// tested Margin Value Constants for Black and White Surfaces
#define BLACK_MARGIN 400
#define WHITE_MARGIN 2000

// targetted Mapping Value Constants for Tested Margin Values
#define CONSTRAIN_LSA_LOW 0
#define CONSTRAIN_LSA_HIGH 1000

// pointer to a character array 
static const char* TAG = "LSA_READINGS";

// function to display reading on OLED
void display_lsa(line_sensor_array readings,u8g2_t *oled_config)
{
    // clear the screen
    u8g2_ClearBuffer(oled_config);

    // plot the bar of LSA 0
    u8g2_DrawFrame(oled_config, 27,0,10,30);
    u8g2_DrawBox(oled_config, 27,0,10,readings.adc_reading[0]*0.03);

    // plot the bar of LSA 1
    u8g2_DrawFrame(oled_config, 47,0,10,30);
    u8g2_DrawBox(oled_config, 47,0,10,readings.adc_reading[1]*0.03);

    // plot the bar of LSA 2
    u8g2_DrawFrame(oled_config,67,0,10,30);
    u8g2_DrawBox(oled_config,67,0,10,readings.adc_reading[2]*0.03);

    // plot the bar of LSA 3
    u8g2_DrawFrame(oled_config, 87,0,10,30);
    u8g2_DrawBox(oled_config, 87,0,10,readings.adc_reading[3]*0.03);

    // Sends the buffer to the OLED Display
	u8g2_SendBuffer(oled_config);
}

// main driver function
void app_main(void)
{
    // enable line sensor after checking optimal working state of ESP
    ESP_ERROR_CHECK(enable_line_sensor());

    // Declaring the required OLED struct
    u8g2_t oled_config;
    // Initialising the OLED
	ESP_ERROR_CHECK(init_oled(&oled_config));

    // Union containing line sensor readings
    line_sensor_array line_sensor_readings;
	
    // infinite loop to get LSA readings continuously
    while(1)
    {
        // get line sensor readings from the LSA sensors
        line_sensor_readings = read_line_sensor();
        for(int i = 0; i < 4; i++)
        {
            // constrain lsa readings between BLACK_MARGIN and WHITE_MARGIN
            line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], BLACK_MARGIN, WHITE_MARGIN);
            // map readings from (BLACK_MARGIN, WHITE_MARGIN) to (CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH)
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], BLACK_MARGIN, WHITE_MARGIN, CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH);
        }
      
        // Displaying Information logs - final lsa readings 
        ESP_LOGI(TAG, "LSA_1: %d \t LSA_2: %d \t LSA_3: %d \t LSA_4: %d",line_sensor_readings.adc_reading[0], line_sensor_readings.adc_reading[1], line_sensor_readings.adc_reading[2], line_sensor_readings.adc_reading[3]);

        // Displaying Readings on OLED Display
        display_lsa(line_sensor_readings,&oled_config);

        // delay of 1s after each log
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

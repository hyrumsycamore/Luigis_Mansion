#include <stdio.h>
#include <stdint.h>
#include "joy.h"
#include "esp_adc/adc_oneshot.h"
// Channels
#define ADC1_CHANX ADC_CHANNEL_6
#define ADC1_CHANY ADC_CHANNEL_7
#define ADC1_ATTEN ADC_ATTEN_DB_12 // The attenuation of the channels

#define NUM_SAMPLES 4
// Average center point of the released joystick
int32_t x_center;
int32_t y_center;
static adc_oneshot_unit_handle_t adc1_handle;

// Initializes the joystick to read x and y values.
int32_t joy_init()
{
    // Configure the oneshot unit
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    //  Configure Channels
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC1_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHANX, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHANY, &config));
    
    // Calibration
    int32_t sum_x = 0;
    int32_t sum_y = 0;
    // Takes and averages 4 samples from the adc
    for (int8_t i = 0; i < NUM_SAMPLES; i++)
    {
        // Intermediate data to hold values from the adc. 
        int_fast32_t raw_y = 0;
        int_fast32_t raw_x = 0;
        adc_oneshot_read(adc1_handle, ADC1_CHANX, &raw_x);
        adc_oneshot_read(adc1_handle, ADC1_CHANY, &raw_y);
        // Adding values for a short term sum
        sum_x += raw_x;
        sum_y += raw_y;
    }
    x_center = (sum_x / NUM_SAMPLES);
    y_center = (sum_y / NUM_SAMPLES);
    return 0;
}
// Deconstructs a constructed joystick
int32_t joy_deinit()
{
    // Checks if the joystick has been initialized
    if (adc1_handle != NULL)
    {
        ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
        adc1_handle = NULL; // removes the pointer
        return 0;// Signifies success
    }
    return 1; // Signifies a failure
}
/* Finds the displacement of the joystick by reading the x and y values from the
adc and subtracting those from a center value to find the displacement.
*/
void joy_get_displacement(int32_t *dcx, int32_t *dcy)
{
    // Intermediate data to hold values from the adc. 
    int_fast32_t rx;
    int_fast32_t ry;
    adc_oneshot_read(adc1_handle, ADC1_CHANX, &rx);
    adc_oneshot_read(adc1_handle, ADC1_CHANY, &ry);
    // Changing the values in the dcx and dcy
    *dcx = rx-x_center;
    *dcy = ry-y_center;
}
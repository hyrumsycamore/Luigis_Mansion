#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "graphic.h"
#include "lcd.h"
#include "pin.h"
#include "hw.h"
#include "config.h"
#include "cursor.h"
#include "sound.h"
#include "player.h"
#include "flashlight.h"
#include "game.h"
#include "com.h"
#include "mixkit_arcade_retro.h"

static const char *TAG = "lab07";

// update period in ms
#define PER_MS ((uint32_t)(CONFIG_GAME_TIMER_PERIOD * 1000))
#define TIME_OUT 500

#define CHK_RET(x) ({                                       \
	int32_t ret_val = (x);                                  \
	if (ret_val != 0)                                       \
	{                                                       \
		ESP_LOGE(TAG, "FAIL: return %ld, %s", ret_val, #x); \
	}                                                       \
	ret_val;                                                \
})

TimerHandle_t update_timer; // declare timer handle
volatile bool interrupt_flag;
uint32_t isr_triggered_count;
uint32_t isr_handled_count;

// Interrupt handler for game - use flag method
void update()
{
	interrupt_flag = true;
	isr_triggered_count++;
}

// Main application
void app_main(void)
{
	// ISR flag and counts
	interrupt_flag = false;
	isr_triggered_count = 0;
	isr_handled_count = 0;

	// initialization
	lcd_init();
	ESP_LOGI(TAG, "Starting");
	lcd_frameEnable();
	// game init
	com_init();
	sound_init(MIXKIT_ARCADE_RETRO_SAMPLE_RATE);
	game_init();

	// Configure I/O pins for buttons
	pin_reset(HW_BTN_A);
	pin_input(HW_BTN_A, true);
	pin_reset(HW_BTN_B);
	pin_input(HW_BTN_B, true);
	pin_reset(HW_BTN_MENU);
	pin_input(HW_BTN_MENU, true);
	pin_reset(HW_BTN_OPTION);
	pin_input(HW_BTN_OPTION, true);
	pin_reset(HW_BTN_SELECT);
	pin_input(HW_BTN_SELECT, true);
	pin_reset(HW_BTN_START);
	pin_input(HW_BTN_START, true);

	// Initialize update timer
	update_timer = xTimerCreate(
		"update_timer",		   // Text name for the timer.
		pdMS_TO_TICKS(PER_MS), // The timer period in ticks.
		pdTRUE,				   // Auto-reload the timer when it expires.
		NULL,				   // No need for a timer ID.
		update				   // Function called when timer expires.
	);
	if (update_timer == NULL)
	{
		ESP_LOGE(TAG, "Error creating update timer");
		return;
	}
	if (xTimerStart(update_timer, pdMS_TO_TICKS(TIME_OUT)) != pdPASS)
	{
		ESP_LOGE(TAG, "Error starting update timer");
		return;
	}

	// Main game loop
	uint64_t tmax = 0;	   // For hardware timer values
	while (pin_get_level(HW_BTN_MENU)) // while MENU button not pressed
	{
		while (!interrupt_flag)
			;
		interrupt_flag = false;
		isr_handled_count++;
		game_tick();
	
	}
	printf("Handled %lu of %lu interrupts\n", isr_handled_count, isr_triggered_count);
	printf("WCET us:%llu\n", tmax);
	sound_deinit();
}

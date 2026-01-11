#include <driver/uart.h>
#include "hw.h"
#include "pin.h"
#include "com.h"
#include <string.h>

#define OFF_SET 32
#define UNPACK  0xFFFFFFFF

const uart_port_t uart_num = UART_NUM_2; // initialize handle

// Initialize the communication channel.
// Return zero if successful, or non-zero otherwise.
int32_t com_init(void)
{
    uart_config_t uart_config = {
        // configure handle
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE, // none
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // flow control disable
        .source_clk = UART_SCLK_DEFAULT,       // source clock default
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    // Set UART pins(TX: IO17, RX: IO16, RTS: , CTS: )
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, HW_EX8, HW_EX7, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, UART_HW_FIFO_LEN(PORT_NUM) * 2, 0, 0, NULL, 0));
    bool enable = true;
    if (enable)
    { // configure hw_ex7 as a pullup
        pin_pullup(HW_EX7, enable);
    }
    return 0;
}

// Free resources used for communication.
// Return zero if successful, or non-zero otherwise.
int32_t com_deinit(void)
{
    if (uart_is_driver_installed(UART_NUM_2))
    { // is driver installed
        uart_driver_delete(UART_NUM_2);
        return 0;
    }
    return -1;
}

// Write data to the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to write
// Return number of bytes written, or negative number if error.
int32_t com_write(const void *buf, uint32_t size)
{
    return uart_tx_chars(UART_NUM_2, buf, size);
}

// Read data from the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to read
// Return number of bytes read, or negative number if error.
int32_t com_read(void *buf, uint32_t size)
{
    return uart_read_bytes(UART_NUM_2, buf, size, 0);
}

//send player position
void com_send_pos(coord_t x, coord_t y)
{
    uint64_t packed = ((uint64_t)((coord_t)x) << OFF_SET) | (coord_t)y;
    com_write((uint64_t *)&packed, sizeof(packed));
}

//used to check id player position is recieved
bool com_recieve_pos(coord_t *x, coord_t *y)
{
    uint64_t packed;
    if (com_read((uint64_t *)&packed, sizeof(packed)) == sizeof(packed))
    {
        *x = (coord_t)(packed >> OFF_SET);
        *y = (coord_t)(packed & UNPACK);
        return true;
    }
    return false;
}

//used to send bool
void com_send_bool(bool button)
{
    uint8_t packed = button;
    com_write((uint8_t *)&packed, sizeof(packed));
}

//used to check if bool was recieved
bool com_recieve_bool(bool *button)
{
    uint8_t packed;
    if (com_read((uint8_t *)&packed, sizeof(packed)) == sizeof(packed))
    {//checks if can be read
        *button = packed;
        return true;
    }
    return false;
}


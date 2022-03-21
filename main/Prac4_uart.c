/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "myUart.h"

#define UART_NUM        (0)
#define UART_RX_PIN     (3)
#define UART_TX_PIN     (1)

#define UARTS_BAUD_RATE         (115200)
#define TASK_STACK_SIZE         (1048)
#define READ_BUF_SIZE           (1024)

/* Message printed by the "consoletest" command.
 * It will also be used by the default UART to check the reply of the second
 * UART. As end of line characters are not standard here (\n, \r\n, \r...),
 * let's not include it in this string. */
const char test_message[] = "This is an example string, if you can read this, the example is a success!";

/**
 * @brief Configure and install the default UART, then, connect it to the
 * console UART.
 */
void uartInit(uart_port_t uart_num, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop, uint8_t txPin, uint8_t rxPin)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UARTS_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(uart_num, READ_BUF_SIZE, READ_BUF_SIZE, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, txPin, rxPin,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

}

void delayMs(uint16_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void uartClrScr(uart_port_t uart_num)
{
    // Uso "const" para sugerir que el contenido del arreglo lo coloque en Flash y no en RAM
    const char caClearScr[] = "\e[2J";
    uart_write_bytes(uart_num, caClearScr, sizeof(caClearScr));
}
void uartGoto11(uart_port_t uart_num)
{
    // Limpie un poco el arreglo de caracteres, los siguientes tres son equivalentes:
     // "\e[1;1H" == "\x1B[1;1H" == {27,'[','1',';','1','H'}
    const char caGoto11[] = "\e[1;1H";
    uart_write_bytes(uart_num, caGoto11, sizeof(caGoto11));
}

bool uartKbhit(uart_port_t uart_num)
{
    uint8_t length;
    uart_get_buffered_data_len(uart_num, (size_t*)&length);
    return (length > 0);
}

void uartPutchar(uart_port_t uart_num, char c)
{
    uart_write_bytes(uart_num, &c, sizeof(c));
}

char uartGetchar(uart_port_t uart_num)
{
    char c;
    // Wait for a received byte
    while(!uartKbhit(uart_num))
    {
        delayMs(10);
    }
    // read byte, no wait
    uart_read_bytes(UART_NUM, &c, sizeof(c), 0);

    return c;
}
void app_main(void)
{
// The following is only example code, delete this and implement
// what is inside the TO_IMPLEMENT check
    char payload[] = "Hola mundo!";

    uartInit(0, 115200, 8, 0, 1, UART_TX_PIN, UART_RX_PIN);
    delayMs(500);
    uartGoto11(UART_NUM);
    uartClrScr(UART_NUM);

    uartPutchar(UART_NUM,payload[0]);
    uartPutchar(UART_NUM,payload[1]);
    uartPutchar(UART_NUM,payload[2]);
    uartPutchar(UART_NUM,payload[3]);
    uartPutchar(UART_NUM,payload[10]);
    
    // Wait for input
    delayMs(500);
    
    // echo forever
    while(1)
    {
        uartPutchar(UART_NUM,uartGetchar(UART_NUM));
    }

#ifdef TO_IMPLEMENT
    char cad[20];
    char cadUart3[20];
    uint16_t num;

    uartInit(0,12345,8,1,2);
    uartInit(1,115200,8,0,1);
    uartInit(2,115200,8,0,1);
    while(1) 
    {
        uartGetchar(0);
        uartClrScr(0);

        uartGotoxy(0,2,2);
        uartSetColor(0,YELLOW);
        uartPuts(0,"Introduce un número:");
        
        uartGotoxy(0,22,2);
        uartSetColor(0,GREEN);
        uartGets(0,cad);
// For the following code to work, TX1 must be physically 
// connected with a jumper wire to RX2
// -------------------------------------------
        // Cycle through UART1->UART2
        uartPuts(1,cad);
        uartPuts(1,"\r");
        uartGets(2,cadUart3);
        uartGotoxy(0,5,3);
        uartPuts(0,cadUart3);
// -------------------------------------------
        num = myAtoi(cad);
        myItoa(num,cad,16);
        
        uartGotoxy(0,5,4);
        uartSetColor(0,BLUE);
        uartPuts(0,"Hex: ");
        uartPuts(0,cad);
        myItoa(num,cad,2);
        
        uartGotoxy(0,5,5);
        uartPuts(0,"Bin: ");
        uartPuts(0,cad);
    }
#endif

}
